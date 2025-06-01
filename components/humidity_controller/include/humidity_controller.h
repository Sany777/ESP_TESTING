#ifndef HUMIDITY_CONTROL_H
#define HUMIDITY_CONTROL_H

#include <stdint.h>
#include <stdbool.h>
#include "humidity_analyzer.h"


typedef enum {
    FAN_OFF,
    FAN_V1,
    FAN_V2,
    FAN_MIN_TIME,
    FAN_MAX_TIME,
}FanState;


typedef struct {
    uint32_t runtime_seconds;
    uint32_t stop_time;
    FanState fan_state;
} FanController;

#define FAN_MIN_RUNTIME_S 30   
#define FAN_MAX_RUNTIME_S 3600   // 1 година

#endif // HUMIDITY_CONTROL_H
