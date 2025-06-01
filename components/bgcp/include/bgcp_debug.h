#ifndef BGCDE_CODE_DEBUG_H_
#define BGCDE_CODE_DEBUG_H_



#include "bgcp_protocol_const.h"
#include "stdint.h"
#include "stdarg.h"

extern void log_printf(const char *format, ...);


#define ERR_CODE_LIST	\
	DEF_ERR(BGCP_OK)	\
	DEF_ERR(BGCP_ERR_DATA_SIZE)	\
	DEF_ERR(BGCP_ERR_DATA_FORMAT_ERR)	\
	DEF_ERR(BGCP_ERR_NO_FUNC_ERR)	\
	DEF_ERR(BGCP_ERR_NO_PARAM)	\
	DEF_ERR(BGCP_ERR_READ)	\
	DEF_ERR(BGCP_ERR_NO_DATA)	\
	DEF_ERR(BGCP_ERR_WRITE)	\
	DEF_ERR(BGCP_ERR_NO_FUNC_SETTED)	\
	DEF_ERR(BGCP_ERR_SET_PARAM)	\
	DEF_ERR(BGCP_ERR_UNEXPECTED_DATA_END)


#define DEF_ERR(code)	 code,
enum{
	ERR_CODE_LIST
	BGCP_ERR_MAX
};


#define PARAM_LOG_LIST	\
		DEF_CODE(	P_POWER	 ) \
		DEF_CODE(	P_SPEED_MODE	) \
		DEF_CODE(	P_MaxSPEED_MODE	) \
		DEF_CODE(	P_BPS_ROTOR_MODE	) \
		DEF_CODE(	P_BPS_ROTOR_TYPE	) \
		DEF_CODE(	P_BOOST_MODE	) \
		DEF_CODE(	P_TIMER	) \
		DEF_CODE(	P_TIMER_MODE	) \
		DEF_CODE(	P_SetTIMER_MINUTES	) \
		DEF_CODE(	P_SetTIMER_HOURS	) \
		DEF_CODE(	P_CurTIMER_TIME	) \
		DEF_CODE(	P_SetTEMP_WinterSummer	) \
		DEF_CODE(	P_SetTIMER_TEMP	) \
		DEF_CODE(	P_OPERATION_MODE	) \
		DEF_CODE(	P_IntRH_CTRL	) \
		DEF_CODE(	P_CurTEMP_Water	) \
		DEF_CODE(	P_IntCO2_CTRL	) \
		DEF_CODE(	P_IntPM2_5_CTRL	) \
		DEF_CODE(	P_IntVOC_CTRL	) \
		DEF_CODE(	P_BoostSWITCH_CTRL	) \
		DEF_CODE(	P_FireALARM_CTRL	) \
		DEF_CODE(	P_10V_SENSOR_CTRL	) \
		DEF_CODE(	P_FplcSWITCH_CTRL	) \
		DEF_CODE(	P_SetTEMP	) \
		DEF_CODE(	P_SetRH	) \
		DEF_CODE(	P_SetCO2	) \
		DEF_CODE(	P_SetPM2_5	) \
		DEF_CODE(	P_SetVOC	) \
		DEF_CODE(	P_SelTEMP_SENSOR	) \
		DEF_CODE(	P_CurSelTEMP	) \
		DEF_CODE(	P_CurTEMP_SuAirIn	) \
		DEF_CODE(	P_CurTEMP_SuAirOut	) \
		DEF_CODE(	P_CurTEMP_ExAirIn	) \
		DEF_CODE(	P_CurTEMP_ExAirOut	) \
		DEF_CODE(	P_CurTEMP_Ext	) \
		DEF_CODE(	P_CurVBAT	) \
		DEF_CODE(	P_CurRH_Int	) \
		DEF_CODE(	P_CurRH_Ext	) \
		DEF_CODE(	P_CurCO2_Int	) \
		DEF_CODE(	P_CurCO2_Ext	) \
		DEF_CODE(	P_CurPM2_5_Int	) \
		DEF_CODE(	P_CurPM2_5_Ext	) \
		DEF_CODE(	P_CurVOC_Int	) \
		DEF_CODE(	P_CurVOC_Ext	) \
		DEF_CODE(	P_Cur10V_SENSOR	) \
		DEF_CODE(	P_CurSuAirFLOW	) \
		DEF_CODE(	P_CurExAirFLOW	) \
		DEF_CODE(	P_CurSuPRESS	) \
		DEF_CODE(	P_CurExPRESS	) \
		DEF_CODE(	P_CurBoostSWITCH	) \
		DEF_CODE(	P_CurFireALARM	) \
		DEF_CODE(	P_CurFplcSWITCH	) \
		DEF_CODE(	P_VENTILATION_MODE	) \
		DEF_CODE(	P_MinSPEED	) \
		DEF_CODE(	P_MaxSPEED	) \
		DEF_CODE(	P_SuSPEED0	) \
		DEF_CODE(	P_ExSPEED0	) \
		DEF_CODE(	P_SuSPEED1	) \
		DEF_CODE(	P_ExSPEED1	) \
		DEF_CODE(	P_SuSPEED2	) \
		DEF_CODE(	P_ExSPEED2	) \
		DEF_CODE(	P_SuSPEED3	) \
		DEF_CODE(	P_ExSPEED3	) \
		DEF_CODE(	P_SuSPEED4	) \
		DEF_CODE(	P_ExSPEED4	) \
		DEF_CODE(	P_SuSPEED5	) \
		DEF_CODE(	P_ExSPEED5	) \
		DEF_CODE(	P_ManualSPEED	) \
		DEF_CODE(	P_BlowingSPEED	) \
		DEF_CODE(	P_BOOST_SuSPEED	) \
		DEF_CODE(	P_BOOST_ExSPEED	) \
		DEF_CODE(	P_FPLC_SuSPEED	) \
		DEF_CODE(	P_FPLC_ExSPEED	) \
		DEF_CODE(	P_SuRPM	) \
		DEF_CODE(	P_ExRPM	) \
		DEF_CODE(	P_MinAirFLOW	) \
		DEF_CODE(	P_MaxAirFLOW	) \
		DEF_CODE(	P_SuSPEED0_FLOW	) \
		DEF_CODE(	P_ExSPEED0_FLOW	) \
		DEF_CODE(	P_SuSPEED1_FLOW	) \
		DEF_CODE(	P_ExSPEED1_FLOW	) \
		DEF_CODE(	P_SuSPEED2_FLOW	) \
		DEF_CODE(	P_ExSPEED2_FLOW	) \
		DEF_CODE(	P_SuSPEED3_FLOW	) \
		DEF_CODE(	P_ExSPEED3_FLOW	) \
		DEF_CODE(	P_SuSPEED4_FLOW	) \
		DEF_CODE(	P_ExSPEED4_FLOW	) \
		DEF_CODE(	P_SuSPEED5_FLOW	) \
		DEF_CODE(	P_ExSPEED5_FLOW	) \
		DEF_CODE(	P_MinAirPRESS	) \
		DEF_CODE(	P_MaxAirPRESS	) \
		DEF_CODE(	P_SuSPEED0_PRESS	) \
		DEF_CODE(	P_ExSPEED0_PRESS	) \
		DEF_CODE(	P_SuSPEED1_PRESS	) \
		DEF_CODE(	P_ExSPEED1_PRESS	) \
		DEF_CODE(	P_MainHEATER_TYPE	) \
		DEF_CODE(	P_COOLER_TYPE	) \
		DEF_CODE(	P_DEF_MODE	) \
		DEF_CODE(	P_SetFILTER_TIMER	) \
		DEF_CODE(	P_CurFILTER_TIMER	) \
		DEF_CODE(	C_RESET_FILTER_TIMER	) \
		DEF_CODE(	P_BoostDelaySwitchingOff	) \
		DEF_CODE(	P_BoostDelaySwitchingOn	) \
		DEF_CODE(	P_RTC_TIME	) \
		DEF_CODE(	P_RTC_CALENDAR	) \
		DEF_CODE(	P_FPLC_MODE	) \
		DEF_CODE(	P_WEEK	) \
		DEF_CODE(	P_SetWEEK_ALL	) \
		DEF_CODE(	P_ExtRH_CTRL	) \
		DEF_CODE(	P_ExtCO2_CTRL	) \
		DEF_CODE(	P_ExtPM2_5_CTRL	) \
		DEF_CODE(	P_ExtVOC_CTRL	) \
		DEF_CODE(	P_SearchDeviceID	) \
		DEF_CODE(	P_DevicePWD	) \
		DEF_CODE(	P_TotalWorkingTime	) \
		DEF_CODE(	P_CurALARMS	) \
		DEF_CODE(	C_RESET_ALARM	) \
		DEF_CODE(	P_StatusHEATER	) \
		DEF_CODE(	P_StatusCOOLER	) \
		DEF_CODE(	P_ALARM	) \
		DEF_CODE(	P_StatusAirQUALITY	) \
		DEF_CODE(	P_CLOUD_CTRL	) \
		DEF_CODE(	P_VerMAIN_FMW	) \
		DEF_CODE(	C_RESTORE_FACTORY	) \
		DEF_CODE(	P_StateFILTER	) \
		DEF_CODE(	P_ETHR_AVAIL	) \
		DEF_CODE(	P_ETHR_DHCP	) \
		DEF_CODE(	P_SetETHR_IP	) \
		DEF_CODE(	P_ETHR_SubnetMASK	) \
		DEF_CODE(	P_ETHR_GATEWAY	) \
		DEF_CODE(	P_ETHR_DNS	) \
		DEF_CODE(	P_ETHR_MAC	) \
		DEF_CODE(	C_APPLY_ETHR_PARAM	) \
		DEF_CODE(	P_ETHR_CONN	) \
		DEF_CODE(	P_CurETHR_IP	) \
		DEF_CODE(	P_WIFI_AVAIL	) \
		DEF_CODE(	P_WIFI_MODE	) \
		DEF_CODE(	P_WIFI_SSID	) \
		DEF_CODE(	P_WIFI_PWD	) \
		DEF_CODE(	P_WIFI_PWD_FirstHALF	) \
		DEF_CODE(	P_WIFI_PWD_SecondHALF	) \
		DEF_CODE(	P_WIFI_SECURITY	) \
		DEF_CODE(	P_WIFI_CHANNEL	) \
		DEF_CODE(	P_WIFI_DHCP	) \
		DEF_CODE(	P_SetWIFI_IP	) \
		DEF_CODE(	P_WIFI_SubnetMASK	) \
		DEF_CODE(	P_WIFI_GATEWAY	) \
		DEF_CODE(	P_WIFI_DNS	) \
		DEF_CODE(	С_APPLY_WIFI_PARAM	) \
		DEF_CODE(	P_WIFI_CONN	) \
		DEF_CODE(	C_OUT_WIFI_SetupMODE	) \
		DEF_CODE(	P_CurWIFI_IP	) \
		DEF_CODE(	P_MaxCO2_Int	) \
		DEF_CODE(	P_MaxPM2_5_Int	) \
		DEF_CODE(	P_CLOUD_HOST	) \
		DEF_CODE(	P_CLOUD_PORT	) \
		DEF_CODE(	P_StatusFanBLOWING	) \
		DEF_CODE(	P_DIRECTION_MODE	) \
		DEF_CODE(	P_DeviceTYPE	) \
		DEF_CODE(	P_CurPreHeaterThermostat	) \
		DEF_CODE(	P_CurMainHeaterThermostat	) \
		DEF_CODE(	P_MinSuAirOutTEMP_CTRL	) \
		DEF_CODE(	P_SetMinSuAirOutTEMP	) \
		DEF_CODE(	P_RH_Kp	) \
		DEF_CODE(	P_RH_Ki	) \
		DEF_CODE(	P_RH_Kd	) \
		DEF_CODE(	P_RH_U	) \
		DEF_CODE(	P_CO2_Kp	) \
		DEF_CODE(	P_CO2_Ki	) \
		DEF_CODE(	P_CO2_Kd	) \
		DEF_CODE(	P_CO2_U	) \
		DEF_CODE(	P_PM2_5_Kp	) \
		DEF_CODE(	P_PM2_5_Ki	) \
		DEF_CODE(	P_PM2_5_Kd	) \
		DEF_CODE(	P_PM2_5_U	) \
		DEF_CODE(	P_VOC_Kp	) \
		DEF_CODE(	P_VOC_Ki	) \
		DEF_CODE(	P_VOC_Kd	) \
		DEF_CODE(	P_VOC_U	) \
		DEF_CODE(	P_PreHeater_Kp	) \
		DEF_CODE(	P_PreHeater_Ki	) \
		DEF_CODE(	P_PreHeater_Kd	) \
		DEF_CODE(	P_PreHeater_U	) \
		DEF_CODE(	P_MainHeater_Kp	) \
		DEF_CODE(	P_MainHeater_Ki	) \
		DEF_CODE(	P_MainHeater_Kd	) \
		DEF_CODE(	P_MainHeater_U	) \
		DEF_CODE(	P_CurWaterPRESS	) \
		DEF_CODE(	P_CurWaterFLOW	) \
		DEF_CODE(	P_CurSuFilterPRESS	) \
		DEF_CODE(	P_CurExFilterPRESS	) \
		DEF_CODE(	P_WaterPRESS_CTRL	) \
		DEF_CODE(	P_WaterFLOW_CTRL	) \
		DEF_CODE(	P_CurSuFanPRESS	) \
		DEF_CODE(	P_CurExFanPRESS	) \
		DEF_CODE(	P_FanAlarmCTRL	) \
		DEF_CODE(	P_SetTimeDetectFanALARM	) \
		DEF_CODE(	P_SetTimeOpenVALVE	) \
		DEF_CODE(	P_SetTimeFanBLOWING	) \
		DEF_CODE(	P_BPS_ROTOR_Kp	) \
		DEF_CODE(	P_BPS_ROTOR_Ki	) \
		DEF_CODE(	P_BPS_ROTOR_Kd	) \
		DEF_CODE(	P_BPS_ROTOR_U	) \
		DEF_CODE(	P_KKB_Kp	) \
		DEF_CODE(	P_KKB_Ki	) \
		DEF_CODE(	P_KKB_Kd	) \
		DEF_CODE(	P_KKB_U	) \
		DEF_CODE(	P_SetMainHeaterMANUAL	) \
		DEF_CODE(	P_SetCoolerMANUAL	) \
		DEF_CODE(	P_SetBpsRotorMANUAL	) \
		DEF_CODE(	P_MainHeaterMODE	) \
		DEF_CODE(	P_CoolerMODE	) \
		DEF_CODE(	P_ENGINEER_PWD	) \
		DEF_CODE(	P_StatusSetupMODE	) \
		DEF_CODE(	P_MAIN_UpdFMW	) \
		DEF_CODE(	C_SetFACTORY	) \
		DEF_CODE(	P_KKB_MinTimeOFF	) \
		DEF_CODE(	P_KKB_MinTimeON	) \
		DEF_CODE(	P_KKB_HYSTERESIS	) \
		DEF_CODE(	P_CorrTEMP_SuAirIn	) \
		DEF_CODE(	P_CorrTEMP_SuAirOut	) \
		DEF_CODE(	P_CorrTEMP_ExAirIn	) \
		DEF_CODE(	P_CorrTEMP_ExAirOut	) \
		DEF_CODE(	P_CorrTEMP_Water	) \
		DEF_CODE(	P_CorrTEMP_Ext	) \
		DEF_CODE(	P_WaterPreheatingStatus	) \
		DEF_CODE(	P_WaterMinStartTemp	) \
		DEF_CODE(	P_WaterMaxStartTemp	) \
		DEF_CODE(	P_WaterMinAlarmTemp	) \
		DEF_CODE(	P_WaterMaxAlarmTemp	) \
		DEF_CODE(	P_WaterMaxStartTime	) \
		DEF_CODE(	P_WaterValveMinPos	) \
		DEF_CODE(	P_WaterHeaterAutoRestart	) \
		DEF_CODE(	P_AlarmsHistory	) \
		DEF_CODE(	C_ResetAlarmsHistory	)





#define FUNC_LOG_LIST	\
		DEF_CODE(BGCP_FUNC_R)\
		DEF_CODE(BGCP_FUNC_W)\
		DEF_CODE(BGCP_FUNC_RW)\
		DEF_CODE(BGCP_FUNC_INC_RW)\
		DEF_CODE(BGCP_FUNC_DEC_RW)\
		DEF_CODE(BGCP_FUNC_RESP)

#define CMD_LOG_LIST	\
		DEF_CODE(BGCP_CMD_FUNC)\
		DEF_CODE(BGCP_CMD_SIZE)\
		DEF_CODE(BGCP_CMD_PAGE)\
		DEF_CODE(BGCP_CMD_NOT_SUP)


const char *get_param_desc(unsigned code);
const char *get_func_desc(unsigned code);
const char *get_cmd_desc(unsigned code);
const char *get_err_desc(unsigned code);


#endif /* BGCP_BGCP_DEBUG_H_ */

















































































































































