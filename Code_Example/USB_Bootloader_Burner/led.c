
#include <avr/io.h>
#include <util/delay.h>
#include "led.h"

#define  LED_DDR  DDRD
#define  LED_PORT PORTD
#define  LED_bp   4U

void LED_Init(void){
  LED_DDR |= (1<<LED_bp);
  LED_PORT&=~(1<<LED_bp);
}

void LED_On(void){
  LED_PORT|= (1<<LED_bp);
}

void LED_Off(void){
  LED_PORT&=~(1<<LED_bp);
}

uint8_t LED_Get_State(void){
  if(LED_PORT & (1<<LED_bp)){
    return 1;
  }else{
    return 0;
  }
}

void LED_Set_State(uint8_t state){
  if(state==1){
    LED_On();
  }else if(state==0){
    LED_Off();
  }
}

void LED_Toggle(void){
  if(LED_Get_State()){
    LED_Set_State(0);
  }else{
    LED_Set_State(1);
  }
}