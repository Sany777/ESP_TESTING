extern "C"{
    #include <stdio.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "driver/uart.h"

}

#include "help_tools.hpp"
#include "MFRC522.hpp"
#include <cstring>

void EspUart::stop()
{
    if(this->_UART_NUM == EspUart::MONITOR_UART && CONFIG_LOG_DEFAULT_LEVEL>0){
        fix_serial_monitor_param(EspUart::MONITOR_UART, EspUart::MONITOR_SPEED);
    } else {
        this->deinit(this->_UART_NUM);
    }
}

int EspUart::fix_serial_monitor_param(uart_port_t uart_num, const uint32_t baudrate)
{
    uint32_t cur_baudrate;
    if(uart_get_baudrate(uart_num, &cur_baudrate) == ESP_OK 
            && baudrate != cur_baudrate){
        return uart_set_baudrate(uart_num, baudrate);
    }
    return ESP_OK;
}

void EspUart::deinit(uart_port_t cur_port)
{
    if(uart_is_driver_installed(cur_port)){
        uart_driver_delete(cur_port);
    }
}

size_t EspUart::write(uint8_t b)
{
    return uart_write_bytes(this->_UART_NUM, &b, 1);
}

int EspUart::available()
{
    size_t len  = 0;
    uart_get_buffered_data_len(this->_UART_NUM, &len);
    return len;
}

uint8_t EspUart::read()
{
    uint8_t b = 0;
    uart_read_bytes(this->_UART_NUM, &b, 1, 200);
    return b;
}

size_t EspUart::read(char *buffer, size_t size)
{
    return this->read((uint8_t *)buffer, size);
}

size_t EspUart::read(uint8_t *buffer, size_t size)
{
    return uart_read_bytes(this->_UART_NUM, buffer, size, 200);
}

size_t EspUart::write(const uint8_t *buffer, size_t size)
{
    return uart_write_bytes(this->_UART_NUM, buffer, size);
}

size_t EspUart::write(const char *buffer)
{
    return uart_write_bytes(this->_UART_NUM, (const uint8_t*)buffer, strlen(buffer));
}



int EspUart::begin(int baud, 
            uart_port_t  port, 
            int  txPin, 
            int  rxPin)
{

    uart_config_t uart_config = {
        .baud_rate = baud,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    if(uart_param_config(port, &uart_config) != ESP_OK
            || uart_set_pin(port, 
                            txPin, 
                            rxPin, 
                            UART_PIN_NO_CHANGE, 
                            UART_PIN_NO_CHANGE) != ESP_OK){
        return ESP_FAIL;
    }
    this->_UART_NUM = port;
    if(uart_is_driver_installed(port) ||
        (port == EspUart::MONITOR_UART && CONFIG_LOG_DEFAULT_LEVEL>0)){
        return fix_serial_monitor_param(port, baud);
    }
    return uart_driver_install(port, DRV_BUF_SIZE * 2, 0, 0, NULL, 0);
}

//------------------MFRC522 register ---------------
#define         COMMAND_WAIT        0x02
#define         COMMAND_READBLOCK   0x03
#define         COMMAND_WRITEBLOCK  0x04
#define         MFRC522_HEADER      0xAB

#define         STATUS_ERROR        0
#define         STATUS_OK           1

#define         MIFARE_KEYA         0x00
#define         MIFARE_KEYB         0x01

/**
 * Constructor.
 */
MFRC522::MFRC522() {
    _Serial = NULL;
}

/**
 * Description： Obtiene control del Serial para controlar MFRC522. 
 * Ademas, pone MFRC522 en modo de espera.
 */
void MFRC522::begin(HardwareSerial *serial) {
    _Serial = serial;
    wait();
}

/**
 * Description：Pone MFRC522 en modo espera.
 */
void MFRC522::wait() {
    _Serial->write(COMMAND_WAIT);
}

/**
 * Description：Returns true if detect card in MFRC522.
 */
bool MFRC522::available() {
    return (_Serial->available() > 0);
}

/**
 * Description：Read the serial number of the card.
 */
void MFRC522::readCardSerial() {
    for (int i = 0; i < sizeof(cardSerial); ){
        if (available()){
            cardSerial[i] = read();
            i++;
        }
    }
}

/**
 * Description：Returns a pointer to array with card serial.
 */
uint8_t *MFRC522::getCardSerial() {
    return cardSerial;
}

/**
 * Description：Read a block data of the card.
 * Return：Return STATUS_OK if success.
 */
bool MFRC522::getBlock(uint8_t block, uint8_t keyType, uint8_t *key, uint8_t *returnBlock) {    
    uint8_t sendData[8] = {
        block,      // block
        keyType,    // key type
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF  // key
    };
    uint8_t returnBlockLength;

    for (int i = 0; i < 6; ++i) {
        sendData[2 + i] = key[i];
    }

    return communicate(
        COMMAND_READBLOCK,  // command
        sendData,           // sendData
        0x0A,               // length
        returnBlock,        // returnData
        &returnBlockLength  // returnDataLength
    );
}

/**
 * Description：Write a block data in the card.
 * Return：Return STATUS_OK if success.
 */
bool MFRC522::writeBlock(uint8_t block, uint8_t keyType, uint8_t *key, uint8_t *data) {    
    uint8_t sendData[24] = {
        block,      // block
        keyType,    // key type
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // key
        0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // Data
    };
    uint8_t returnBlock[3];
    uint8_t returnBlockLength;

    for (int i = 0; i < 6; ++i) {
        sendData[2 + i] = key[i];
    }

    for (int i = 0; i < 16; ++i) {
        sendData[8 + i] = data[i];
    }

    uint8_t result = communicate(
        COMMAND_WRITEBLOCK, // command
        sendData,           // sendData
        0x1A,               // length
        returnBlock,        // returnData
        &returnBlockLength  // returnDataLength
    );

    return result;
}

/**
 * Description：Comunication between MFRC522 and ISO14443.
 * Return：Return STATUS_OK if success.
 */
bool MFRC522::communicate(uint8_t command, uint8_t *sendData, uint8_t sendDataLength, uint8_t *returnData, uint8_t *returnDataLength) {
    // Send instruction to MFRC522
    write(MFRC522_HEADER);      // Header
    write(sendDataLength);      // Length (Length + Command + Data)
    write(command);             // Command

    for (int i = 0; i < sendDataLength - 2; i++) {
        write(sendData[i]);     // Data
    }

    // Read response to MFRC522
    while (!available());
    uint8_t header = read();           // Header
    while (!available());
    *returnDataLength = read();     // Length (Length + Command + Data)
    while (!available());
    uint8_t commandResult = read();    // Command result

    for (int i = 0; i < *returnDataLength - 2; i=i) {
        if (available()) {
            returnData[i] = read(); // Data
            i++;
        }
    }

    // Return
    if (command != commandResult) {
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

/*
 * Description：Write a uint8_t data into MFRC522.
 */
void MFRC522::write(uint8_t value) {
    _Serial->write(value);
}

/*
 * Description：Read a uint8_t data of MFRC522
 * Return：Return the read value.
 */
uint8_t MFRC522::read() {
    return _Serial->read();
}
