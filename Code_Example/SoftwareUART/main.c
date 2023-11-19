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
    
	if( (debug_databsy_get()==0) && (debug_buf_index_get()!=0) ){
	  for(uint8_t i=0;i<debug_buf_index_get();i++){
	    debug_tx_byte(debug_buf_get(i));
	  }
	  debug_flush_buf();
	}
	_delay_us(100);
	
	
  }
}