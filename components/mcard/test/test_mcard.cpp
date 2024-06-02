extern "C"{
    #include <limits.h>
    #include "unity.h"
    #include "sdcard.hpp"
    #include "esp_err.h"
};


TEST_CASE("initialisation", "[mcard]")
{
    TEST_ASSERT_TRUE(sdcard->begin() == ESP_OK);
}

TEST_CASE("create a bin file", "[mcard]")
{
    uint8_t data[] = {0x01, 0x02, 0x03};
    TEST_ASSERT_EQUAL(sizeof(data), sdCard->writeBinFile("test.bin", data, sizeof(data)));
}

TEST_CASE("deleting a no existing file", "[mcard][false]")
{
    const char filename[] = "no_exist.txt";
    TEST_ASSERT_FALSE(sdCard->deleteFile(filename) == ESP_OK);
}

TEST_CASE("deleting an existing file", "[mcard][true]")
{
    const char filename[] = "test.txt";
    const char data[] = "data";
    TEST_ASSERT_EQUAL(sizeof(data), sdCard->writeBinFile(filename, data, sizeof(data)));
    TEST_ASSERT_TRUE(sdCard->deleteFile(filename) == ESP_OK);
}

TEST_CASE("rename an existing file", "[mcard][true]")
{
    const char filename[] = "test.txt";
    TEST_ASSERT_EQUAL(sizeof(data), sdCard->writeBinFile(filename, data, sizeof(data)));
    TEST_ASSERT_TRUE(sdCard->renameFile(filename) == ESP_OK);
}

TEST_CASE("rename a no existing file", "[mcard][false]")
{
    const char filename[] = "no_exist.txt";
    TEST_ASSERT_FALSE(sdCard->renameFile(filename) != ESP_OK);
}

TEST_CASE("reading a file", "[mcard][true]")
{
    const char filename[] = "test.txt";
    const uint8_t data[] = {0x01, 0x02, 0x03};
    uint8_t *buf = NULL;
    TEST_ASSERT_TRUE_MESSAGE(sizeof(buf) != sdCard->readFile(filename, &buf, sizeof(buf)),
                        "the file that was read has an unexpected length");
    TEST_ASSERT_TRUE_MESSAGE(memcpu(data, buf, sizeof(data)),
                        "the file that was read has an unexpected data");
}

TEST_CASE("getting file size", "[mcard]")
{
    const char filename[] = "test.txt";
    uint8_t data[] = {0x01, 0x02, 0x03};
    TEST_ASSERT_EQUAL(sizeof(data), sdCard->writeBinFile(filename, data, sizeof(data)));
    TEST_ASSERT_EQUAL(sizeof(data), sdCard->getFileSize(filename, data, sizeof(data)));
}

TEST_CASE("getting the create time", "[mcard]")
{
    const char filename[] = "test.txt";
    const char data[] = "hello world";
    time_t t;
    TEST_ASSERT_EQUAL( sdCard->getFileTime)
}

TEST_CASE("creating a text file", "[mcard]")
{
    const char data[] = "hello world";

}

TEST_CASE("append data to text file", "[mcard]")
{
    const char data[] = "hello world";

}

TEST_CASE("append data to bin file", "[mcard]")
{
    const char data[] = "hello world";

}




