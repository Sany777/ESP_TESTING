#include "humidity_analyzer.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>


#define ARR_LEN(arr) (sizeof(arr)/sizeof(arr[0]))


void humidity_analyze(HumidityAnalyzer* ha, int16_t current_humidity) 
{
    if (!ha) return;
    ha->growth_state = GROWTH_MEASURING;
    ha->acc_state = ACCELERATION_MEASURING;
    // Сумування для усереднення
    ha->humidity_sum += current_humidity;
    ha->humidity_count++;
    // Очікуємо всі дані для усереднення
    if (ha->humidity_count < AVERAGE_SAMPLING_S) return;
    
    // Обчислюємо середнє значення вологості
    int16_t new_avg = (int16_t)(ha->humidity_sum / ha->humidity_count);
    ha->humidity_sum = 0;
    ha->humidity_count = 0;
    // Обчислюємо швидкість зміни вологості (похідна 1-го порядку)
    int16_t new_rate = new_avg - ha->avg_humidity;
    // Обчислюємо прискорення зміни вологості (похідна 2-го порядку)
    int16_t new_accel = new_rate - ha->humidity_rate;
    // Оновлюємо стан
    ha->avg_humidity = new_avg;
    ha->humidity_rate = new_rate;
    ha->acceleration = new_accel;
    // Сумування для аналізу довгострокової тенденції зростання
    ha->long_term_rate += new_rate;
    ha->rate_count++;

    // Аналіз короткострокової тенденції зростання
    if(ha->measure_init == false){
        ha->measure_init = true;
        ha->rate_count = 0;
        ha->long_term_rate = 0;
        ha->humidity_rate = 0;
    } else if (new_accel >= ACCEL_THRESHOLD && new_rate >= 0) {
        ha->acc_state = ACCELERATION_RISING;
    } else if (new_accel <= -ACCEL_THRESHOLD && new_rate <= 0) {
        ha->acc_state = ACCELERATION_FALLING;
    } else {
        ha->acc_state = ACCELERATION_STABLE;
    }
    // Очікуємо повноти даних
    if (ha->rate_count < MAX_HUM_RATE_COUNT) return;
    if (ha->long_term_rate > RISE_THRESHOLD) {
        ha->growth_state = GROWTH_RISING;
    } else if (ha->long_term_rate < -RISE_THRESHOLD) {
        ha->growth_state = GROWTH_FALLING;
    } else {
        ha->growth_state = GROWTH_STABLE;
    }
    ha->long_term_rate = 0;
    ha->rate_count = 0;
}

static
const char* growth_state_str(GrowthState state) 
{
    switch (state) {
        case GROWTH_MEASURING: return "MEASURING";
        case GROWTH_STABLE: return "STABLE";
        case GROWTH_RISING: return "RISING";
        case GROWTH_FALLING: return "FALLING";
        default: return "";
    }
}

static
const char* acceleration_state_str(AccelerationState state) 
{
    switch (state) {
        case ACCELERATION_MEASURING: return "MEASURING";
        case ACCELERATION_STABLE: return "STABLE";
        case ACCELERATION_RISING: return "RISING";
        case ACCELERATION_FALLING: return "FALLING";
        default: return "";
    }
}

static
void run_test(const int16_t* data, size_t len, const char* label) 
{
    HumidityAnalyzer ha = {0};
    printf("..... Тест: %s .....\n", label);

    for (size_t i = 0; i < len; i++) {
        for(int ii=0; ii<AVERAGE_SAMPLING_S; ii++){
            humidity_analyze(&ha, data[i]);  
        }
        printf("Step %2u | Avg: %3u | dH/dt: %+4d | d2H/dt2: %+4d | Acc: %s | Growth: %s\n",
            i + 1,
            ha.avg_humidity,
            ha.humidity_rate,
            ha.acceleration,
            acceleration_state_str(ha.acc_state),
            growth_state_str(ha.growth_state)
        );
    }
    printf("\n");
}


int main() 
{
    // Початкові 10 хв — стабільна вологість (~55%)
    const int16_t stable_before[] = {
        550, 550, 552, 551, 550, 553, 549, 551, 552, 550, 551,
        552, 553, 551, 550, 552, 550, 551, 550, 552, 553
    };

    // 10 хв зростання вологості (~55% → ~65%)
    const int16_t rising[] = {
        554, 555, 560, 566, 570, 576, 582, 588, 595, 603, 610,
        615, 620, 625, 630, 635, 640, 645, 648, 650, 652, 653
    };

    // Стабільно висока (не зростає)
    const int16_t stable_high[] = {
        650, 653, 652, 654, 653, 655, 654, 653, 654, 653, 654,
        653, 653, 654, 653, 654, 653, 653, 653, 654, 653,653
    };

    // Падіння вологості після провітрювання
    const int16_t falling[] = {
        650, 650, 645, 640, 635, 630, 625, 620, 615, 610, 605,
        600, 595, 590, 585, 580, 575, 570, 565, 560, 555, 555
    };

    run_test(stable_before, ARR_LEN(stable_before), "Стабільна до душу");
    run_test(rising, ARR_LEN(rising), "Зростання після ввімкнення душу");
    run_test(stable_high, ARR_LEN(stable_high), "Стабільно висока після душу");
    run_test(falling, ARR_LEN(falling), "Падіння після провітрювання");
}