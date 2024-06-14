extern "C"{
    #include <limits.h>
    #include "unity.h"
    #include "esp_err.h"
    #include <time.h>
    #include <string.h>
    #include <errno.h>
    #include "esp_log.h"
    #include "cmock.h"
};

#include "sdcard.hpp"

static const uint8_t BIN_DATA[] = {0x01, 0x02, 0x03};
static const char TEXT_DATA[] = "hello world!";
static const size_t BIN_DATA_SIZE = sizeof(BIN_DATA);
static const size_t TEXT_DATA_SIZE = sizeof(TEXT_DATA)-1;
static const char BIN_FILENAME[] = "test.bin";
static const char TEXT_FILENAME[] = "test.txt";
static const char NO_EXIST_FILENAME[] = "no_exist";
const char *TAG = "errno ";

void show_errno()
{
    if(errno != 0){
        ESP_LOGE(TAG, "%d", errno);
        errno = 0;
    }
}

TEST_CASE("initialisation", "[mcard]")
{
    TEST_ESP_OK(sdcard->begin());
}

TEST_CASE("create a bin file", "[mcard][true]")
{
    size_t bw = sdcard->writeToFile(BIN_FILENAME, BIN_DATA, BIN_DATA_SIZE);
    show_errno();
    TEST_ASSERT_EQUAL(BIN_DATA_SIZE, bw);
}

TEST_CASE("deleting a no existing file", "[mcard][false]")
{
    const esp_err_t err = sdcard->deleteFile(NO_EXIST_FILENAME);
    show_errno();
    TEST_ASSERT_NOT_EQUAL(ESP_OK, err);
}

TEST_CASE("deleting an existing file", "[mcard][true]")
{
    sdcard->writeToFile(BIN_FILENAME, BIN_DATA, BIN_DATA_SIZE);
    const esp_err_t err = sdcard->deleteFile(BIN_FILENAME);
    show_errno();
    TEST_ESP_OK(err);
}

TEST_CASE("rename an existing file", "[mcard][true]")
{
    const char NEW_FILENAME[] = "newname.txt";
    const size_t bw = sdcard->writeToFile(BIN_FILENAME, BIN_DATA, BIN_DATA_SIZE);
    const esp_err_t err = sdcard->renameFile(BIN_FILENAME, NEW_FILENAME);
    show_errno();
    TEST_ASSERT_TRUE_MESSAGE(bw != 0,
            "no data was writed");
    TEST_ESP_OK(err);
}

TEST_CASE("rename a no existing file", "[mcard][false]")
{
    const char NEW_FILENAME[] = "new_filname.txt";
    TEST_ASSERT_NOT_EQUAL(ESP_OK, sdcard->renameFile(NO_EXIST_FILENAME, NEW_FILENAME));
}


TEST_CASE("reading a file", "[mcard][true]")
{
    uint8_t file_buf[BIN_DATA_SIZE];
    TEST_ASSERT_EQUAL(BIN_DATA_SIZE, sdcard->writeToFile(BIN_FILENAME, BIN_DATA, BIN_DATA_SIZE));
    const size_t br = sdcard->readFile(BIN_FILENAME, file_buf, BIN_DATA_SIZE);
    show_errno();
    TEST_ASSERT_EQUAL_MESSAGE(BIN_DATA_SIZE, br,
                        "the file that was read has an unexpected length");
    TEST_ASSERT_EQUAL_MESSAGE(0, memcmp(BIN_DATA, file_buf, BIN_DATA_SIZE),
                        "the file read contains unexpected data");
}

TEST_CASE("getting file size", "[mcard][true]")
{
    sdcard->writeToFile(BIN_FILENAME, BIN_DATA, BIN_DATA_SIZE);
    const size_t size = sdcard->getFileSize(BIN_FILENAME);
    show_errno();
    TEST_ASSERT_EQUAL(BIN_DATA_SIZE, size);
}

TEST_CASE("get the file creation time", "[mcard][true]")
{
    sdcard->writeToFile(BIN_FILENAME, BIN_DATA, BIN_DATA_SIZE);
    const time_t creation_time = sdcard->getFileTime(BIN_FILENAME);
    const time_t now_time = time(NULL);
    show_errno();
    TEST_ASSERT_EQUAL(now_time, creation_time);
}

TEST_CASE("creating a txt file", "[mcard][true]")
{
    const size_t bw = sdcard->writeTextToFile(TEXT_FILENAME, false, TEXT_DATA);
    show_errno();
    TEST_ASSERT_EQUAL(TEXT_DATA_SIZE, bw);
}

TEST_CASE("append data to txt file", "[mcard]")
{
    TEST_ASSERT_EQUAL(TEXT_DATA_SIZE, sdcard->writeTextToFile("new.txt", false, TEXT_DATA));
    TEST_ASSERT_EQUAL(TEXT_DATA_SIZE, sdcard->writeTextToFile("new.txt", true, TEXT_DATA));
}

TEST_CASE("write formatted text to a txt file", "[mcard][true]")
{
    char check_buf[100] = {0};
    char file_buf[100] = {0};
    const char str_data[] = "string for writing";
    const char formated_string[] = "writed string:'%s', len=%d";

    snprintf(check_buf, 100, formated_string,  str_data, sizeof(str_data));
    const size_t bw = sdcard->writeTextToFile(TEXT_FILENAME, false,
                         formated_string, str_data, sizeof(str_data));
    const size_t br = sdcard->readFile(TEXT_FILENAME, (uint8_t*)file_buf, 100);
    show_errno();
    TEST_ASSERT_TRUE_MESSAGE(bw != 0,
                "no data was writed");
    TEST_ASSERT_TRUE_MESSAGE(br != 0,
                "no data was readed");
    TEST_ASSERT_TRUE_MESSAGE(bw == br,
                "the file that was read has an unexpected length");
    TEST_ASSERT_TRUE_MESSAGE(0 == memcmp(check_buf, file_buf, br),
                        "the file read contains unexpected data");
}

TEST_CASE("deinitialisation", "[mcard]")
{
    TEST_ESP_OK(sdcard->deinit());
}

TEST_CASE("format memory card", "[mcard][true][ignore]")
{
    TEST_ESP_OK(sdcard->format());
}