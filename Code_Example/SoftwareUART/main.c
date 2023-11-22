#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "debug.h"

#define  OnDelay  10
#define  OffDelay 1000

int main(void){
  
  debug_init();
  sei();
  
  
  while(1){
    
	if( debug_data_available_rx_buf() ){
	  for(uint8_t i=0;i<debug_available_bytes_rx_buf();i++){
	    debug_tx_byte(debug_buf_get(i));
	  }
	  debug_flush_buf();
	}
	_delay_us(100);
	
	
  }
}