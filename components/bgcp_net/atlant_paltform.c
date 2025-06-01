#include "bgcp.h"

#include "device.h"
#include "string.h"

#define MAX_DEV_PWD_LEN 9
#define MAX_HOST_NAME_LEN 100
#define NET_BUF_LEN 5096
#define MAX_SSID_LEN 32
#define MAX_PWD_LEN 64

#define FILTER_TIMER_NOT_TRIGGERED 0
#define FILTER_TIMER_TRIGGERED 3
#define WARNING_VAL 2
#define EMERGENCY_VAL 1
#define SENSOR_NOT_PRESENTED -32768
#define ErrExFanStop  1
#define ErrRH_IntAbsent 13
#define ErrVOC_IntAbsent 16
#define ErrFilterTimer 29
#define DEF_RH_THRESHOLD 60
#define DEF_VOC_THRESHOLD 40
#define DEF_WIFI_CH 11
#define DEF_DEV_PWD 1111
#define CLOUD_HOSTNAME "blauberg-group-cloud.com"
#define DEV_HOSTNAME  "device-fan.local"
#define DEF_CLOUD_PORT 1336
#define DEF_WIFI_PWD "11111111"
#define DEF_FIRMWARE "v0.1"

#define FILTER_TIM_EXPIRIED_VAL 3
#define FILTER_TIM_RESET_VAL 0

char device_firmware[] = DEF_FIRMWARE;
const char def_cloud_host_name[] = CLOUD_HOSTNAME;
const char def_device_name[] = DEV_HOSTNAME;


#define KEY1_VAL 0x123456789ABCDEF0ULL
#define KEY2_VAL 0xABCDEF0123456789ULL




void send_emergency_data(uint8_t *data, uint16_t buf_size)
{
   
}




void send_cur_alarms()
{

}

void reset_alarm()
{

}

void wifi_settings_refuse()
{

}

void wifi_settings_apply()
{

}

void reset_alarm_history()
{

}

void send_working_time()
{

}

void send_alarm_status()
{

}

void reset_filter_timer()
{
    
}

void send_filter_time()
{

}

void send_status_wifi_conn()
{

}

void set_default_settings()
{
    
}


#define SUP_TIMER_MODE 1

void send_timer_mode()
{
    uint8_t resp = SUP_TIMER_MODE;
    write_response(&resp, sizeof(resp));
}

void reset_wifi_param()
{

}





#define DEF_RH_THRESHOLD 60
#define DEF_VOC_THRESHOLD 40

#define P_IntervalTimer 1234


void init_param_set()
{
    // filter
    // bind_const_param(P_CurFILTER_TIMER, &net_buf.filter_time, sizeof(FilterTime));
    // bind_binary_param(P_SetFILTER_TIMER, &net_buf.filt_time_set, sizeof(FilterTime));
    // bind_const_param(P_StateFILTER, &net_buf.filter_status, sizeof(uint8_t));
    // bind_cmd_param(C_RESET_FILTER_TIMER, reset_filter_timer);
    // // network
    // bind_str_param(P_CLOUD_HOST, net_buf.net_data.cloud_host, 0, MAX_HOST_NAME_LEN);
    // set_uint16_t_param(P_CLOUD_PORT, &net_buf.net_data.cloud_port, 0, 0XFFFF);
    // // wifi
    // bind_str_param(P_WIFI_SSID, net_buf.sta_auth_data.ssid, 0, MAX_SSID_LEN);
    // bind_str_param(P_WIFI_PWD, net_buf.sta_auth_data.pwd, 0, MAX_PWD_LEN);
    // bind_cmd_param(C_OUT_WIFI_SetupMODE, wifi_settings_refuse);
    // bind_cmd_param(С_APPLY_WIFI_PARAM, wifi_settings_apply);
    // bind_resp_param(P_WIFI_CONN, send_status_wifi_conn);
    // bind_binary_param(P_SetWIFI_IP, &net_buf.net_data.ap_ip, sizeof(uint32_t));
    // bind_cmd_param(C_RESTORE_FACTORY, reset_wifi_param);
    // // platform
    // bind_str_param(P_DevicePWD, net_buf.device_pwd, 0, MAX_DEV_PWD_LEN);
    // bind_resp_param(P_TotalWorkingTime, send_working_time);
    // bind_ranked_param(P_ExSPEED1_FLOW, 
    //                 &net_buf.settings.speed1, 
    //                 speed1_flow_val_list, 
    //                 ARR_LEN(speed1_flow_val_list));
    // bind_ranked_param(P_ExSPEED2_FLOW, 
    //                 &net_buf.settings.speed2,
    //                 speed2_flow_val_list, 
    //                 ARR_LEN(speed2_flow_val_list));
    // bind_ranked_param(P_SetRH, 
    //                 &net_buf.RH_threshold,
    //                 rh_threshold_val_list, 
    //                 ARR_LEN(rh_threshold_val_list));
    // bind_ranked_param(P_SetVOC, 
    //                 &net_buf.VOC_threshold,
    //                 voc_threshold_val_list, 
    //                 ARR_LEN(voc_threshold_val_list));
    // bind_ranked_param(P_BoostDelaySwitchingOn, 
    //                 &net_buf.settings.boost_on_delay,
    //                 on_timer_val_list, 
    //                 ARR_LEN(on_timer_val_list));
    // bind_ranked_param(P_BoostDelaySwitchingOff, 
    //                 &net_buf.settings.boost_off_delay,
    //                 off_timer_val_list, 
    //                 ARR_LEN(off_timer_val_list));
    // bind_ranked_param(P_IntervalTimer, 
    //                 &net_buf.settings.interval_timer,
    //                 interval_tim_val_list, 
    //                 ARR_LEN(interval_tim_val_list));
    // bind_const_param(P_CurRH_Int, &net_buf.RH_perc, sizeof(uint8_t));
    // bind_const_param(P_CurVOC_Int, &net_buf.VOC_perc, sizeof(uint8_t));    
    // bind_const_param(P_VerMAIN_FMW, device_firmware, sizeof(device_firmware));
    // bind_const_param(P_ExRPM, &net_buf.rpm, sizeof(uint16_t));
    // bind_const_param(P_CurBoostSWITCH, &net_buf.boost_switch, sizeof(uint8_t));
    // bind_bool_param(P_BoostSWITCH_CTRL, &net_buf.boost_switch);
    // // emergency
    // bind_resp_param(P_CurALARMS, send_cur_alarms);
    // bind_cmd_param(C_RESET_ALARM, reset_alarm);
    // bind_resp_param(P_ALARM, send_alarm_status);
    // bind_cmd_param(C_RESTORE_FACTORY, set_default_settings);

    // control
    // set_uint8_t_param(P_SPEED_MODE, )
    // bind_const_param(P_BOOST_MODE, )
//    P_ExRPM // (16B)
//    P_BOOST_ExSPEED

//    P_SetTIMER_HOURS
//    P_TIMER
//    P_CurTIMER_TIME

    // P_DeviceTYPE/
    // P_BoostSWITCH_CTRL
//    P_BoostDelaySwitchingOn 
//    P_BoostDelaySwitchingOff
//    P_CurBoostSWITCH
//     // emergency
//     set_uint8_t_param(P_ALARM, send_alarm_status);
//     bind_resp_param(P_CurALARMS, send_cur_alarms);
//     bind_cmd_param(C_RESET_ALARM, reset_alarm);

    // P_CurALARMS // [code][alarm 1/warning 2]...
    // C_RESET_ALARM
    // P_ALARM

//     //P_DevicePWD // [0...8], def 1111
//     //P_POWER
//     P_SPEED_MODE
//     P_TIMER_MODE

//     //P_CLOUD_CTRL
//     //P_CLOUD_HOST
//     //P_CLOUD_PORT

//     //P_WIFI_MODE    
//     P_WIFI_CONN // підключення до AP маршрутизатора
//     P_CurWIFI_IP    // AP or seted
//     //P_WIFI_SSID
//     //P_WIFI_PWD
//     //С_APPLY_WIFI_PARAM
//     //P_WIFI_CHANNEL
//     P_SetWIFI_IP // seted ip
//     //C_OUT_WIFI_SetupMODE

//     P_StatusAirQUALITY
//     C_RESTORE_FACTORY

//     P_VerMAIN_FMW //([V][V][day][month][year])
    
//     P_StateFILTER
//     //C_RESET_FILTER_TIMER
//     //P_CurFILTER_TIMER
//     //P_SetFILTER_TIMER
    
//     // P_IntRH_CTRL
//     // P_IntVOC_CTRL
    
//     P_SetVOC // 20-100%, 60
//     P_SetRH // 1-40%, 40

//     P_CurRH_Int // 20-100%, 0 - no
//     P_CurVOC_Int //

//     P_CurBoostSWITCH // R
//     P_BoostDelaySwitchingOn 
//     P_BoostDelaySwitchingOff
//     P_ExSPEED1_FLOW // м3 для 1 швидкості 
//     P_ExSPEED2_FLOW // м3 для 2 швидкості 
//     // ext
//     P_MaxAirFLOW // максимальний потік 2b M3
//     //P_TotalWorkingTime // [min][hour][day: 2b]
//     P_DeviceTYPE //(2b)
//     P_StatusSetupMODE 

//     P_SetTIMER_HOURS
//    // P_TIMER_MODE

}