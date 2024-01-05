#include <avr/io.h>
#include <util/delay.h>


int main(void){

  DDRD  |= (1<<4);
  PORTD &=~(1<<4);
  
  
  
  while(1){
    PORTD |= (1<<4);
	_delay_ms(10);
	PORTD &=~(1<<4);
	_delay_ms(500);
  }
}