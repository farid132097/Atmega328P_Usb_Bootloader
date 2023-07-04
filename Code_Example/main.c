#include <avr/io.h>
#include <util/delay.h>

#define  Delay 1000 


int main(void){

  DDRD  |= (1<<4);
  PORTD &=~(1<<4);

  while(1){
    PORTD |= (1<<4);
	_delay_ms(Delay);
	PORTD &=~(1<<4);
	_delay_ms(Delay);
  }
}