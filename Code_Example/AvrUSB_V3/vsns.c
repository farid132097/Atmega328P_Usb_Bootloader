

#include <avr/io.h>
#include <util/delay.h>
#include "vsns.h"

#define  VSNS_EN_DDR  DDRC
#define  VSNS_EN_PORT PORTD
#define  VSNS_EN_bp   0x01


void Vsns_ADC_Reference_Internal(void){
  ADMUX|=(1<<REFS0)|(1<<REFS1);
}

void Vsns_ADC_Reference_Avcc(void){
  ADMUX&=~(1<<REFS1);
  ADMUX|=(1<<REFS0);
}

void Vsns_ADC_Enable(void){
  ADCSRA|=(1<<ADEN); 
}

void Vsns_ADC_Disable(void){
  ADCSRA&=~(1<<ADEN); 
}

void Vsns_ADC_Init(void){
  ADCSRA =(1<<ADPS1)|(1<<ADPS2);
  ADCSRA|=(1<<ADEN);
}

uint16_t Vsns_ADC_Read(uint8_t channel){
  uint8_t admux=(ADMUX & 0xC0);
  admux|=channel;
  if(ADMUX!=admux){ADMUX=admux;}
  ADCSRA|=(1<<ADSC);
  while(!(ADCSRA &(1<<ADIF)));
  ADCSRA|=(1<<ADIF);
  return ADCW;
}



void Vsns_PMOS_Enable(void){
  VSNS_EN_PORT|=(1<<VSNS_EN_bp);
}

void Vsns_PMOS_Disable(void){
  VSNS_EN_PORT&=~(1<<VSNS_EN_bp);
}

uint16_t Vsns_V_Read(void){
  Vsns_PMOS_Enable();
  Vsns_ADC_Enable();
  Vsns_ADC_Read(0x06);
  Vsns_ADC_Read(0x06);
  uint32_t temp=Vsns_ADC_Read(0x06);
  Vsns_ADC_Disable();
  Vsns_PMOS_Disable();
  temp*=16;
  temp*=1100;
  temp/=1024;
  return (uint16_t)temp;
}

void Vsns_Init(void){
  VSNS_EN_DDR |= (1<<VSNS_EN_bp);
  VSNS_EN_PORT&=~(1<<VSNS_EN_bp);
  Vsns_ADC_Reference_Internal();
}