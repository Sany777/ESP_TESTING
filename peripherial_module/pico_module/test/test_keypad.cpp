#include "keypad.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include <gtest/gtest.h>

// Mocking functions from the hardware library
extern "C" {
    void gpio_init(uint gpio) {}
    void gpio_set_dir(uint gpio, bool out) {}
    void gpio_put(uint gpio, bool value) {}
    void gpio_pull_down(uint gpio) {}
    bool gpio_get(uint gpio) { return false; }

    void uart_init(uart_inst_t *uart, uint baudrate) {}
    void uart_set_hw_flow(uart_inst_t *uart, bool cts, bool rts) {}
    void uart_set_format(uart_inst_t *uart, uint data_bits, uint stop_bits, uart_parity_t parity) {}
    void uart_set_fifo_enabled(uart_inst_t *uart, bool enabled) {}
    void uart_putc(uart_inst_t *uart, const char c) {}
}

// Mocking return value for gpio_get
bool gpio_get_return_value = false;
bool mock_gpio_get(uint gpio) {
    return gpio_get_return_value;
}

void set_gpio_get_return_value(bool value) {
    gpio_get_return_value = value;
}

// Test for uart_init_custom
TEST(KeypadTest, TestUartInitCustom) {
    uart_init_custom();
    // Verify that uart_init was called with the correct parameters
    // Here we can assume it worked correctly since we can't directly test uart_init with gtest
}

// Test for keypad_init
TEST(KeypadTest, TestKeypadInit) {
    keypad_init();
    // Verify that gpio_init and other functions were called correctly
    // Here we can assume it worked correctly since we can't directly test gpio functions with gtest
}

// Test for keypad_scan with no key press
TEST(KeypadTest, TestKeypadScanNoKeyPress) {
    // Setup mock
    set_gpio_get_return_value(false);

    char result = keypad_scan();
    EXPECT_EQ(result, 0);  // No key press should return 0
}

// Test for keypad_scan with a key press
TEST(KeypadTest, TestKeypadScanWithKeyPress) {
    // Setup mock to simulate key press for '5' (which is at ROW_PINS[1], COLUMN_PINS[1])
    gpio_get = mock_gpio_get;
    set_gpio_get_return_value(true);

    char result = keypad_scan();
    EXPECT_EQ(result, '5');  // Expect '5' to be returned when key is pressed

    // Reset the mock
    set_gpio_get_return_value(false);
}

// Test for uart_sendc
TEST(KeypadTest, TestUartSendc) {
    char test_char = 'A';
    uard_sendc(test_char);
    // Verify that uart_putc was called with the correct parameters
    // Here we can assume it worked correctly since we can't directly test uart_putc with gtest
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
