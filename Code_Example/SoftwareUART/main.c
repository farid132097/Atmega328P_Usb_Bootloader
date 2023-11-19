#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "debug.h"

#define  OnDelay  10
#define  OffDelay 1000

int main(void){
  
  debug_init();
  DDRD  |= (1<<4);
  PORTD &=~(1<<4);
  sei();
  
  
  while(1){
    
    //PORTD |= (1<<4);
	//debug_tx_text_nl("Hello World! -> Software Serial test");
	//PORTD &=~(1<<4);
	if(debug_rx_byte()!=0){
	  debug_tx_byte('a');
	  debug_rx_byte_clear();
	}
	//_delay_ms(OffDelay);
	
  }
}