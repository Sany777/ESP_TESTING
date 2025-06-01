#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "stdint.h"
#include "stdbool.h"

typedef void (*EventHandler)(unsigned now_ms, void *ctx);


// ---------- Macros for packing/unpacking void * values ----------

// uint32_t → void*
#define CTX_U32(val) ((void *)(uintptr_t)(uint32_t)(val))
#define CTX_TO_U32(ctx) ((uint32_t)(uintptr_t)(ctx))
// int32_t → void*
#define CTX_I32(val) ((void *)(uintptr_t)(int32_t)(val))
#define CTX_TO_I32(ctx) ((int32_t)(uintptr_t)(ctx))
// bool → void*
#define CTX_BOOL(val) ((void *)(uintptr_t)((val) ? 1 : 0))
#define CTX_TO_BOOL(ctx) ((bool)((uintptr_t)(ctx) & 1))


#define NO_CALL 0
#define FOREVER (-1)

typedef enum {
    EVN_NO_ENTRY = 0,
    EVN_UPDATE_SETTINGS,
    EVN_START_AP,
    EVN_STOP_AP,
    EVN_FACTORY_RESET,
    EVN_SET_WIFI_OFF,
    EVN_SET_HUM_AUTO,
    EVN_BUT_PRESS,
    EVN_READ_GPIO,
    EVN_HDC_READ_HUMIDITY,
    EVN_HDC1080_POLLING,
    EVN_READ_TEMPERATURE,
    EVN_STA_CONNECTED,
    EVN_STA_CONN_FAIL,
    EVN_AP_START,
    EVN_AP_STOP,
    EVN_STA_STOP,
    EVN_AP_CLIENT,
    EVN_ANY, 
} EventId;


void init_event_manager(void);
bool set_event(int event_id, void *ctx);
bool start_periodic_event(int event_id, void *ctx, uint64_t delay, int32_t invoke);
void stop_periodic_event(int event_id);

void unregister_event_handler(EventHandler handler) ;
void unregister_event_id(EventId event_id);
bool register_event_handler(EventId event_id, void(*handler)(unsigned now_ms, void *ctx));


#endif
