
extern "C" {
    #include <sys/unistd.h>
    #include <sys/stat.h>
    #include "esp_vfs_fat.h"
    #include "sdmmc_cmd.h"
    #include "driver/sdmmc_host.h"
    #include <cstring>
    #include "driver/gpio.h"
}

#include "sdcard.hpp"




static SemaphoreHandle_t sdCardSemaphore;
static FILE *_file;
static sdmmc_card_t *_card;

static SDCardImp sd;
SDCard *sdcard = &sd;

AutoLock::AutoLock(SemaphoreHandle_t &semaphore){
    if(semaphore == NULL){
        semaphore = xSemaphoreCreateRecursiveMutex();
    }
    this->_semaphore = semaphore;
    xSemaphoreTakeRecursive(semaphore, portMAX_DELAY);
}

AutoLock::~AutoLock(){
    xSemaphoreGiveRecursive(this->_semaphore);
}


void led_on(uint8_t state)
{
    const int GPIO_OUTPUT_IO_LED = 4;
    gpio_set_level((gpio_num_t)GPIO_OUTPUT_IO_LED, state);
}

void SDCardImp::initGPIO()
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << GPIO_NUM_4);
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
}

int SDCardImp::getFileSize(const char * file_name)
{
    struct stat st;
    if (this->mounted && stat( file_name, &st) == 0) {
        return st.st_size;
    }
    return -1;
}

int SDCardImp::getFileTime(const char *file_name)
{
    struct stat st;
    if (stat(file_name, &st) == 0) {
        return st.st_ctime;
    }
    return -1;
}


bool SDCardImp::begin()
{
    if(this->mounted)return true;
    ENTER_CRITICAL(sdCardSemaphore);
    initGPIO();
    if(this->mounted)return true;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
    .format_if_mount_failed = false,
    .max_files = 2,
    .allocation_unit_size = SECTOR_SIZE,
    .disk_status_check_enable = false
    };
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
#ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
    slot_config.width = 4;
#else
    slot_config.width = 1;
#endif
    this->mounted = esp_vfs_fat_sdmmc_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &_card) == ESP_OK;

    return this->mounted;
}

bool SDCardImp::renameFile(const char *new_name, const char *old_name)
{
    ENTER_CRITICAL(sdCardSemaphore);
    if(!this->mounted && !begin())return false;
    struct stat st;
    if (stat(new_name, &st) == 0) {
        unlink(new_name);
    }
    return rename(old_name, new_name) == 0;
}

bool SDCardImp::openReadFile(const char* file_name, bool bin)
{
    return openFile(file_name, bin ? "rb":"r");
}


bool SDCardImp::openFile(const char*  file_name, const char* mode)
{
    if(!this->mounted && !begin())return false;
    char name_buf[100] = {0};
    snprintf(name_buf, 100, "%s/%s", MOUNT_POINT,  file_name);
    _file = fopen(name_buf, mode);
    return _file != NULL;
}

bool SDCardImp::openWriteFile(const char* file_name, bool bin, bool append=false)
{
    char mode[3] = { append ? 'a' : 'w', bin ? 'b' : '\0' };
    return openFile(file_name, mode);
}

int SDCardImp::writeBinFile(const char * file_name, uint8_t *data, uint32_t data_size)
{
    ENTER_CRITICAL(sdCardSemaphore);
    int bc = 0;
    if(openWriteFile( file_name, true, false)){
        bc = fwrite(data, sizeof(uint8_t), data_size, _file);
        closeFile();
    }
    return bc;
}

int SDCardImp::writeTextFile(const char * file_name, char *str, bool append)
{
    ENTER_CRITICAL(sdCardSemaphore);
    int bc = 0;
    if(openWriteFile( file_name, false, append)){
        bc = fwrite(str, sizeof(uint8_t), strlen(str), _file);
        closeFile();
    }
    return bc;
}

int SDCardImp::readBinFile(uint8_t *file_buf, uint32_t file_size)
{
    return fread(file_buf, sizeof(uint8_t), file_size, _file);
}

bool SDCardImp::writeTextToFile(bool append, const char *file_name, const char* format, ...)
{
    if(openWriteFile(file_name, false, append)){
        va_list args;
        va_start(args, format);
        vfprintf(_file, format, args);
        va_end(args);
        closeFile();
        return true;
    }
    return false;
}


bool SDCardImp::format()
{
    ENTER_CRITICAL(sdCardSemaphore);
    if(!this->mounted && !begin())return false;
    return esp_vfs_fat_sdcard_format(MOUNT_POINT, _card) == 0;
}

uint8_t* SDCardImp::prepareBufer(uint8_t *&buf, uint32_t file_size)
{
    if(buf){
        delete [] buf;
        buf = NULL;
    }
    buf = new uint8_t [file_size];
    return buf;
}


int SDCardImp::readFile(const char * file_name, uint8_t *&buf, uint32_t buf_size)
{
    ENTER_CRITICAL(sdCardSemaphore);
    int bc = 0;
    const uint32_t actual_file_size = getFileSize(file_name);
    if(actual_file_size && openReadFile( file_name, true)){
        if(actual_file_size > buf_size || buf == NULL){
            prepareBufer(buf, actual_file_size);
        }
        bc = readBinFile(buf, actual_file_size);
        closeFile();
    }
    return bc;
}


void SDCardImp::closeFile()
{
    ENTER_CRITICAL(sdCardSemaphore);
    fclose(_file);
    led_on(false);
}


void SDCardImp::printInfo()
{
    ENTER_CRITICAL(sdCardSemaphore);
    if(!this->mounted)return;
    sdmmc_card_print_info(stdout, _card);
}

int SDCardImp::deleteFile(const char* file_name)
{
    ENTER_CRITICAL(sdCardSemaphore);
    if(!this->mounted)return -1;
    return unlink(file_name);
}






