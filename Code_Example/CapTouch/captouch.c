

/* 
 * File:   captouch.c
 * Author: MD. Faridul Islam
 * Atmega328P CapTouch Library
 * Created on October 30, 2022, 19:00
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "CapTouch.h"

uint8_t    CapTouch_Pins[CAPTOUCH_TOTAL_TOUCH_KEY]={
  CAPTOUCH_TOUCH_KEY_1, 
  CAPTOUCH_TOUCH_KEY_2
};

typedef struct captouch_t{
  uint8_t  Channel;
  uint8_t  Result[CAPTOUCH_TOTAL_TOUCH_KEY];
  int      CalibrationValue[CAPTOUCH_TOTAL_TOUCH_KEY];
  int      RawADC1[CAPTOUCH_TOTAL_TOUCH_KEY];
  int      RawADC2[CAPTOUCH_TOTAL_TOUCH_KEY];
  int      RawDiffADC[CAPTOUCH_TOTAL_TOUCH_KEY];
  int      DiffThreshold[CAPTOUCH_TOTAL_TOUCH_KEY];
  uint8_t  TriggerMethod[CAPTOUCH_TOTAL_TOUCH_KEY];
  uint8_t  LastTouchState[CAPTOUCH_TOTAL_TOUCH_KEY];
  int32_t  TouchDuration[CAPTOUCH_TOTAL_TOUCH_KEY];
  int32_t  TouchDurationTemp[CAPTOUCH_TOTAL_TOUCH_KEY];
  uint16_t MinShortPressTime;
  uint16_t MaxShortPressTime;
  uint16_t LongPressTime;
  uint8_t  LongPressFlag[CAPTOUCH_TOTAL_TOUCH_KEY];
  uint8_t  UsingReferenceTimer;
  uint8_t  ErrorCode[CAPTOUCH_TOTAL_TOUCH_KEY];
}captouch_t;


#ifdef CAPTOUCH_ENABLE_LPF
typedef struct iir_lpf_t{
  uint8_t  Alpha ;
  int32_t  Input ;
  int32_t  Output;
}iir_lpf_t;
#endif

captouch_t CapTouch;

#ifdef CAPTOUCH_ENABLE_LPF
iir_lpf_t  IIR_LPF[CAPTOUCH_TOTAL_TOUCH_KEY*3];
#endif
  
void CapTouch_Struct_Init(void){
  CapTouch.Channel=0;
  for(uint8_t i=0;i<CAPTOUCH_TOTAL_TOUCH_KEY;i++){
    CapTouch.Result[i]=0;
    CapTouch.CalibrationValue[i]=0;
    CapTouch.RawADC1[i]=0;
    CapTouch.RawADC2[i]=0;
    CapTouch.RawDiffADC[i]=0;
    CapTouch.DiffThreshold[i]=CAPTOUCH_THRESHOLD;
    CapTouch.TriggerMethod[i]=0;
    CapTouch.LastTouchState[i]=0;
    CapTouch.TouchDuration[i]=0;
    CapTouch.TouchDurationTemp[i]=0;
    CapTouch.MinShortPressTime=CAPTOUCH_SHORT_PRESS_TIME_MIN;
    CapTouch.MaxShortPressTime=CAPTOUCH_SHORT_PRESS_TIME_MAX;
    CapTouch.LongPressTime=CAPTOUCH_LONG_PRESS_TIME;
    CapTouch.LongPressFlag[i]=0;
    CapTouch.UsingReferenceTimer=0;
    CapTouch.ErrorCode[i]=0;
  }
}

#ifdef CAPTOUCH_ENABLE_LPF
void CapTouch_IIR_LPF_Struct_Init(void){
  uint8_t start=2;
  for(uint8_t i=0;i<CAPTOUCH_TOTAL_TOUCH_KEY*3;i++){
    IIR_LPF[i].Alpha=CAPTOUCH_ENABLE_LPF;
	IIR_LPF[i].Input=0;
	IIR_LPF[i].Output=0;
    if(i == start){
      IIR_LPF[i].Alpha = CAPTOUCH_OUTPUT_LPF;
      start+=3;
    }
  }
}

void CapTouch_IIR_LPF_Set_Alpha(uint8_t lpf_index, uint8_t val){
  IIR_LPF[lpf_index].Alpha = val;
}

uint8_t CapTouch_IIR_LPF_Get_Alpha(uint8_t lpf_index){
  return IIR_LPF[lpf_index].Alpha;
}

int32_t CapTouch_IIR_LPF_Get_Filtered_Value(uint8_t lpf_index, int32_t val){
  int32_t tmp1=CapTouch_IIR_LPF_Get_Alpha(lpf_index);
  tmp1*=val;
  int32_t tmp2=100;
  tmp2-=CapTouch_IIR_LPF_Get_Alpha(lpf_index);
  tmp2*=IIR_LPF[lpf_index].Output ;
  IIR_LPF[lpf_index].Output = tmp1 + tmp2;
  IIR_LPF[lpf_index].Output/=100;
  return IIR_LPF[lpf_index].Output;
}
#endif

void CapTouch_Set_Key_Pin_Input(uint8_t current_channel){
  CAPTOUCH_ADC_DDR &=~(1<<current_channel);
}

void CapTouch_Set_Key_Pin_Analog(uint8_t current_channel){
  CAPTOUCH_ADC_DDR &=~(1<<current_channel);
  CAPTOUCH_ADC_PORT&=~(1<<current_channel);
}

void CapTouch_Set_Key_Pin_Output(uint8_t current_channel){
  CAPTOUCH_ADC_DDR |= (1<<current_channel);
}

void CapTouch_Set_Key_Pin_Output_Low(uint8_t current_channel){
  CAPTOUCH_ADC_PORT&=~(1<<current_channel);
}

void CapTouch_Set_Key_Pin_Output_High(uint8_t current_channel){
  CAPTOUCH_ADC_PORT|=(1<<current_channel);
}

void CapTouch_ADC_Register_Config(void){
  ADMUX =0x40;
  ADCSRA=0x87;
}

void CapTouch_ADC_Wakeup(void){
  ADCSRA|=(1<<ADEN);
}

void CapTouch_ADC_Sleep(void){
  ADCSRA&=~(1<<ADEN);
}

uint16_t CapTouch_ADC_Sample(uint8_t channel){
  ADMUX=0x40|channel;
  if(ADCSRA!=0x87){
    ADCSRA=0x87;
  }
  ADCSRA|=(1<<ADSC);
  while(!(ADCSRA & (1<<ADIF)));
  ADCSRA|=(1<<ADIF);
  return ADCW;
}

void CapTouch_Precharge_SH_Cap(uint8_t current_channel){
  CapTouch_Set_Key_Pin_Output(CapTouch_Pins[current_channel]);
  CapTouch_Set_Key_Pin_Output_Low(CapTouch_Pins[current_channel]);
  for(uint8_t i=0;i<CAPTOUCH_PRECHARGE_SMPL;i++){
    CapTouch_ADC_Sample(CAPTOUCH_INT_VREF_ADC_CHANNEL);
  }
}

void CapTouch_ADC_Sample1(uint8_t current_channel){
  CapTouch_Set_Key_Pin_Analog(CapTouch_Pins[current_channel]);
  CapTouch.RawADC1[current_channel]=0;
  for(uint8_t i=0;i<CAPTOUCH_SAMPLE1_SMPL;i++){
    CapTouch.RawADC1[current_channel]+=CapTouch_ADC_Sample(CapTouch_Pins[current_channel]);
  }
  #ifdef CAPTOUCH_ENABLE_LPF
	CapTouch.RawADC1[current_channel] = CapTouch_IIR_LPF_Get_Filtered_Value((current_channel*3) ,CapTouch.RawADC1[current_channel]);
  #endif
}

void CapTouch_Precharge_Button(uint8_t current_channel){
  CapTouch_Set_Key_Pin_Output(CapTouch_Pins[current_channel]);
  CapTouch_Set_Key_Pin_Output_High(CapTouch_Pins[current_channel]);
  for(uint8_t i=0;i<CAPTOUCH_DISCHARGE_SMPL;i++){
    CapTouch_ADC_Sample(CAPTOUCH_INT_GND_ADC_CHANNEL);
  }
}

void CapTouch_ADC_Sample2(uint8_t current_channel){
  CapTouch_Set_Key_Pin_Analog(CapTouch_Pins[current_channel]);
  CapTouch.RawADC2[current_channel]=0;
  for(uint8_t i=0;i<CAPTOUCH_SAMPLE2_SMPL;i++){
    CapTouch.RawADC2[current_channel]+=CapTouch_ADC_Sample(CapTouch_Pins[current_channel]);
  }
  #ifdef CAPTOUCH_ENABLE_LPF
	CapTouch.RawADC2[current_channel] = CapTouch_IIR_LPF_Get_Filtered_Value((current_channel*3)+1 ,CapTouch.RawADC2[current_channel]);
  #endif
}

void CapTouch_Read_Delta(uint8_t current_channel){
  CapTouch_Precharge_SH_Cap(current_channel);
  CapTouch_ADC_Sample1(current_channel);
  CapTouch_Precharge_Button(current_channel);
  CapTouch_ADC_Sample2(current_channel);
  int delta=CapTouch.RawADC1[current_channel];
  delta-=CapTouch.RawADC2[current_channel];
  CapTouch.RawDiffADC[current_channel]=delta;
  #ifdef CAPTOUCH_ENABLE_LPF
    CapTouch.RawDiffADC[current_channel] = CapTouch_IIR_LPF_Get_Filtered_Value((current_channel*3)+2, delta);
  #endif
}


void CapTouch_Calibration(void){
  int32_t sum[CAPTOUCH_TOTAL_TOUCH_KEY];
  for(uint8_t key=0;key<CAPTOUCH_TOTAL_TOUCH_KEY;key++){
	sum[key]=0;
  }

  for(int i=0;i<CAPTOUCH_CALIBRATION_SAMPLES;i++){
    for(uint8_t key=0;key<CAPTOUCH_TOTAL_TOUCH_KEY;key++){
      CapTouch_Read_Delta(key);
      sum[key]+=CapTouch.RawDiffADC[key];
	}
  }
  
  for(uint8_t key=0;key<CAPTOUCH_TOTAL_TOUCH_KEY;key++){
	sum[key]/=CAPTOUCH_CALIBRATION_SAMPLES;
    CapTouch.CalibrationValue[key]=sum[key]-CapTouch.DiffThreshold[key];
  }
}


void CapTouch_Response(uint8_t current_channel){
  CapTouch_Read_Delta(current_channel);
  if(CapTouch.CalibrationValue[current_channel] > CapTouch.RawDiffADC[current_channel]){
    CapTouch.Result[current_channel]=1;
  }else{
    CapTouch.Result[current_channel]=0;
  }
}

void CapTouch_Check_Trigger(uint8_t current_channel){
  uint8_t current_state=0;
  CapTouch_Response(current_channel);
  current_state=CapTouch.Result[current_channel];
  if((CapTouch.LastTouchState[current_channel]==0) && (current_state==1)){
    CapTouch.LongPressFlag[current_channel]=0;     //Rising Edge
    CapTouch.TriggerMethod[current_channel]=0x01;
  }else if((CapTouch.LastTouchState[current_channel]==1) && (current_state==0)){
    CapTouch.TriggerMethod[current_channel]=0x04;  //Falling Edge
  }else if((CapTouch.LastTouchState[current_channel]==1) && (current_state==1)){
    CapTouch.TriggerMethod[current_channel]=0x02;  //Constant Touched
  }else{
    CapTouch.TriggerMethod[current_channel]=0x00;  //No Touch Event Occured
  }
  CapTouch.LastTouchState[current_channel]=current_state;
}


void CapTouch_Check_Touch_Duration(uint8_t current_channel, uint32_t refernce_time){
  int32_t duration=0;
  CapTouch_Check_Trigger(current_channel);
  if(CapTouch.TriggerMethod[current_channel] == 0x01){
    CapTouch.UsingReferenceTimer|=(1<<current_channel);
    CapTouch.TouchDurationTemp[current_channel]=refernce_time;
  }else if( (CapTouch.TriggerMethod[current_channel] == 0x04) || (CapTouch.TriggerMethod[current_channel] == 0x02) ){
    duration = refernce_time;
    duration-=CapTouch.TouchDurationTemp[current_channel];
    if(duration>=0){
      CapTouch.TouchDuration[current_channel]=duration;
    }
    if(CapTouch.TriggerMethod[current_channel]==0x04){
      CapTouch.TouchDurationTemp[current_channel]=refernce_time;
      CapTouch.UsingReferenceTimer&=~(1<<current_channel);
    }
  }
  else{
    CapTouch.TouchDuration[current_channel]=0;
    CapTouch.TouchDurationTemp[current_channel]=0;
  }
}


void CapTouch_Scan_Sensors(uint32_t refernce_time){
  #ifdef CAPTOUCH_AUTO_PWR_DOWN
    CapTouch_ADC_Wakeup();
	CapTouch_ADC_Sample(0);
  #endif
  for(uint8_t channel=0;channel<CAPTOUCH_TOTAL_TOUCH_KEY;channel++){
    for(uint8_t i=0; i<CAPTOUCH_CHECK_DUR_SMPL;i++){
      CapTouch_Check_Touch_Duration(channel, refernce_time);
    }
  }
  #ifdef CAPTOUCH_AUTO_PWR_DOWN
    CapTouch_ADC_Sleep();
  #endif
}

uint8_t CapTouch_Short_Pressed(uint8_t current_channel){
  uint8_t sts=0;
  if(CapTouch.TriggerMethod[current_channel]==0x04){
    if((CapTouch.TouchDuration[current_channel]>=CapTouch.MinShortPressTime) && (CapTouch.TouchDuration[current_channel]<=CapTouch.MaxShortPressTime)){
      sts=1;
    }
  }
  return sts;
}

uint8_t CapTouch_Long_Pressed(uint8_t current_channel){
  uint8_t sts=0;
  if((CapTouch.TouchDuration[current_channel]>CapTouch.LongPressTime) && (CapTouch.LongPressFlag[current_channel]==0)){
    CapTouch.LongPressFlag[current_channel]=1;
    sts=1;
  }
  return sts;
}

uint8_t CapTouch_Constant_Pressed(uint8_t current_channel){
  uint8_t sts=0;
  if((CapTouch.TriggerMethod[current_channel]==0x02) && (CapTouch.TouchDuration[current_channel]>CapTouch.MinShortPressTime)){
    sts=1;
  }
  return sts;
}



uint8_t CapTouch_Short_Pressed_Filtered(uint8_t current_channel){
  uint8_t  sts=0, res=1;
  for(uint8_t i=0;i<current_channel;i++){
    if(CapTouch_Get_Touch_Duration(i)!=0){
	  res=0;
	}
  }
  
  if( (res == 1) && CapTouch_Short_Pressed(current_channel) ){
    sts=1;
  }else{
    res=0;
  }
  
  if(res == 1){
    for(uint8_t i=current_channel+1;i<CAPTOUCH_TOTAL_TOUCH_KEY;i++){
      if(CapTouch_Get_Touch_Duration(i)!=0){
	    sts=0;
	  }
	}
  }
  
  return sts;
}

uint8_t CapTouch_Long_Pressed_Filtered(void){
  uint8_t  sts=0, lt_chnl=0, res=0;
  for(uint8_t i=0;i<CAPTOUCH_TOTAL_TOUCH_KEY; i++){
    if(CapTouch_Long_Pressed(i)){
	  lt_chnl=i;
	  sts |= (1<<i);
	  res=1;
    }
  }
  if(res==1){
    for(uint8_t i=0; i<lt_chnl; i++){
      if(CapTouch_Get_Touch_Duration(i)>=(CapTouch.LongPressTime/2)){
	    sts |= (1<<i);
		CapTouch.LongPressFlag[i]=1;
	    break;
	  }
    }
    for(uint8_t i=lt_chnl+1; i<CAPTOUCH_TOTAL_TOUCH_KEY; i++){
      if(CapTouch_Get_Touch_Duration(i)>=(CapTouch.LongPressTime/2)){
	    sts |= (1<<i);
		CapTouch.LongPressFlag[i]=1;
	    break;
	  }
    }
  }
  return sts;
}

void CapTouch_Set_Short_Press_Min_Time(uint8_t val){
  CapTouch.MinShortPressTime=val;
}

void CapTouch_Set_Short_Press_Max_Time(uint8_t val){
  CapTouch.MaxShortPressTime=val;
}

void CapTouch_Set_Long_Press_Time(uint8_t val){
  CapTouch.LongPressTime=val;
}

int CapTouch_Get_RawADC1(uint8_t current_channel){
  return CapTouch.RawADC1[current_channel];
}

int CapTouch_Get_RawADC2(uint8_t current_channel){
  return CapTouch.RawADC2[current_channel];
}

int CapTouch_Get_RawDifferentialADC(uint8_t current_channel){
  return CapTouch.RawDiffADC[current_channel];
}

int CapTouch_Get_Calibration_Value(uint8_t current_channel){
  return CapTouch.CalibrationValue[current_channel];
}

int32_t CapTouch_Get_Touch_Duration(uint8_t current_channel){
  return CapTouch.TouchDuration[current_channel];
}

uint8_t CapTouch_Get_Touch_Response(uint8_t current_channel){
  return CapTouch.Result[current_channel];
}

uint8_t CapTouch_Get_Reference_Timer_Use_Flag(void){
  if(CapTouch.UsingReferenceTimer){
    return 1;
  }else{
    return 0;
  }
}

uint8_t CapTouch_Get_ErrorCode(uint8_t current_channel){
  return CapTouch.ErrorCode[current_channel];
}

void CapTouch_Get_Debug_Data(uint8_t current_channel, int *data_out){
  data_out[0]=8;
  data_out[1]=current_channel;
  data_out[2]=CapTouch_Get_RawADC1(current_channel);
  data_out[3]=CapTouch_Get_RawADC2(current_channel);
  data_out[4]=CapTouch_Get_RawDifferentialADC(current_channel);
  data_out[5]=CapTouch_Get_Calibration_Value(current_channel);
  data_out[6]=CapTouch_Get_Touch_Duration(current_channel);
  data_out[7]=CapTouch_Get_Touch_Response(current_channel);
  data_out[8]=CapTouch_Get_Reference_Timer_Use_Flag();
}

void CapTouch_Init(void){
  CapTouch_Struct_Init();
  #ifdef CAPTOUCH_ENABLE_LPF
    CapTouch_IIR_LPF_Struct_Init();
  #endif
  CapTouch_ADC_Register_Config();
  CapTouch_Calibration();
}

