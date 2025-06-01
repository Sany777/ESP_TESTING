#pragma once 


#include "stdint.h"
#include "stdbool.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "stddef.h"
#include "esp_log.h"
#include "esp_err.h"


#define PERIODIC_EVN_LIST_LEN  50
#define NEW_EVN_QUEUE_LEN 10
#define EVN_QUEUE_LEN 20
#define STACK_SIZE 5096

typedef struct {
    EventId event_id;
    EventHandler handler;
} EventHandlerRegistry;


typedef struct {
	int event_id;
	void* ctx;
} DevEvn;

typedef struct {
	DevEvn evn;
	int32_t invoke;
	uint64_t init_delay;
	uint64_t delay;
} PeriodicEvent;

extern QueueHandle_t evn_queue;
extern volatile unsigned ms_counter;



void init_event_time_manager();

