#pragma once

extern "C"{
    #include "driver/uart.h"
    #include "driver/gpio.h"
}

class HardwareSerial{
public:
    virtual int available() = 0;
    virtual uint8_t read() = 0;
    virtual size_t read(uint8_t *buffer, size_t size) = 0;
    virtual size_t read(char *buffer, size_t size) = 0;
    virtual size_t write(uint8_t b) = 0;
    virtual size_t write(const uint8_t *buffer, size_t size) = 0;
    virtual size_t write(const char *buffer) = 0;
};

class EspUart : public HardwareSerial{
private:
    static constexpr size_t DRV_BUF_SIZE = 1000;
    uart_port_t _UART_NUM;
    void deinit(uart_port_t cur_port);
    int fix_serial_monitor_param(uart_port_t uart_num, const uint32_t baudrate);
public:
    static constexpr int UART_TIMEOUT = 200;
    static constexpr uart_port_t MONITOR_UART = UART_NUM_0;
    static constexpr uint32_t  MONITOR_SPEED = 115200;
    static constexpr int MONITOR_TX_PIN = GPIO_NUM_1;
    static constexpr int MONITOR_RX_PIN = GPIO_NUM_3;
    bool monitor_work;
    int available()override;
    uint8_t read()override;
    size_t read(char *buffer, size_t size)override;
    size_t read(uint8_t *buffer, size_t size)override;
    size_t write(const uint8_t *buffer, size_t size)override;
    size_t write(const char *buffer)override;
    size_t write(uint8_t b)override;
    int begin(int baud, 
                uart_port_t port = MONITOR_UART, 
                int txPin = -1, 
                int rxPin = -1);
    void stop();
    EspUart():_UART_NUM(MONITOR_UART)
    {

    }

    ~EspUart()
    {
        this->stop();
    }
};


class MFRC522 {

public:
    MFRC522();
    void begin(HardwareSerial *serial);
    bool available();
    void wait();
    void readCardSerial();
    uint8_t *getCardSerial();
    bool getBlock(uint8_t block, uint8_t keyType, uint8_t *key, uint8_t *returnBlock);
    bool writeBlock(uint8_t block, uint8_t keyType, uint8_t *key, uint8_t *data);
    bool communicate(uint8_t command, uint8_t *sendData, uint8_t sendDataLength, uint8_t *returnData, uint8_t *returnDataLength);
    void write(uint8_t value);
    uint8_t read();
    
private:
    HardwareSerial *_Serial;
    uint8_t cardSerial[4];
    
};

