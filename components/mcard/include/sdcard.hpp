#pragma once

extern "C"{
    #include <esp_system.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include <stdarg.h>
}

#include <cstdint>

void led_on(uint8_t state);

class SDCard {
public:
    virtual int writeBinFile(const char *filename, uint8_t *data, uint32_t data_size) = 0;
    virtual int writeTextFile(const char *filename, char *str, bool append=false) = 0;
    virtual void printInfo() = 0;
    virtual int deleteFile(const char* name) = 0;
    virtual bool renameFile(const char *new_name, const char *old_name) = 0;
    virtual int readFile(const char *filename, uint8_t *&buf, uint32_t buf_size) = 0;
    virtual int getFileSize(const char *file_name) = 0;
    virtual int getFileTime(const char *file_name) = 0;
    virtual bool format() = 0;
    virtual bool writeTextToFile(bool append, const char *file_name, const char* format, ...) = 0;
};

class SDCardImp: public SDCard {
private:
    static constexpr char MOUNT_POINT[] = "/sdcard";
    static constexpr size_t SECTOR_SIZE = 16 * 1024;
    bool mounted;
    bool openReadFile(const char* file_name, bool bin);
    bool openFile(const char* file_name, const char* mode);
    bool openWriteFile(const char* file_name, bool bin, bool append);
    int readBinFile(uint8_t *file_buf, uint32_t file_size);
    bool begin();
    uint8_t* prepareBufer(uint8_t *&buf, uint32_t file_size);
    void closeFile();
    void initGPIO();
public:
    int writeBinFile(const char * file_name, uint8_t *data, uint32_t data_size)override;
    int writeTextFile(const char * file_name, char *str, bool append=false)override;
    void printInfo()override;
    int deleteFile(const char* name)override;
    bool renameFile(const char *new_name, const char *old_name)override;
    int readFile(const char * file_name, uint8_t *&buf, uint32_t buf_size)override;
    int getFileSize(const char *file_name)override;
    int getFileTime(const char *file_name)override;
    bool format()override;
    bool writeTextToFile(bool append, const char *file_name, const char* format, ...)override;
};

class AutoLock{
    SemaphoreHandle_t _semaphore;
public:
    AutoLock(SemaphoreHandle_t &semaphore);
    ~AutoLock();
};

#define ENTER_CRITICAL(semaphore)    \
    AutoLock _al(semaphore)


extern SDCard *sdcard;