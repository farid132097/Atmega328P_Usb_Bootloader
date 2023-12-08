#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "lowpower.h"
#include "nrf24l01p.h"
#include "dataconv.h"
#include "eeprom.h"
#include "vsns.h"
#include "led.h"


int main(void){

  DDRD  |= (1<<4);
  PORTD &=~(1<<4);
  
  DDRD&=~((1<<3)|(1<<2));
  PORTD|=(1<<3)|(1<<2);
  
  nrf24l01p_init();
  Sleep_Init();
  Vsns_Init();
  led_init();
  
  while(1){
	    
	    led_on();
		uint8_t buf[32];
		uint16_t val=Vsns_V_Read();
		dataconv_dword_to_num(val, buf, 0);
		buf[dataconv_dword_to_num_len()]='\r';
		buf[dataconv_dword_to_num_len()+1]='\n';
	    nrf24l01p_tx_basic(buf,dataconv_dword_to_num_len()+2);
	    nrf24l01p_mode_set(0);
	    led_off();
	    Deep_Sleep();
		
    }
}