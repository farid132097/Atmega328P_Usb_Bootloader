

#include <avr/io.h>
#include <util/delay.h>
#include "led.h"

#define  LED_DDR  DDRD
#define  LED_PORT PORTD
#define  LED_bp   0x04



void led_init(void){
  LED_DDR|=(1<<LED_bp);
  LED_PORT&=~(1<<LED_bp);
}

void led_on(void){
  LED_PORT|=(1<<LED_bp);
}

void led_off(void){
  LED_PORT&=~(1<<LED_bp);
}