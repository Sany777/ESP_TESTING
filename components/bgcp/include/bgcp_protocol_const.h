#ifndef BGCP_PROTOCOL_CONST_H_
#define BGCP_PROTOCOL_CONST_H_



// ETHERNET: [0XFD][0XFD][TYPE][SIZE ID][ID][SIZE PWD][PWD][DATA][CHECKSUM L][CHECKSUM H]

//=========== Тип протокола ============//

#define BGCP_RS485 1
#define BGCP_ETH 2
#define BGCP_ETH_CLOUD 3
#define BGCP_P2P 4
//======================================//

//========= Функции протокола ==========//


//======================================//

//========== Особые комманды ===========//
#define BGCP_CMD_PAGE 0xFF
#define BGCP_CMD_FUNC 0xFC
#define BGCP_CMD_SIZE 0xFE
#define BGCP_CMD_NOT_SUP 0xFD

#define BGCP_FUNC_R 0x1
#define BGCP_FUNC_W  0x2
#define BGCP_FUNC_RW 0x3
#define BGCP_FUNC_INC_RW 0x4
#define BGCP_FUNC_DEC_RW 0x5
#define BGCP_FUNC_RESP 0x6
//======================================//

// R/W/RW
#define P_EEPROM 224

//======== Обмен данными по протоколу BGCP ======//
#define P_POWER 1
		#define OFF 0
		#define	ON 1
		#define	SWITCH 2
#define P_SPEED_MODE 2
//		#define SPEED1 1
//		#define	SPEED2 2
//		#define SPEED3 3
//		#define SPEED4 4
//		#define SPEED5 5
		#define	MANUAL_SPEED 255
#define P_MaxSPEED_MODE 3
		//																								3 - 5
#define P_BPS_ROTOR_MODE 4
		// OFF 0
		// ON 1
		#define AUTO_BPS_ROTOR 2
#define P_BPS_ROTOR_TYPE 5
		// OFF 0
		#define BPS_RELAY 1
		#define BPS_ANALOG 2
		#define ROTOR_RELAY 3
		#define ROTOR_ANALOG 4
		#define BPS_3_POINT 5
#define P_BOOST_MODE 6
		// OFF 0
		// ON 1
		// SWITCH 2
#define P_TIMER 7
		#define Normal_Mode 0
		#define Night_Mode 1
		#define Party_Mode 2
#define P_TIMER_MODE 8
		#define STANDBY 0
		#define TIMER1 1
		#define TIMER2 2
		#define TIMER3 3
		#define TIMER4 4
		#define TIMER5 5
#define P_SetTIMER_MINUTES 9
		// 0 - 59
#define P_SetTIMER_HOURS 10
		// 0 - 23
#define P_CurTIMER_TIME 11
		// 0 - 59 sec
			// 0 - 59 min
				// 0 - 23 hour

#define P_SetTEMP_WinterSummer 12
		// +5...+15
#define P_SetTIMER_TEMP 13
		// +15...+30
#define P_OPERATION_MODE 14
		#define VENTILATION 0
		#define HEATING 1
		#define COOLING 2
		#define AUTO 3
#define P_IntRH_CTRL 15
		// OFF 0
		// ON 1
		// SWITCH 2
#define P_CurTEMP_Water 16
		// -40 - +80
#define P_IntCO2_CTRL															17
		// OFF  															 			0
		// ON  															 				1
		// SWITCH																					2
#define P_IntPM2_5_CTRL														18
		// OFF  															 			0
		// ON  															 				1
		// SWITCH																					2
#define P_IntVOC_CTRL															19
		// OFF  															 			0
		// ON  															 				1
		// SWITCH																					2
#define P_BoostSWITCH_CTRL												20
		// OFF  															 			0
		// ON  															 				1
		// SWITCH																					2
#define P_FireALARM_CTRL													21
		// OFF  															 			0
		// ON  															 				1
		// SWITCH																					2
#define P_10V_SENSOR_CTRL													22
		// OFF  															 			0
		// ON  															 				1
		// SWITCH																					2
#define P_FplcSWITCH_CTRL													23
		// OFF  															 			0
		// ON  															 				1
		// SWITCH																					2
#define P_SetTEMP																	24
		//																								+15...+30
#define P_SetRH																		25
		//																								30 - 95
#define P_SetCO2																	26
		//																								5 -
		//																										2000
#define P_SetPM2_5																27
		//																								5 -
		//																										1000
#define P_SetVOC																	28
		//																								5 - 100
#define P_SelTEMP_SENSOR													29
		#define ExAirInTEMP																0
		#define ExtTEMP																		1
		#define SuAirOutTEMP															2
#define P_CurSelTEMP															30
		//																								-40 - +80
#define P_CurTEMP_SuAirIn													31
		//																								-40 - +80
#define P_CurTEMP_SuAirOut												32
		//																								-40 - +80
#define P_CurTEMP_ExAirIn													33
		//																								-40 - +80
#define P_CurTEMP_ExAirOut												34
		//																								-40 - +80
#define P_CurTEMP_Ext															35
		//																								-40 - +80
#define P_CurVBAT																	36
		//																								0 -
		//																										5000
#define P_CurRH_Int																37
		//																								30 - 95
#define P_CurRH_Ext																38
		//																								30 - 95
#define P_CurCO2_Int															39
		//																								0 -
			//																									2000
#define P_CurCO2_Ext															40
		//																								0 -
			//																									2000
#define P_CurPM2_5_Int														41
		//																								5 -
			//																									1000
#define P_CurPM2_5_Ext														42
		//																								5 -
			//																									1000
#define P_CurVOC_Int															43
		//																								5 - 100
#define P_CurVOC_Ext															44
		//																								5 - 100
#define P_Cur10V_SENSOR														45
		//																								0 - 100
#define P_CurSuAirFLOW														46
		//																								5 -
			//																									...
#define P_CurExAirFLOW														47
		//																								5 -
			//																									...
#define P_CurSuPRESS															48
		//
			//
#define P_CurExPRESS															49
		//
			//
#define P_CurBoostSWITCH													50
		// OFF  															 			0
		// ON  															 				1
#define P_CurFireALARM														51
		// OFF  															 			0
		// ON  															 				1
#define P_CurFplcSWITCH														52
		// OFF  															 			0
		// ON  															 				1
#define P_VENTILATION_MODE												53
		#define PERCENT_MODE															0
		#define CONSTANT_FLOW															1
		#define CONSTANT_PRESSURE													2
#define P_MinSPEED																54
		//																								0 - 100
#define P_MaxSPEED																55
		//																								0 - 100
#define P_SuSPEED0																56
		//																								0 - off, min - max
#define P_ExSPEED0																57
		//																								0 - off, min - max
#define P_SuSPEED1																58
		//																								min - max
#define P_ExSPEED1																59
		//																								min - max
#define P_SuSPEED2																60
		//																								min - max
#define P_ExSPEED2																61
		//																								min - max
#define P_SuSPEED3																62
		//																								min - max
#define P_ExSPEED3																63
		//																								min - max
#define P_SuSPEED4																64
		//																								min - max
#define P_ExSPEED4																65
		//																								min - max
#define P_SuSPEED5																66
		//																								min - max
#define P_ExSPEED5																67
		//																								min - max
#define P_ManualSPEED															68
		//																								min - max
#define P_BlowingSPEED														69
		//																								min - max
#define P_BOOST_SuSPEED														70
		//																								min - max
#define P_BOOST_ExSPEED														71
		//																								min - max
#define P_FPLC_SuSPEED														72
		//																								min - max
#define P_FPLC_ExSPEED														73
		//																								min - max
#define P_SuRPM																		74
		//																								0 -
			//																									...
#define P_ExRPM																		75
		//																								0 -
			//																									...
#define P_MinAirFLOW															76
		//
			//
#define P_MaxAirFLOW															77
		//
			//
#define P_SuSPEED0_FLOW														78
		//																								0 - off, min -
			//																									max
#define P_ExSPEED0_FLOW														79
		//																								0 - off, min -
			//																									max
#define P_SuSPEED1_FLOW														80
		//																								min -
			//																									max
#define P_ExSPEED1_FLOW														81
		//																								min -
			//																									max
#define P_SuSPEED2_FLOW														82
		//																								min -
			//																									max
#define P_ExSPEED2_FLOW														83
		//																								min -
			//																									max
#define P_SuSPEED3_FLOW														84
		//																								min -
			//																									max
#define P_ExSPEED3_FLOW														85
		//																								min -
			//																									max
#define P_SuSPEED4_FLOW														86
		//																								min -
			//																									max
#define P_ExSPEED4_FLOW														87
		//																								min -
			//																									max
#define P_SuSPEED5_FLOW														88
		//																								min -
			//																									max
#define P_ExSPEED5_FLOW														89
		//																								min -
			//																									max
#define P_MinAirPRESS															90
		//
			//
#define P_MaxAirPRESS															91
		//
			//
#define P_SuSPEED0_PRESS													92
		//																								0 - off, min -
			//																									max
#define P_ExSPEED0_PRESS													93
		//																								0 - off, min -
			//																									max
#define P_SuSPEED1_PRESS													94
		//																								min -
			//																									max
#define P_ExSPEED1_PRESS													95
		//																								min -
			//																									max
#define P_MainHEATER_TYPE													96
		// OFF  															 			0
		#define HEATER_ELECT  														1
		#define HEATER_WATER															2
#define P_COOLER_TYPE															97
		// OFF  															 			0
		#define COOLER_RELAY 															1
		#define COOLER_ANALOG															2
#define P_DEF_MODE																98
		// OFF																						0
		#define PREHEATING_DEF														1
		#define	BPS_ROTOR_DEF															2
		#define OFF_SuFLOW_DEF														3
		#define RECIR_DEF																	4
#define P_SetFILTER_TIMER													99
		//																								70 - 182
#define P_CurFILTER_TIMER													100
		//																								0 - 59
			//																									0 - 23
				//																										0 - 181
#define C_RESET_FILTER_TIMER											101
		// any of byte
#define P_BoostDelaySwitchingOff									102
		//																								0 - 60
#define P_BoostDelaySwitchingOn										103
		//																								0 - 15
#define P_RTC_SECONDS															104
		//																								0 - 59
#define P_RTC_MINUTES															105
		//																								0 - 59
#define P_RTC_HOURS																106
		//																								0 - 23
#define P_RTC_DATE																107
		//																								1 - 31
#define P_RTC_WeekDay															108
		//																								1 - 7
#define P_RTC_MONTH																109
		//																								1 - 12
#define P_RTC_YEAR																110
		//																								0 - 99
#define P_RTC_TIME																111
		//																								0 - 59
			//																									0 - 59
				//																										0 - 23
#define P_RTC_CALENDAR														112
		//																								1 - 31
			//																									1 - 7
				//																										1 - 12
					//																											0 - 99
#define P_FPLC_MODE																113
		// OFF  															 			0
		// ON  															 				1
		// SWITCH																					2
#define P_WEEK 																		114
		// OFF  															 			0
		// ON  															 				1
		// SWITCH																					2
#define P_SetWEEK_SPEED														115
		// weekday																				1 - 7
			// period																						1 - 4
				// speed mode																					0 - MaxSpeed
#define P_SetWEEK_TEMP														116
		// weekday																				1 - 7
			// period																						1 - 4
				// temp																								0, 15 - 30

#define P_SetWEEK_EndPERIOD												118
		// weekday																				1 - 7
			// period																						1 - 3
				// min                                            		0 - 59
					// hours																								0 - 23
#define P_SetWEEK_ALL															119
		// weekday																				1 - 7
			// period																						1 - 3
				// speed mode																					0 - MaxSpeed
					// temp																									0, 15 - 30
						// min                                            				0 - 59
							// hours																										0 - 23
#define P_ExtRH_CTRL															120
#define P_ExtCO2_CTRL															121
#define P_ExtPM2_5_CTRL														122
#define P_ExtVOC_CTRL															123
#define P_SearchDeviceID													124
#define P_DevicePWD																125
#define P_TotalWorkingTime												126
		//																								0 - 59 минут
			//																									0 - 23 часов
				//																										0  - дней
					//																									 65535 дней
#define P_CurALARMS																127
		// код аварии/предуприждения
			// тип: 1-авария; 2-предуприждение
					//...
#define C_RESET_ALARM															128
#define P_StatusHEATER														129
#define P_StatusCOOLER														130
#define P_ALARM																		131
		// OFF																						0
		#define Alarm																			1  // Обозначает серьезную ошибку в работе. Приточка принудительно выключается.
		#define Warning																		2  // Предуприждение. Приточка принудительно не выключается.
#define P_StatusAirQUALITY												132
		// RH																							0 - 1
			// CO2																							0 - 1
				// PM2.5																							0 - 1
					// VOC																									0 - 1
#define P_CLOUD_CTRL															133
#define P_VerMAIN_FMW															134
#define C_RESTORE_FACTORY													135
#define P_StateFILTER															136
		//																								0 - чистый, 1 - нужно менять
#define P_ETHR_AVAIL															137
#define P_ETHR_DHCP																138
#define P_SetETHR_IP															139
#define P_ETHR_SubnetMASK													140
#define P_ETHR_GATEWAY														141
#define P_ETHR_DNS																142
#define P_ETHR_MAC																143
#define C_APPLY_ETHR_PARAM												144
#define P_ETHR_CONN																145
#define P_CurETHR_IP															146
#define P_WIFI_AVAIL															147
#define P_WIFI_MODE																148
		//#define	STATION															1
		//#define	AP																	2
#define P_WIFI_SSID																149
		// ...
		// max 32 bytes
#define P_WIFI_PWD																150
		// ...
		// max 64 bytes
#define P_WIFI_PWD_FirstHALF											151
		// ...
		// max 32 bytes
#define P_WIFI_PWD_SecondHALF											152
		// ...
		// max 32 bytes
#define P_WIFI_SECURITY														153
		//#define	SECURITY_OPEN 													48
		//#define	SECURITY_WPA_PSK 												50
		//#define	SECURITY_WPA2_PSK 											51
		//#define	SECURITY_WPA_WPA2_PSK 									52
#define P_WIFI_CHANNEL														154
		//																								1 - 13
#define P_WIFI_DHCP																155
		//#define	STATIC															0
		//#define	DHCP																1
#define P_SetWIFI_IP															156
		// first byte
			// second byte
				// third byte
					// fourth byte
#define	P_WIFI_SubnetMASK													157
		// first byte
			// second byte
				// third byte
					// fourth byte
#define P_WIFI_GATEWAY														158
		// first byte
			// second byte
				// third byte
					// fourth byte
#define P_WIFI_DNS																159
		// first byte
			// second byte
				// third byte
					// fourth byte
#define С_APPLY_WIFI_PARAM												160
		// any of byte
#define P_WIFI_CONN																161
		// OFF  															 			0
		// ON  															 				1
#define C_OUT_WIFI_SetupMODE											162
		// any of byte
#define P_CurWIFI_IP															163
		// first byte
			// second byte
				// third byte
					// fourth byte
#define P_rs485_BAUD															164
		#define BAUD9600																	0
		#define BAUD14400																	1
		#define BAUD19200																	2
		#define BAUD38400																	3
		#define BAUD57600																	4
		#define BAUD115200																5
#define P_rs485_ADDR															165
		// 																								1 - 32
#define P_rs485_StopBITS													166
		// 			1																					0
		//			1.5																				1
		//			2																					2
#define P_rs485_PARITY														167
		//			none																			0
		//			even																			1
		// 			odd																				2


#define P_MaxCO2_Int															178
		//																								500 -
			//																							 10000
#define P_MaxPM2_5_Int														179
		//																								500 -
			//																							 10000
#define P_CLOUD_HOST															180
#define P_CLOUD_PORT															181
		//																								1 -
			//																									65535
#define P_StatusFanBLOWING												182
#define P_DIRECTION_MODE                          183

#define P_DeviceTYPE															185
		//																								0 -
			//																									65535
#define P_CurPreHeaterThermostat									186
		// OFF  															 			0
		// ON  															 				1
#define P_CurMainHeaterThermostat									187
		// OFF  															 			0
		// ON  															 				1
#define P_MinSuAirOutTEMP_CTRL										188
		// OFF  															 			0
		// ON  															 				1
		// SWITCH																					2
#define P_SetMinSuAirOutTEMP											189
		//																								+5...+15
#define P_RH_Kp																		190
		//																								0 -
			//																									1000
#define P_RH_Ki																		191
		//																								0 -
			//																									1000
#define P_RH_Kd																		192
		//																								0 -
			//																									1000
#define P_RH_U																		193
		//																								0 - 100%
#define P_CO2_Kp																	194
		//																								0 -
			//																									1000
#define P_CO2_Ki																	195
		//																								0 -
			//																									1000
#define P_CO2_Kd																	196
		//																								0 -
			//																									1000
#define P_CO2_U																		197
		//																								0 - 100%
#define P_PM2_5_Kp																198
		//																								0 -
			//																									1000
#define P_PM2_5_Ki																199
		//																								0 -
			//																									1000
#define P_PM2_5_Kd																200
		//																								0 -
			//																									1000
#define P_PM2_5_U																	201
		//																								0 - 100%
#define P_VOC_Kp																	202
		//																								0 -
			//																									1000
#define P_VOC_Ki																	203
		//																								0 -
			//																									1000
#define P_VOC_Kd																	204
		//																								0 -
			//																									1000
#define P_VOC_U																		205
		//																								0 - 100%
#define P_PreHeater_Kp														206
		//																								0 -
			//																									1000
#define P_PreHeater_Ki														207
		//																								0 -
			//																									1000
#define P_PreHeater_Kd														208
		//																								0 -
			//																									1000
#define P_PreHeater_U															209
		//																								0 - 100%
#define P_MainHeater_Kp														210
		//																								0 -
			//																									1000
#define P_MainHeater_Ki														211
		//																								0 -
			//																									1000
#define P_MainHeater_Kd														212
		//																								0 -
			//																									1000
#define P_MainHeater_U														213
		//																								0 - 100%
#define P_CurWaterPRESS														214
#define P_CurWaterFLOW														215
#define P_CurSuFilterPRESS												216
#define P_CurExFilterPRESS												217
#define P_WaterPRESS_CTRL													218
#define P_WaterFLOW_CTRL													219
#define P_CurSuFanPRESS														220
#define P_CurExFanPRESS														221
#define P_FanAlarmCTRL														222
		//	OFF																						0
		//	Taxo																					1 - 254
		//	PRESS																					255
#define P_SetTimeDetectFanALARM										223
		//																								5 - 120


#define P_SetTimeOpenVALVE												225
		//																								0 - 240
#define P_SetTimeFanBLOWING												226
		//																								20 - 240
#define P_BPS_ROTOR_Kp														227
#define P_BPS_ROTOR_Ki														228
#define P_BPS_ROTOR_Kd														229
#define P_BPS_ROTOR_U															230
#define P_KKB_Kp																	231
#define P_KKB_Ki																	232
#define P_KKB_Kd																	233
#define P_KKB_U																		234
#define P_SetMainHeaterMANUAL											235
		//																								0 - 100
#define P_SetCoolerMANUAL													236
		//																								0 - 100
#define P_SetBpsRotorMANUAL												237
		//																								0 - 100
#define P_MainHeaterMODE													238
		// OFF																						0
		// ON																							1
		#define AUTO_MainHEATER														2
#define P_CoolerMODE															239
		// OFF																						0
		// ON																							1
		#define AUTO_COOLER																2


#define P_ENGINEER_PWD														241
#define P_StatusSetupMODE													242
		// OFF																						0
		// ON																							1


#define P_MAIN_UpdFMW															257
		// 1 пакет удачно принят
		// 2 пакет уже записан
		// 3 пропустили пакет
		// 4 CRC32 файла OK
		// 5 CRC32 файла ERROR
		// 6 неверный размер пакета
#define C_SetFACTORY															258
#define P_KKB_MinTimeOFF													259
		//																								0 - 20 мин
#define P_KKB_MinTimeON														260
		//																								0 - 20 мин
#define P_KKB_HYSTERESIS													261
		//																								1 - 10 °C
#define P_CorrTEMP_SuAirIn												262
		//																								-50.0 ... +50.0
#define P_CorrTEMP_SuAirOut												263
		//																								-50.0 ... +50.0
#define P_CorrTEMP_ExAirIn												264
		//																								-50.0 ... +50.0
#define P_CorrTEMP_ExAirOut												265
		//																								-50.0 ... +50.0
#define P_CorrTEMP_Water													266
		//																								-50.0 ... +50.0
#define P_CorrTEMP_Ext														267
		//																								-50.0 ... +50.0

#define P_BPS_Position														270
		#define BPS_SupplyDuct														0
		#define BPS_ExtractDuct														1


#define P_WaterPreheatingStatus										276
		// OFF																						0
		// ON																							1
#define P_WaterMinStartTemp												277
		//																								+30°C ... +60°C
#define P_WaterMaxStartTemp												278
		//																								+30°C ... +60°C
#define P_WaterMinAlarmTemp												279
		//																								+10°C ... +30°C
#define P_WaterMaxAlarmTemp												280
		//																								+10°C ... +30°C
#define P_WaterMaxStartTime												281
		//																								2 ... 30 минут
#define P_WaterValveMinPos												282
		//																								0 ... 100%
#define P_WaterHeaterAutoRestart									283
		// OFF  															 			0
		// ON  															 				1
		// SWITCH																	2
#define P_AlarmsHistory														284
#define C_ResetAlarmsHistory											285
//==========================================//
#define P_TestMode 812
#define KEY_TEST_MODE 0x6609F900

#endif /* BGCP_BGCP_PROTOCOL_CONST_H_ */
