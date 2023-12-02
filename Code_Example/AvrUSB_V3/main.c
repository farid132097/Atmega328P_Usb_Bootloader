#include <avr/io.h>
#include <util/delay.h>
#include "lowpower.h"
#include "nrf24l01pv2.h"
#include "vsns.h"

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
      
	    uint8_t buf[32]="Bello world\r\n";
	    PORTD |=(1<<4);
	    nRF24L01P_WakeUp();
	    nRF24L01P_Transmit_Basic(buf,13);
	    nRF24L01P_Deep_Sleep();
	    PORTD &=~(1<<4);
	    Deep_Sleep();
    }
}