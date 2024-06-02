
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


esp_err_t led_on(uint8_t state)
{
    const int GPIO_OUTPUT_IO_LED = 4;
    return gpio_set_level((gpio_num_t)GPIO_OUTPUT_IO_LED, state);
}

esp_err_t SDCardImp::initGPIO()
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << GPIO_NUM_4);
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    return gpio_config(&io_conf);
}

size_t SDCardImp::getFileSize(const char * file_name)
{
    struct stat st;
    size_t size = 0;
    if (this->mounted && stat( file_name, &st) == 0) {
        size = st.st_size;
    }
    return size;
}

time_t SDCardImp::getFileTime(const char *file_name)
{
    struct stat st;
    time_t time = 0;
    if (stat(file_name, &st) == 0) {
        time = st.st_ctime;
    }
    return time;
}


esp_err_t SDCardImp::begin()
{

    ENTER_CRITICAL(sdCardSemaphore);
    if(this->mounted)return true;
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
    esp_err_t res = esp_vfs_fat_sdmmc_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &_card);
    this->mounted = res == ESP_OK;
    return res;
}

esp_err_t SDCardImp::renameFile(const char *new_name, const char *old_name)
{
    ENTER_CRITICAL(sdCardSemaphore);
    if(!this->mounted && begin() != ESP_OK)return false;
    struct stat st;
    if (stat(new_name, &st) == 0) {
        unlink(new_name);
    }
    return rename(old_name, new_name) == 0 ? ESP_OK : ESP_FAIL;
}

esp_err_t SDCardImp::openReadFile(const char* file_name, bool bin)
{
    return openFile(file_name, bin ? "rb":"r") == 0 ? ESP_OK : ESP_FAIL;
}


esp_err_t SDCardImp::openFile(const char*  file_name, const char* mode)
{
    if(!this->mounted && begin() != ESP_OK)return false;
    char name_buf[100] = {0};
    snprintf(name_buf, 100, "%s/%s", MOUNT_POINT,  file_name);
    _file = fopen(name_buf, mode);
    return _file != NULL ? ESP_OK : ESP_FAIL;
}

esp_err_t SDCardImp::openWriteFile(const char* file_name, bool bin, bool append=false)
{
    char mode[3] = { append ? 'a' : 'w', bin ? 'b' : '\0' };
    return openFile(file_name, mode) == 0 ? ESP_OK : ESP_FAIL;;
}

size_t SDCardImp::writeBinFile(const char * file_name, uint8_t *data, uint32_t data_size)
{
    ENTER_CRITICAL(sdCardSemaphore);
    size_t bc = 0;
    if(openWriteFile( file_name, true, false) == ESP_OK){
        if(data && data_size){
            bc = fwrite(data, sizeof(uint8_t), data_size, _file);
        }
        closeFile();
    }
    return bc;
}

size_t SDCardImp::writeTextFile(const char * file_name, char *str, bool append)
{
    ENTER_CRITICAL(sdCardSemaphore);
    size_t bc = 0;
    if(openWriteFile( file_name, false, append) == ESP_OK){
        if(data && data_size){
            bc = fwrite(str, sizeof(uint8_t), strlen(str), _file);
        }
        closeFile();
    }
    return bc;
}

size_t SDCardImp::readBinFile(uint8_t *file_buf, uint32_t file_size)
{
    return fread(file_buf, sizeof(uint8_t), file_size, _file);
}

size_t SDCardImp::writeTextToFile(bool append, const char *file_name, const char* format, ...)
{
    size_t bw = 0;
    if(openWriteFile(file_name, false, append) == ESP_OK){
        va_list args;
        va_start(args, format);
        bw = vfprintf(_file, format, args);
        va_end(args);
        closeFile();
    }
    return bw;
}


esp_err_t SDCardImp::format()
{
    ENTER_CRITICAL(sdCardSemaphore);
    if(!this->mounted && begin() != ESP_OK)return false;
    return esp_vfs_fat_sdcard_format(MOUNT_POINT, _card) == 0 ? ESP_OK : ESP_FAIL;;
}

void SDCardImp::prepareBufer(uint8_t **buf, uint32_t file_size)
{
    if(buf){
        delete [] buf;
        buf = NULL;
    }
    *buf = new uint8_t [file_size];
}


size_t SDCardImp::readFile(const char * file_name, uint8_t **buf, uint32_t buf_size)
{
    ENTER_CRITICAL(sdCardSemaphore);
    size_t bc = 0;
    const size_t actual_file_size = getFileSize(file_name);
    if(actual_file_size && openReadFile( file_name, true) == ESP_OK){
        if(actual_file_size > buf_size || *buf == NULL){
            prepareBufer(buf, actual_file_size);
        }
        bc = readBinFile(buf, actual_file_size);
        closeFile();
    }
    return bc;
}


esp_err_t SDCardImp::closeFile()
{
    ENTER_CRITICAL(sdCardSemaphore);
    return fclose(_file) == 0 ? ESP_OK : ESP_FAIL;;
}


void SDCardImp::printInfo()
{
    ENTER_CRITICAL(sdCardSemaphore);
    if(!this->mounted)return;
    sdmmc_card_print_info(stdout, _card);
}

esp_err_t SDCardImp::deleteFile(const char* file_name)
{
    ENTER_CRITICAL(sdCardSemaphore);
    if(!this->mounted)return -1;
    return unlink(file_name) == 0 ? ESP_OK : ESP_FAIL;
}

esp_err_t SDCardImp::deinit()
{
    ENTER_CRITICAL(sdCardSemaphore);
    this->mounted = false;
    return esp_vfs_fat_sdmmc_unmount();
}




