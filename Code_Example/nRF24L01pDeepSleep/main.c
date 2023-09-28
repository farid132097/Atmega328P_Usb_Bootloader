#include <avr/io.h>
#include <util/delay.h>
#include "lowpower.h"
#include "nrf24l01pv2.h"


uint32_t last_val=0;
uint8_t  state=0;


int main(void){

  DDRD  |= (1<<4);
  PORTD &=~(1<<4);
  
  DDRD&=~((1<<3)|(1<<2));
  PORTD|=(1<<3)|(1<<2);
  
  nRF24L01P_Init();
  Sleep_Init();
  
  while(1){
      
	  if(WDT_Get_Ticks()<10){
	    uint8_t buf[32];
	    PORTD |=(1<<4);
	    nRF24L01P_WakeUp();
	    nRF24L01P_Transmit_Basic(buf,10);
	    nRF24L01P_Deep_Sleep();
	  }else{
	    PORTD &=~(1<<4);
	    Deep_Sleep();
	  }
	
  }
}