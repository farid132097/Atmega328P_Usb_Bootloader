#include <avr/io.h>
#include <util/delay.h>
#include "lowpower.h"


uint32_t last_val=0;
uint8_t  state=0;


int main(void){

  DDRD  |= (1<<4);
  PORTD &=~(1<<4);
  
  DDRD&=~((1<<3)|(1<<2));
  PORTD|=(1<<3)|(1<<2);
  
  
  Sleep_Init();
  
  while(1){
    
	
	if(WDT_Get_Ticks()<10){
	  PORTD |=(1<<4);
	}else{
	  PORTD &=~(1<<4);
	  Deep_Sleep();
	}
	
  }
}