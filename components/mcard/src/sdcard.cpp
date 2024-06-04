
extern "C" {
    #include <sys/unistd.h>
    #include <sys/stat.h>
    #include "esp_vfs_fat.h"
    #include "sdmmc_cmd.h"
    #include "driver/sdmmc_host.h"
    #include "driver/gpio.h"
    #include <cstring>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
}

#include "help_tools.hpp"
#include <string>
#include "sdcard.hpp"


static SemaphoreHandle_t sdCardSemaphore;
static FILE *_file;
static sdmmc_card_t *_card;
static SDCardImp sd;
static const char *MOUNT_POINT = "/card";
static const char *PATH_PREF_CSTR = "/card/";
SDCard *sdcard = &sd;


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
    std::string path = PATH_PREF_CSTR; 
    path += file_name; 
    if (this->mounted || begin() == ESP_OK) {
        if(stat(path.c_str(), &st) == 0) size = st.st_size;
    }
    return size;
}

time_t SDCardImp::getFileTime(const char* file_name)
{
    std::string path = PATH_PREF_CSTR; 
    path += file_name; 
    struct stat st;
    time_t time = 0;
    if (stat(path.c_str(), &st) == 0) {
        time = st.st_ctime;
    }
    return time;
}


esp_err_t SDCardImp::begin()
{
    ENTER_CRITICAL(sdCardSemaphore);
    if(this->mounted)return ESP_OK;
    initGPIO();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
    .format_if_mount_failed = true,
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

esp_err_t SDCardImp::renameFile(const char* old_name, const char* new_name)
{
    ENTER_CRITICAL(sdCardSemaphore);
    std::string new_name_path = PATH_PREF_CSTR;
    new_name_path += new_name;
    std::string old_name_path = PATH_PREF_CSTR;
    old_name_path += old_name;
    if(!this->mounted && begin() != ESP_OK)return ESP_FAIL;
    struct stat st;
    if (stat(old_name_path.c_str(), &st) == 0) {
        if (stat(new_name_path.c_str(), &st) == 0) {
            unlink(new_name_path.c_str());
        }
        return rename(old_name_path.c_str(), new_name_path.c_str()) == 0 ? ESP_OK : ESP_FAIL;
    }
    return ESP_FAIL;
}


esp_err_t SDCardImp::openFile(const char*  file_name, const char* mode)
{
    if(!this->mounted && begin() != ESP_OK)return ESP_FAIL;
    std::string path = PATH_PREF_CSTR;
    path += file_name; 
    _file = fopen(path.c_str(), mode);
    return _file != NULL ? ESP_OK : ESP_FAIL;
}



size_t SDCardImp::writeToFile(const char * file_name, const uint8_t *data, const size_t data_size, bool append)
{
    ENTER_CRITICAL(sdCardSemaphore);
    size_t bc = 0;
    if(openFile(file_name, append ? "ab" : "wb") == ESP_OK){
        bc = fwrite(data, sizeof(uint8_t), data_size, _file);
        fclose(_file);
    }
    return bc;
}



size_t SDCardImp::writeTextToFile( const char *file_name, bool append, const char* format, ...)
{
    ENTER_CRITICAL(sdCardSemaphore);
    size_t bw = 0;
    if(openFile(file_name, append ? "ab":"wb") == ESP_OK){
        va_list args;
        va_start(args, format);
        bw = vfprintf(_file, format, args);
        fclose(_file);
        va_end(args);
    }
    return bw;
}


esp_err_t SDCardImp::format()
{
    ENTER_CRITICAL(sdCardSemaphore);
    if(!this->mounted && begin() != ESP_OK)return false;
    this->mounted = false;
    return esp_vfs_fat_sdcard_format(MOUNT_POINT, _card) == 0 ? ESP_OK : ESP_FAIL;;
}


size_t SDCardImp::readFile(const char * file_name, uint8_t *buf, const size_t buf_size)
{
    ENTER_CRITICAL(sdCardSemaphore);
    size_t bc = 0;
    const size_t file_size = getFileSize(file_name);
    if(file_size 
            && file_size <= buf_size 
            && openFile(file_name, "rb") == ESP_OK){
        bc = fread(buf, sizeof(uint8_t), file_size, _file);
        fclose(_file);
    }
    return bc;
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
    if(file_name == NULL || (!this->mounted && begin() != ESP_OK))return ESP_FAIL;
    std::string path = PATH_PREF_CSTR;
    path += file_name; 
    return unlink(path.c_str()) == 0 ? ESP_OK : ESP_FAIL;
}

esp_err_t SDCardImp::deinit()
{
    ENTER_CRITICAL(sdCardSemaphore);
    this->mounted = false;
    return esp_vfs_fat_sdcard_unmount(MOUNT_POINT, _card);
}




