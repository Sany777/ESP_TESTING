#include "unity.h"
#include "esp_log.h"
#include "string.h"
#include "stdio.h"
#include "bgcp.h"


static const char *TAG = "BGCP";
static uint8_t uint8_t_val = 7, uint8_t_val2 = 9;
static uint16_t uint16_t_val = 123;
static uint16_t uint16_t_val2 = 0;
static bool bool_val = true;

// values for write
bool bool_w = false;
static uint8_t uint8_t_w = 22;
static uint16_t uint16_t_w2 = 321;

float f;
uint64_t dw;
uint32_t WaterValveMinPos;
int AlarmsHistory;

float f_w = 12.3;
uint64_t dw_w = 12345678;
uint32_t WaterValveMinPos_w = 0xFFFFFF00;
int AlarmsHistory_w = -1;



TEST_CASE("create paramets", "[BGCP]")
{
	TEST_ASSERT(bind_bool_param(P_POWER, &bool_val));
	TEST_ASSERT(set_uint8_t_param(P_BPS_ROTOR_MODE, &uint8_t_val, 0, 222));
	TEST_ASSERT(set_uint16_t_param(P_10V_SENSOR_CTRL, &uint16_t_val, 0, 3000));
	TEST_ASSERT(set_uint16_t_param(P_RTC_CALENDAR, &uint16_t_val2, 0, 1000));
	TEST_ASSERT(set_uint8_t_param(P_FPLC_MODE, &uint8_t_val2, 0, 10));
}


TEST_CASE("test the requests creation", "[BGCP]")
{
	reset_req_queue();
	//  write uint8_t param
	queue_push_param_uint8_t_data(BGCP_FUNC_W, P_POWER, bool_w);
	//  rrw  param
	queue_push_param_req(BGCP_FUNC_INC_RW, P_10V_SENSOR_CTRL);
	queue_push_param_uint16_t_data(BGCP_FUNC_W, P_RTC_CALENDAR, uint16_t_w2);
	queue_push_param_uint8_t_data(BGCP_FUNC_RW, P_BPS_ROTOR_MODE, uint8_t_w);
	//  read param
	queue_push_param_req(BGCP_FUNC_R, P_POWER);
	// unknown parameter
	queue_push_param_uint8_t_data(BGCP_FUNC_W, P_CurTEMP_ExAirOut, 123);
	queue_push_param_req(BGCP_FUNC_DEC_RW, P_FPLC_MODE);
	queue_push_param_req(BGCP_FUNC_R, P_CurTEMP_ExAirOut);
	queue_push_param_req(BGCP_FUNC_DEC_RW, P_WIFI_DNS);
	uint8_t *data = get_req_queue_data();
   TEST_ASSERT_GREATER_THAN(11, get_req_queue_size());
   TEST_ASSERT(data[0] == BGCP_FUNC_W);
   TEST_ASSERT(data[1] == P_POWER);
   TEST_ASSERT(data[2] == bool_w);
   TEST_ASSERT(data[3] == BGCP_CMD_FUNC);
   TEST_ASSERT(data[4] == BGCP_FUNC_INC_RW);
   TEST_ASSERT(data[5] == P_10V_SENSOR_CTRL);
   TEST_ASSERT(data[6] == BGCP_CMD_FUNC);
   TEST_ASSERT(data[7] == BGCP_FUNC_W);
   TEST_ASSERT(data[8] == BGCP_CMD_SIZE);
   TEST_ASSERT(data[9] == sizeof(uint16_t_w2));
   TEST_ASSERT(data[10] == P_RTC_CALENDAR);
}


TEST_CASE("test parse", "[BGCP]")
{
	uint8_t buf[200];
	memcpy(buf, get_req_queue_data(), get_req_queue_size());
	parse(buf,  get_req_queue_size());
	memcpy(buf, get_req_queue_data(), get_req_queue_size());
   TEST_ASSERT_GREATER_THAN(7, get_req_queue_size());
   TEST_ASSERT(buf[0] == BGCP_FUNC_RESP);
   TEST_ASSERT(buf[1] == BGCP_CMD_SIZE);
   TEST_ASSERT(buf[2] == sizeof(uint16_t_w2));
   TEST_ASSERT(buf[3] == P_10V_SENSOR_CTRL);
   TEST_ASSERT(buf[6] == P_BPS_ROTOR_MODE);
   TEST_ASSERT(buf[7] == uint8_t_w);
   TEST_ASSERT(buf[8] == P_POWER);
   TEST_ASSERT(buf[9] == bool_w);
}


TEST_CASE("test commit changes", "[BGCP]")
{
	TEST_ASSERT_EQUAL(bool_w, bool_val);
	TEST_ASSERT_EQUAL(124, uint16_t_val);
	TEST_ASSERT_EQUAL(uint16_t_val2, uint16_t_w2);
	TEST_ASSERT_EQUAL(uint8_t_w, uint8_t_val);
	TEST_ASSERT_EQUAL(8, uint8_t_val2);
}