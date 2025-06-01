#include "humidity_controller.h"




bool humidity_rise(const HumidityAnalyzer* ha)
{
    return ha->growth_state == GROWTH_RISING || ha->acc_state == ACCELERATION_RISING;
}


void fan_controller_update(FanController* fc, const HumidityAnalyzer* ha)
{
    switch (fc->fan_state){
        case FAN_OFF:
            break;
        case FAN_V1:
            if (humidity_rise(ha)) {
                fc->fan_state = FAN_V2;
                fc->runtime_seconds = 0;
            }
            break;

        case FAN_V2:
            fc->runtime_seconds++;
            if (fc->runtime_seconds >= FAN_MAX_RUNTIME_S) {
                fc->fan_state = FAN_MAX_TIME;
            } else if(ha->growth_state == GROWTH_FALLING && ha->acc_state != ACCELERATION_RISING){
                fc->fan_state = FAN_MIN_TIME;
                fc->stop_time = fc->runtime_seconds + FAN_MIN_RUNTIME_S;
            }
            break;

        case FAN_MIN_TIME:
            fc->runtime_seconds++;
            if(humidity_rise(ha)){
                fc->fan_state = FAN_V2;
            } else if(fc->runtime_seconds >= fc->stop_time) {
                fc->fan_state = FAN_OFF;
            }
            break;

        case FAN_MAX_TIME:

        default:
            fc->fan_state = FAN_OFF;
            fc->runtime_seconds = 0;
            fc->stop_time = 0;
            break;
    }
}
