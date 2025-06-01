#include "unity.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "string.h"
#include "stdio.h"
#include "event_manager.h"


#include "unity.h"
#include "event_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ------------------ Test data ------------------

static int test_data = 0;

static void handler_add_value(unsigned now_ms, void *ctx)
{
    (void)now_ms;
    test_data += CTX_TO_U32(ctx);
}

static void handler_set_flag(unsigned now_ms, void *ctx)
{
    (void)now_ms;
    test_data = CTX_TO_BOOL(ctx);
}


TEST_CASE("Event handler is called once", "[event_manager]")
{
    init_event_manager();
    test_data = 0;
    TEST_ASSERT_TRUE(register_event_handler(EVN_UPDATE_SETTINGS, handler_add_value));
    TEST_ASSERT_TRUE(set_event(EVN_UPDATE_SETTINGS, CTX_U32(5)));
    vTaskDelay(pdMS_TO_TICKS(10));
    TEST_ASSERT_EQUAL(5, test_data);
}

TEST_CASE("Only one periodic event is active", "[event_manager]")
{
    init_event_manager();
    test_data = 0;
    TEST_ASSERT_TRUE(register_event_handler(EVN_READ_GPIO, handler_add_value));
    TEST_ASSERT_TRUE(start_periodic_event(EVN_READ_GPIO, CTX_U32(1), 5, 3));
    TEST_ASSERT_TRUE(start_periodic_event(EVN_READ_GPIO, CTX_U32(2), 5, 2));
    vTaskDelay(pdMS_TO_TICKS(30));
    TEST_ASSERT_TRUE(test_data == 3 || test_data == 4); 
}


TEST_CASE("Handler replacement behavior", "[event_manager]")
{
    void handler1(unsigned now_ms, void *ctx) { (void)now_ms; (void)ctx; test_data = 100; };
    void handler2(unsigned now_ms, void *ctx) { (void)now_ms; (void)ctx; test_data = 200; };
    init_event_manager();
    test_data = 0;
    TEST_ASSERT_TRUE(register_event_handler(EVN_SET_WIFI_OFF, handler1));
    TEST_ASSERT_TRUE(register_event_handler(EVN_SET_WIFI_OFF, handler2));
    TEST_ASSERT_TRUE(set_event(EVN_SET_WIFI_OFF, NULL));
    vTaskDelay(pdMS_TO_TICKS(10));
    TEST_ASSERT_EQUAL(200, test_data);
}

TEST_CASE("Unregister event handler prevents call", "[event_manager]")
{
    init_event_manager();
    test_data = 0;
    TEST_ASSERT_TRUE(register_event_handler(EVN_FACTORY_RESET, handler_set_flag));
    TEST_ASSERT_TRUE(set_event(EVN_FACTORY_RESET, CTX_BOOL(true)));
    vTaskDelay(pdMS_TO_TICKS(5));
    TEST_ASSERT_EQUAL(true, test_data);
    unregister_event_handler(handler_set_flag);
    test_data = 0;
    TEST_ASSERT_TRUE(set_event(EVN_FACTORY_RESET, CTX_BOOL(true)));
    vTaskDelay(pdMS_TO_TICKS(5));
    TEST_ASSERT_EQUAL(0, test_data);
}