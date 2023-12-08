
#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t EEPROM_Read(uint16_t addr){
  cli();
  EEAR=addr;
  EECR|=(1<<EERE);
  while(EECR & (1<<EEPE));
  sei();
  return EEDR;
}

void EEPROM_Write(uint16_t addr, uint8_t data){
  if(EEPROM_Read(addr)!=data){
    cli();
    EEAR=addr;
    EEDR=data;
    EECR=(1<<EEMPE);
    EECR|=(1<<EEPE);
    while(EECR & (1<<EEPE));
    sei();
  }
}

void EEPROM_Erase(uint16_t addr){
  EEPROM_Write(addr, 0xFF);
}


uint16_t EEPROM_Read_Word(uint16_t addr){
  uint16_t temp=EEPROM_Read(addr);
  temp<<=8;
  temp|=EEPROM_Read(addr+1);
  return temp;
}

void EEPROM_Write_Word(uint16_t addr, uint16_t data){
  EEPROM_Write( addr, (data>>8) & 0xFF );
  EEPROM_Write( addr+1, data & 0xFF );
}

uint32_t EEPROM_Read_Dword(uint16_t addr){
  uint32_t temp=EEPROM_Read_Word( addr );
  temp<<=16;
  temp|=EEPROM_Read_Word( addr+2 );
  return temp;
}

void EEPROM_Write_Dword(uint16_t addr, uint32_t data){
  EEPROM_Write_Word( addr, (data>>16) & 0xFFFF);
  EEPROM_Write_Word( addr+2, (data & 0xFFFF));
}

void EEPROM_Erase_Region(uint16_t start_addr, uint16_t stop_addr){
  for(uint16_t i=start_addr;i<=stop_addr;i++){
	EEPROM_Erase( i );
  }
}

void EEPROM_Clear_Data_Space(void){
  if( EEPROM_Read_Dword(1020) != 0x01020304 ){
    EEPROM_Erase_Region(1018,1019);
	EEPROM_Write_Dword(1020, 0x01020304);
  }
}

void EEPROM_Init(void){
  EEPROM_Clear_Data_Space();
}
  
  
  
  
  
  
  