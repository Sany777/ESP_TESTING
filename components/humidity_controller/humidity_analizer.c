#include "humidity_analyzer.h"




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
    // Обчислюємо прискорення зростання вологості (похідна 2-го порядку)
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
    } else if (new_accel >= ACCEL_THRESHOLD && new_rate > 0) {
        ha->acc_state = ACCELERATION_RISING;
    } else if (new_accel <= -ACCEL_THRESHOLD && new_rate < 0) {
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

