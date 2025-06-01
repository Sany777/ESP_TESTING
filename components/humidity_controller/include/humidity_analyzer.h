#ifndef HUMIDITY_ANALYZER_H
#define HUMIDITY_ANALYZER_H

#include <stdint.h>
#include <stdbool.h>



#define MAX_SCALE_VAL           1000
#define AVERAGE_SAMPLING_S      30
#define HUMIDITY_RISE_WINDOW_S  600
#define MAX_HUM_RATE_COUNT      (HUMIDITY_RISE_WINDOW_S / AVERAGE_SAMPLING_S)
#define ACCEL_THRESHOLD         5
#define RISE_PERCENTAGE         8  // 8.0%
#define RISE_THRESHOLD          ((MAX_SCALE_VAL * 8) / 100)  

typedef enum {
    GROWTH_MEASURING,
    GROWTH_STABLE,
    GROWTH_RISING,
    GROWTH_FALLING,
} GrowthState;

typedef enum {
    ACCELERATION_MEASURING,
    ACCELERATION_STABLE,   
    ACCELERATION_RISING,  
    ACCELERATION_FALLING,
} AccelerationState;

typedef struct {
    // Вхідні дані
    int32_t humidity_sum;
    uint16_t humidity_count;
    // Усереднене значення вологості
    uint16_t avg_humidity;
    // Похідні
    int16_t humidity_rate;
    int16_t acceleration;
    // Сумування для довгострокового аналізу
    int32_t long_term_rate;
    uint16_t rate_count;
    // Стан
    AccelerationState acc_state;
    GrowthState growth_state;
    bool measure_init;
} HumidityAnalyzer;

void humidity_analyze(HumidityAnalyzer* ha, int16_t current_humidity);




#endif // HUMIDITY_ANALYZER_H