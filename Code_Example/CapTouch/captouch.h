

/* 
 * File:   captouch.h
 * Author: MD. Faridul Islam
 * Atmega328P CapTouch Library
 * Created on October 30, 2022, 19:00
 */


#define  CAPTOUCH_ADC_DDR               DDRC
#define  CAPTOUCH_ADC_PORT              PORTC

#define  CAPTOUCH_INT_VREF_ADC_CHANNEL  6
#define  CAPTOUCH_INT_GND_ADC_CHANNEL   15

#define  CAPTOUCH_PRECHARGE_SMPL        1
#define  CAPTOUCH_SAMPLE1_SMPL          1
#define  CAPTOUCH_DISCHARGE_SMPL        1
#define  CAPTOUCH_SAMPLE2_SMPL          1
#define  CAPTOUCH_CHECK_DUR_SMPL        1

#define  CAPTOUCH_THRESHOLD             30
#define  CAPTOUCH_SHORT_PRESS_TIME_MIN  2   
#define  CAPTOUCH_SHORT_PRESS_TIME_MAX  50
#define  CAPTOUCH_LONG_PRESS_TIME       100

#define  CAPTOUCH_AUTO_PWR_DOWN         1

#define  CAPTOUCH_CALIBRATION_SAMPLES   32
#define  CAPTOUCH_TOTAL_TOUCH_KEY       2
#define  CAPTOUCH_TOUCH_KEY_1           4
#define  CAPTOUCH_TOUCH_KEY_2           5

#define  CAPTOUCH_ENABLE_LPF            50
#define  CAPTOUCH_OUTPUT_LPF            50

void     CapTouch_Struct_Init(void);
void 	 CapTouch_Set_Key_Pin_Input(uint8_t current_channel);
void 	 CapTouch_Set_Key_Pin_Analog(uint8_t current_channel);
void 	 CapTouch_Set_Key_Pin_Output(uint8_t current_channel);
void 	 CapTouch_Set_Key_Pin_Output_Low(uint8_t current_channel);
void 	 CapTouch_Set_Key_Pin_Output_High(uint8_t current_channel);

void 	 CapTouch_ADC_Register_Config(void);
void 	 CapTouch_ADC_Wakeup(void);
void 	 CapTouch_ADC_Sleep(void);
uint16_t CapTouch_ADC_Sample(uint8_t channel);

void     CapTouch_Precharge_SH_Cap(uint8_t current_channel);
void     CapTouch_ADC_Sample1(uint8_t current_channel);
void     CapTouch_Precharge_Button(uint8_t current_channel);
void     CapTouch_ADC_Sample2(uint8_t current_channel);
void     CapTouch_Read_Delta(uint8_t current_channel);

void     CapTouch_Calibration(void);
void 	 CapTouch_Response(uint8_t current_channel);

void     CapTouch_Check_Trigger(uint8_t current_channel);
void     CapTouch_Check_Touch_Duration(uint8_t current_channel, uint32_t refernce_time);
void     CapTouch_Scan_Sensors(uint32_t refernce_time);

uint8_t  CapTouch_Short_Pressed(uint8_t current_channel);
uint8_t  CapTouch_Long_Pressed(uint8_t current_channel);
uint8_t  CapTouch_Constant_Pressed(uint8_t current_channel);

uint8_t  CapTouch_Short_Pressed_Filtered(uint8_t current_channel);
uint8_t  CapTouch_Long_Pressed_Filtered(void);

void 	 CapTouch_Set_Short_Press_Min_Time(uint8_t val);
void 	 CapTouch_Set_Short_Press_Max_Time(uint8_t val);
void 	 CapTouch_Set_Long_Press_Time(uint8_t val);

int      CapTouch_Get_RawADC1(uint8_t current_channel);
int      CapTouch_Get_RawADC2(uint8_t current_channel);
int      CapTouch_Get_RawDifferentialADC(uint8_t current_channel);

int 	 CapTouch_Get_Calibration_Value(uint8_t current_channel);
int32_t  CapTouch_Get_Touch_Duration(uint8_t current_channel);
uint8_t  CapTouch_Get_Touch_Response(uint8_t current_channel);
uint8_t  CapTouch_Get_Long_Press_Reg(void);

uint8_t  CapTouch_Get_Reference_Timer_Use_Flag(void);
uint8_t  CapTouch_Get_ErrorCode(uint8_t current_channel);
void 	 CapTouch_Get_Debug_Data(uint8_t current_channel, int *data_out);

void     CapTouch_Init(void);

