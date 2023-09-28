#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#define  OnDelay  10
#define  OffDelay 1000

uint8_t val[27000] PROGMEM={22};

int main(void){

  DDRD  |= (1<<4);
  PORTD &=~(1<<4);
  
  
  
  while(1){
    PORTD |= (1<<4);
	_delay_ms(OnDelay);
	PORTD &=~(1<<4);
	_delay_ms(OffDelay);
  }
}