

#include <avr/io.h>
#include <util/delay.h>
#include "spi.h"

typedef struct spi_t{
  uint8_t  Error;
  uint8_t  Ticks;
  uint8_t  Mode ;
  uint16_t CounterVal;
  uint16_t Delay[8];
  uint8_t  InitClock[4];
}spi_t;

spi_t SPI;

void SPI_Struct_Init(void){
  SPI.Error=0;
  SPI.Ticks=0;
  SPI.Mode =0;
  SPI.CounterVal=0;
  SPI.Delay[0]=16;
  SPI.Delay[1]=32;
  SPI.Delay[2]=64;
  SPI.Delay[3]=128;
  SPI.Delay[4]=256;
  SPI.Delay[5]=512;
  SPI.Delay[6]=1024;
  SPI.Delay[7]=2048;
  SPI.InitClock[0]=4;  //very fast =375khz
  SPI.InitClock[1]=6;  //fast = 93.75 khz
  SPI.InitClock[2]=9;  //slow = 11.72 khz
  SPI.InitClock[3]=11; //very slow = 2.93 khz
}

void SPI_SCK_Input(void){
  SCK_DDR  &=~(1<<SCK_bp);
}

void SPI_SCK_Output(void){
  SCK_DDR  |= (1<<SCK_bp);
}

void SPI_SCK_Set_Output_State(uint8_t state){
  if(state == 1){
    SCK_PORT |= (1<<SCK_bp);
  }else if(state == 0){
    SCK_PORT &=~(1<<SCK_bp);
  }
}

void SPI_MISO_Input(void){
  MISO_DDR &=~(1<<MISO_bp);
}

void SPI_MISO_Output(void){
  MISO_DDR |= (1<<MISO_bp);
}

void SPI_MISO_Set_Output_State(uint8_t state){
  if(state == 1){
    MISO_PORT |= (1<<MISO_bp);
  }else if(state == 0){
    MISO_PORT &=~(1<<MISO_bp);
  }
}

uint8_t SPI_MISO_Get_State(void){
  if(MISO_PIN & (1<<MISO_bp)){
    return 1;
  }else{
    return 0;
  }
}

void SPI_MOSI_Input(void){
  MOSI_DDR &=~(1<<MOSI_bp);
}

void SPI_MOSI_Output(void){
  MOSI_DDR |= (1<<MOSI_bp);
}

void SPI_MOSI_Set_Output_State(uint8_t state){
  if(state == 1){
    MOSI_PORT |= (1<<MOSI_bp);
  }else if(state == 0){
    MOSI_PORT &=~(1<<MOSI_bp);
  }
}

void SPI_RESET_Input(void){
  RESET_DDR&=~(1<<RESET_bp);
}

void SPI_RESET_Output(void){
  RESET_DDR|= (1<<RESET_bp);
}

void SPI_RESET_Set_Output_State(uint8_t state){
  if(state == 1){
    RESET_PORT |= (1<<RESET_bp);
  }else if(state == 0){
    RESET_PORT &=~(1<<RESET_bp);
  }
}

void SPI_Reset_Set_Input_Pull_Up(void){
  SPI_RESET_Input();
  SPI_RESET_Set_Output_State(1);
}

void SPI_Reset_State(void){
  SPI_RESET_Output();
  SPI_RESET_Set_Output_State(0);
  SPI_SCK_Input();
  SPI_MISO_Input();
  SPI_MOSI_Input();
}

void SPI_GPIO_Init(void){
  SPI_RESET_Output();
  SPI_RESET_Set_Output_State(0);
  _delay_us(500);
  SPI_SCK_Output();
  SPI_MISO_Input();
  SPI_MOSI_Output();
  SPI_SCK_Set_Output_State(0);
  SPI_MOSI_Set_Output_State(1);
}

void SPI_Delay_Init(void){
  TCCR1A=0;
  TCCR1B=0;
  TCCR1C=0;
  TIMSK1=0;
  TIFR1 =0;
  TCCR1B=(1<<CS11);
}

void SPI_Set_SCK_Frequency_HW(uint8_t sck_step){
  SPCR = 0x00;
  SPSR = 0x00;
  SPDR = 0x00;
  if(sck_step == 0){           //FOSC/2 = 6MHZ
    SPCR &=~(1<<SPR0);
	SPCR &=~(1<<SPR1);
	SPSR |= (1<<SPI2X);
	SPI.Mode =0;
	SPI.CounterVal=0;
  }else if(sck_step == 1){     //FOSC/4 = 3MHZ
    SPCR &=~(1<<SPR0);
	SPCR &=~(1<<SPR1);
	SPSR &=~(1<<SPI2X);
	SPI.Mode =0;
	SPI.CounterVal=0;
  }else if(sck_step == 2){     //FOSC/8 = 1.5MHZ
    SPCR |= (1<<SPR0);
	SPCR &=~(1<<SPR1);
	SPSR |= (1<<SPI2X);
	SPI.Mode =0;
	SPI.CounterVal=0;
  }else if(sck_step == 3){     //FOSC/16 = 750kHZ
    SPCR |= (1<<SPR0);
	SPCR &=~(1<<SPR1);
	SPSR &=~(1<<SPI2X);
	SPI.Mode =0;
	SPI.CounterVal=0;
  }else if(sck_step == 4){     //FOSC/32 = 375kHZ
    SPCR &=~(1<<SPR0);
	SPCR |= (1<<SPR1);
	SPSR |= (1<<SPI2X);
	SPI.Mode =0;
	SPI.CounterVal=0;
  }else if(sck_step == 5){     //FOSC/64 = 187.5kHZ
    SPCR |= (1<<SPR0);
	SPCR |= (1<<SPR1);
	SPSR |= (1<<SPI2X);
	SPI.Mode =0;
	SPI.CounterVal=0;
  }else if(sck_step == 6){     //FOSC/128 = 93.75kHZ
    SPCR |= (1<<SPR0);
	SPCR |= (1<<SPR1);
	SPSR &=~(1<<SPI2X);
	SPI.Mode =0;
	SPI.CounterVal=0;
  }else{                       //FOSC/128 = 93.75kHZ
    SPCR |= (1<<SPR0);
	SPCR |= (1<<SPR1);
	SPSR &=~(1<<SPI2X);
	SPI.Mode =0;
	SPI.CounterVal=0;
  }
  SPCR|=(1<<SPE)|(1<<MSTR);
}

//add software spi functions

void SPI_Set_SCK_Frequency_SW(uint8_t sck_step){
  SPCR = 0x00;
  SPSR = 0x00;
  SPDR = 0x00;
  if(sck_step == 7){             //FOSC/256 = 46.88kHZ
    SPI.Mode =1;
    SPI.CounterVal=SPI.Delay[0];
  }else if (sck_step == 8){      //FOSC/512 = 23.44kHZ
    SPI.Mode =1;
    SPI.CounterVal=SPI.Delay[1];
  }else if (sck_step == 9){      //FOSC/1024 = 11.72kHZ
    SPI.Mode =1;
    SPI.CounterVal=SPI.Delay[2];
  }else if (sck_step == 10){     //FOSC/2048 = 5.86kHZ
    SPI.Mode =1;
    SPI.CounterVal=SPI.Delay[3];
  }else if (sck_step == 11){     //FOSC/4096 = 2.93kHZ
    SPI.Mode =1;
    SPI.CounterVal=SPI.Delay[4];
  }else{                         //FOSC/4096 = 2.93kHZ
    SPI.Mode =1;
    SPI.CounterVal=SPI.Delay[4];
  }
}

void SPI_Config_Register(uint8_t sck_step){
  if( (sck_step>=0) && (sck_step<=6) ){
    SPI_Set_SCK_Frequency_HW(sck_step);
  }else{
    SPI_Set_SCK_Frequency_SW(sck_step);
  }
}

void SPI_SW_Delay_Bit(void){
  TCNT1=0;
  while(TCNT1 < SPI.CounterVal){}
}

void SPI_SW_Delay_Half_Bit(void){
  TCNT1=0;
  uint16_t hbit=SPI.CounterVal/2;
  while(TCNT1 < hbit){}
}

uint8_t SPI_Transceive_HW(uint8_t data){
  SPDR = data;
  while(!(SPSR & (1 << SPIF))){}
  return SPDR;
}

uint8_t SPI_Transceive_SW(uint8_t data){
  uint8_t rdata=0;
  for(uint8_t i=0;i<8;i++){
    if(data & 0x80){
	  SPI_MOSI_Set_Output_State(1);
	}else{
	  SPI_MOSI_Set_Output_State(0);
	}
	rdata<<=1;
	data <<=1;
	rdata|=SPI_MISO_Get_State();
	SPI_SW_Delay_Half_Bit();
	SPI_SCK_Set_Output_State(1);
	SPI_SW_Delay_Bit();
	SPI_SCK_Set_Output_State(0);
	SPI_SW_Delay_Half_Bit();
  }
  SPI_MOSI_Set_Output_State(1);
  return rdata;
}

uint8_t SPI_Transceive(uint8_t data){
  uint8_t rdata=0;
  if(SPI.Mode){
    rdata = SPI_Transceive_SW(data);
  }else{
    rdata = SPI_Transceive_HW(data);
  }
  return rdata;
}

void SPI_Init(uint8_t sck_step){
  SPI_Struct_Init();
  SPI_GPIO_Init();
  SPI_Delay_Init();
  SPI_Config_Register(sck_step);
}

void SPI_Init_Clock(uint8_t sck_step){
  if( (sck_step>=0) && (sck_step<=3) ){
    SPI_Init(SPI.InitClock[sck_step]);
  }else{ //Slowest
    SPI_Init(11);
  }
}

uint8_t SPI_Get_Error(void){
  return SPI.Error;
}

uint8_t SPI_Get_Ticks(void){
  return SPI.Ticks;
}




