#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lowpower.h"


uint32_t last_val=0;
uint8_t  state=0;


int main(void){

  DDRD  |= (1<<4);
  PORTD &=~(1<<4);
  
  DDRD&=~((1<<3)|(1<<2));
  PORTD|=(1<<3)|(1<<2);
  
  
  Sleep_Init();
  
  //12Mhz
  cli();
  CLKPR = (1<<CLKPCE);
  CLKPR = 0;
  sei();
  while(WDT_Get_Ticks()<5);
  
  //6Mhz
  cli();
  CLKPR = (1<<CLKPCE);
  CLKPR = 1;
  sei();
  while(WDT_Get_Ticks()<10);
  
  //3Mhz
  cli();
  CLKPR = (1<<CLKPCE);
  CLKPR = 2;
  sei();
  while(WDT_Get_Ticks()<15);
  
  //1.5Mhz
  cli();
  CLKPR = (1<<CLKPCE);
  CLKPR = 3;
  sei();
  while(WDT_Get_Ticks()<20);
  
  //750khz
  cli();
  CLKPR = (1<<CLKPCE);
  CLKPR = 4;
  sei();
  while(WDT_Get_Ticks()<25);
  
  //375khz
  cli();
  CLKPR = (1<<CLKPCE);
  CLKPR = 5;
  sei();
  while(WDT_Get_Ticks()<30);
  
  //375khz
  cli();
  CLKPR = (1<<CLKPCE);
  CLKPR = 6;
  sei();
  while(WDT_Get_Ticks()<35);
  
  //187.5khz
  cli();
  CLKPR = (1<<CLKPCE);
  CLKPR = 7;
  sei();
  while(WDT_Get_Ticks()<40);
  
  //93.75khz
  cli();
  CLKPR = (1<<CLKPCE);
  CLKPR = 8;
  sei();
  while(WDT_Get_Ticks()<45);
  
  //12Mhz
  cli();
  CLKPR = (1<<CLKPCE);
  CLKPR = 0;
  sei();
  while(WDT_Get_Ticks()<50);
  
  //Power-Down + WDT Enabled
  PORTD &=~(1<<4);
  while(WDT_Get_Ticks()<60){
    Deep_Sleep();
  }
  
  //Power-Down + WDT Disabled
  cli();
  MCUSR &=~(1<<WDRF);
  WDTCSR|= (1<<WDCE)|(1<<WDE);
  WDTCSR = 0;
  sei();
  Deep_Sleep();
  
  while(1){
    
	
	/*if(WDT_Get_Ticks()<10){
	  PORTD |=(1<<4);
	}else{
	  PORTD &=~(1<<4);
	  Deep_Sleep();
	}*/
	
	
  }
}