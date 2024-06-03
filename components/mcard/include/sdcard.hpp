#pragma once

extern "C"{
    #include <esp_system.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include <stdarg.h>
    #include "time.h"
}

#include <cstdint>


class SDCard {
public:
    virtual esp_err_t begin() = 0;
    virtual size_t getFileSize(const char * file_name) = 0;
    virtual time_t getFileTime(const char *file_name) = 0;
    virtual esp_err_t renameFile(const char *old_name, const char *new_name) = 0;
    virtual size_t writeToFile(const char * file_name, const uint8_t *data, const size_t data_size, bool append=false) = 0;
    virtual size_t writeTextToFile( const char *file_name, bool append, const char* format, ...) = 0;
    virtual esp_err_t format() = 0;
    virtual size_t readFile(const char * file_name, uint8_t *buf, const size_t buf_size) = 0;
    virtual void printInfo() = 0;
    virtual esp_err_t deleteFile(const char* file_name) = 0;
    virtual esp_err_t deinit() = 0;
};

class SDCardImp: public SDCard {
private:
    
    static constexpr size_t SECTOR_SIZE = 16 * 1024;
    bool mounted;
    esp_err_t initGPIO();
    esp_err_t openFile(const char*  file_name, const char* mode);
public:
    esp_err_t begin()override;
    size_t getFileSize(const char * file_name)override;
    time_t getFileTime(const char *file_name)override;
    esp_err_t renameFile(const char *old_name, const char *new_name)override;
    size_t writeToFile(const char * file_name, const uint8_t *data, const size_t data_size, bool append=false)override;
    size_t writeTextToFile( const char *file_name, bool append, const char* format, ...)override;
    esp_err_t format()override;
    size_t readFile(const char * file_name, uint8_t *buf, const size_t buf_size)override;
    void printInfo()override;
    esp_err_t deleteFile(const char* file_name)override;
    esp_err_t deinit()override;
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



