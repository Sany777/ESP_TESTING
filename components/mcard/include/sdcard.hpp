#pragma once

extern "C"{
    #include <esp_system.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include <stdarg.h>
}

#include <cstdint>


class SDCard {
public:
    virtual esp_err_t begin() = 0;
    virtual size_t getFileSize(const char * file_name) = 0;
    virtual time_t getFileTime(const char *file_name) = 0;
    virtual esp_err_t renameFile(const char *new_name, const char *old_name) = 0;
    virtual size_t writeBinFile(const char * file_name, uint8_t *data, uint32_t data_size) = 0;
    virtual size_t writeTextFile(const char * file_name, char *str, bool append) = 0;
    virtual size_t writeTextToFile(bool append, const char *file_name, const char* format, ...) = 0;
    virtual esp_err_t format() = 0;
    virtual size_t readFile(const char * file_name, uint8_t **buf, uint32_t buf_size) = 0;
    virtual void printInfo() = 0;
    virtual esp_err_t deleteFile(const char* file_name) = 0;
};

class SDCardImp: public SDCard {
private:
    static constexpr char MOUNT_POINT[] = "/sdcard";
    static constexpr size_t SECTOR_SIZE = 16 * 1024;
    bool mounted;
    esp_err_t initGPIO();
    esp_err_t openReadFile(const char* file_name, bool bin);
    esp_err_t openFile(const char*  file_name, const char* mode);
    esp_err_t openWriteFile(const char* file_name, bool bin, bool append=false);
    void prepareBufer(uint8_t **buf, uint32_t file_size);
    esp_err_t closeFile();
    size_t readBinFile(uint8_t *file_buf, uint32_t file_size);
public:
    esp_err_t begin();
    size_t getFileSize(const char * file_name);
    time_t getFileTime(const char *file_name);
    esp_err_t renameFile(const char *new_name, const char *old_name);
    size_t writeBinFile(const char * file_name, uint8_t *data, uint32_t data_size);
    size_t writeTextFile(const char * file_name, char *str, bool append);
    size_t writeTextToFile(bool append, const char *file_name, const char* format, ...);
    esp_err_t format();
    size_t readFile(const char * file_name, uint8_t **buf, uint32_t buf_size);
    void printInfo();
    esp_err_t deleteFile(const char* file_name);
    esp_err_t deinit();
};


class AutoLock{
    SemaphoreHandle_t _semaphore;
public:
    AutoLock(SemaphoreHandle_t &semaphore);
    ~AutoLock();
};

#define ENTER_CRITICAL(semaphore)    \
    AutoLock _al(semaphore)

esp_err_t led_on(uint8_t state);

extern SDCard *sdcard;



