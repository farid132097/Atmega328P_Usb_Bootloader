#include <avr/io.h>
#include <util/delay.h>
#include "captouch.h"
#include "debug.h"

int main(void){

  debug_init();
  CapTouch_Init();
  
  uint32_t ticks=0;
  while(1){
    
    CapTouch_Scan_Sensors(ticks);
	ticks++;
	
	uint8_t sts=CapTouch_Long_Pressed_Filtered();
	
	if(sts==0b01){
	  debug_tx_text_nl("LP BT 0");
	}else if(sts==0b10){
	  debug_tx_text_nl("LP BT 1");
	}else if(sts==0b11){
	  debug_tx_text_nl("LP BT 1 & 0");
	}
	
	
	_delay_ms(20);
    
  }
}