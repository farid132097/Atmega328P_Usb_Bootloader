

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "firmware.h"
#include "spi.h"
#include "debug.h"


//#define  ISP_DEBUG_ENABLE

#define  ISP_RETRY_CYCELS    0x05
#define  ISP_DEFAULT_LFUSE   0x62
#define  ISP_DEFAULT_HFUSE   0xD9
#define  ISP_DEFAULT_EFUSE   0xFF
#define  ISP_DEFAULT_LBITS   0xFF

#define  ISP_OVERWRITE_LFUSE 0xE2
#define  ISP_OVERWRITE_HFUSE 0xD0
#define  ISP_OVERWRITE_EFUSE 0xFF
#define  ISP_OVERWRITE_LBITS 0xCF

typedef struct isp_t{
  uint32_t DeviceSignature;
  uint8_t  LoopCounter;
  uint8_t  SPIClkStep;
  uint8_t  LFUSE;
  uint8_t  HFUSE;
  uint8_t  EFUSE;
  uint8_t  LBITS;
}isp_t;

uint16_t ISP_Buf[64];

isp_t ISP;


void ISP_Struct_Init(void){
  ISP.DeviceSignature=0;
  ISP.LoopCounter=0;
  ISP.SPIClkStep=0;
  ISP.LFUSE=0;
  ISP.HFUSE=0;
  ISP.EFUSE=0;
  ISP.LBITS=0;
}

void ISP_Flush_Buffer(void){
  for(uint8_t i=0;i<64;i++){
    ISP_Buf[i]=0xFF;
  }
}

void ISP_Init(void){
  ISP_Struct_Init();
  ISP_Flush_Buffer();
}

void ISP_Reset_MCU(void){
  SPI_RESET_Set_Output_State(0);
  SPI_SCK_Set_Output_State(0);
  _delay_us(500);
  SPI_RESET_Set_Output_State(1);
  _delay_us(500);
  SPI_RESET_Set_Output_State(0);
  _delay_ms(30);
}

void ISP_Reset_Disable(void){
  SPI_Reset_State();
  _delay_us(500);
  SPI_RESET_Set_Output_State(1);
  _delay_ms(10);
}

uint8_t ISP_Enter_Programming_Mode(void){
  uint8_t status = 0, retry_cycles=0, loop_counter=0;
  uint8_t clock_state=0;
  
  while(status != 0x53){
	ISP_Struct_Init();
    SPI_Init_Clock(clock_state);
	
	#ifdef ISP_DEBUG_ENABLE
	debug_tx_parameter_nl("SPI_SCK  ", clock_state);
	#endif
	
    ISP_Reset_MCU();
	
	#ifdef ISP_DEBUG_ENABLE
	debug_tx_text_nl("MCU_RST ");
	#endif
	
    SPI_Transceive(0xAC);
    SPI_Transceive(0x53);
    status=SPI_Transceive(0x00);
    SPI_Transceive(0x00);
	
	
	#ifdef ISP_DEBUG_ENABLE
	debug_tx_parameter_nl("SPI_STS  ", status);
	debug_tx_nl();
	#endif
	
    retry_cycles++;
	if( (clock_state <= 3) && (retry_cycles >= ISP_RETRY_CYCELS) ){
	  clock_state++;
	  retry_cycles=0;
	}
	ISP.SPIClkStep=clock_state;
	
	loop_counter++;
	if( loop_counter > (ISP_RETRY_CYCELS*4) ){
	  ISP.LoopCounter = 0;
	  status = 0;
	  break;
	}
	ISP.LoopCounter = loop_counter;
   }
   
   if(status==0x53){
     return 1;
   }else{
     return 0;
   }
}

uint32_t ISP_Read_Device_Signature(void){
  uint32_t signature_bytes[3]={0,0,0};
  for(uint8_t i=0;i<3;i++){
    SPI_Transceive(0x30);
    SPI_Transceive(0x00);
    SPI_Transceive(i);
    signature_bytes[i]=SPI_Transceive(0x00);
  }
  ISP.DeviceSignature   = signature_bytes[0];
  ISP.DeviceSignature <<= 8;
  ISP.DeviceSignature  |= signature_bytes[1];
  ISP.DeviceSignature <<= 8;
  ISP.DeviceSignature  |= signature_bytes[2];
  return ISP.DeviceSignature;
}

uint8_t ISP_Read_MCU_Busy(void){
  SPI_Transceive(0xF0);
  SPI_Transceive(0x00);
  SPI_Transceive(0x00);
  uint8_t sts=SPI_Transceive(0x00);
  sts&=1;
  return sts;
}

void ISP_Wait_Till_MCU_Busy(void){
  while(ISP_Read_MCU_Busy()){
    _delay_us(100);
  }
}

uint8_t ISP_Read_Device_LFUSE(void){
  ISP_Wait_Till_MCU_Busy();
  ISP.LFUSE=0x00;
  SPI_Transceive(0x50);
  SPI_Transceive(0x00);
  SPI_Transceive(0x00);
  ISP.LFUSE=SPI_Transceive(0x00);
  return ISP.LFUSE;
}

uint8_t ISP_Read_Device_HFUSE(void){
  ISP_Wait_Till_MCU_Busy();
  ISP.HFUSE=0x00;
  SPI_Transceive(0x58);
  SPI_Transceive(0x08);
  SPI_Transceive(0x00);
  ISP.HFUSE=SPI_Transceive(0x00);
  return ISP.HFUSE;
}

uint8_t ISP_Read_Device_EFUSE(void){
  ISP_Wait_Till_MCU_Busy();
  ISP.EFUSE=0x00;
  SPI_Transceive(0x50);
  SPI_Transceive(0x08);
  SPI_Transceive(0x00);
  ISP.EFUSE=SPI_Transceive(0x00);
  return ISP.EFUSE;
}

uint8_t ISP_Read_Device_LBITS(void){
  ISP_Wait_Till_MCU_Busy();
  ISP.LBITS=0x00;
  SPI_Transceive(0x58);
  SPI_Transceive(0x00);
  SPI_Transceive(0x00);
  ISP.LBITS=SPI_Transceive(0x00);
  return ISP.LBITS;
}


void ISP_Read_Device_Parameters(void){
  ISP_Read_Device_Signature();
  ISP_Read_Device_LFUSE();
  ISP_Read_Device_HFUSE();
  ISP_Read_Device_EFUSE();
  ISP_Read_Device_LBITS();
}


void ISP_Write_Device_LFUSE(uint8_t val){
  SPI_Transceive(0xAC);
  SPI_Transceive(0xA0);
  SPI_Transceive(0x00);
  SPI_Transceive(val) ;
  ISP_Wait_Till_MCU_Busy();
}

void ISP_Write_Device_HFUSE(uint8_t val){
  SPI_Transceive(0xAC);
  SPI_Transceive(0xA8);
  SPI_Transceive(0x00);
  SPI_Transceive(val) ;
  ISP_Wait_Till_MCU_Busy();
}

void ISP_Write_Device_EFUSE(uint8_t val){
  SPI_Transceive(0xAC);
  SPI_Transceive(0xA4);
  SPI_Transceive(0x00);
  SPI_Transceive(val) ;
  ISP_Wait_Till_MCU_Busy();
}

void ISP_Write_Device_LBITS(uint8_t val){
  SPI_Transceive(0xAC);
  SPI_Transceive(0xE0);
  SPI_Transceive(0x00);
  SPI_Transceive(val) ;
  ISP_Wait_Till_MCU_Busy();
}

uint8_t ISP_Write_Verify_Device_LFUSE(uint8_t val){
  ISP_Wait_Till_MCU_Busy();
  ISP_Write_Device_LFUSE(val);
  if(ISP_Read_Device_LFUSE()==val){
    return 1;
  }else{
    return 0;
  }
}

uint8_t ISP_Write_Verify_Device_HFUSE(uint8_t val){
  ISP_Wait_Till_MCU_Busy();
  ISP_Write_Device_HFUSE(val);
  if(ISP_Read_Device_HFUSE()==val){
    return 1;
  }else{
    return 0;
  }
}

uint8_t ISP_Write_Verify_Device_EFUSE(uint8_t val){
  ISP_Wait_Till_MCU_Busy();
  ISP_Write_Device_EFUSE(val);
  if(ISP_Read_Device_EFUSE()==val){
    return 1;
  }else{
    return 0;
  }
}

uint8_t ISP_Write_Verify_Device_LBITS(uint8_t val){
  ISP_Wait_Till_MCU_Busy();
  ISP_Write_Device_LBITS(val);
  if(ISP_Read_Device_LBITS()==val){
    return 1;
  }else{
    return 0;
  }
}

void ISP_Chip_Erase(void){
  ISP_Wait_Till_MCU_Busy();
  SPI_Transceive(0xAC);
  SPI_Transceive(0x80);
  SPI_Transceive(0x00);
  SPI_Transceive(0x00);
  ISP_Wait_Till_MCU_Busy();
}

void ISP_Write_EEPROM(uint16_t addr, uint8_t data){
  ISP_Wait_Till_MCU_Busy();
  SPI_Transceive(0xC0);
  SPI_Transceive((addr>>8) & 0x03);
  SPI_Transceive(addr & 0xFF);
  SPI_Transceive(data);
  ISP_Wait_Till_MCU_Busy();
}

uint8_t ISP_Read_EEPROM(uint16_t addr){
  ISP_Wait_Till_MCU_Busy();
  SPI_Transceive(0xA0);
  SPI_Transceive((addr>>8) & 0x03);
  SPI_Transceive(addr & 0xFF);
  uint8_t data = SPI_Transceive(0x00);
  ISP_Wait_Till_MCU_Busy();
  return data;
}

void ISP_Load_PGM_Word(uint8_t word_addr, uint16_t data){
  ISP_Wait_Till_MCU_Busy();
  SPI_Transceive(0x40); //low byte
  SPI_Transceive(0x00);
  SPI_Transceive(word_addr & 0x3F);
  SPI_Transceive((data>>8) & 0xFF);
  ISP_Wait_Till_MCU_Busy();
  SPI_Transceive(0x48); //high byte
  SPI_Transceive(0x00);
  SPI_Transceive(word_addr & 0x3F);
  SPI_Transceive(data & 0xFF);
  ISP_Wait_Till_MCU_Busy();
}

void ISP_Load_PGM_Page(uint16_t *data){
  for(uint16_t i=0;i<64;i++){
    ISP_Load_PGM_Word( i, data[i] );
  }
}

void ISP_Write_PGM_Page(uint8_t page, uint16_t *data){
  ISP_Load_PGM_Page(data);
  SPI_Transceive(0x4C);
  SPI_Transceive(page>>2);
  SPI_Transceive((page & 0x03)<<6);
  SPI_Transceive(0x00);
  _delay_ms(5);
  ISP_Wait_Till_MCU_Busy();
}

uint16_t ISP_Read_Progmem(uint16_t addr){
  return pgm_read_word(&(firmware_data[addr]));
}

void ISP_Load_Buffer_With_Offset(uint16_t offset){
  for(uint16_t pwords=0;pwords<64;pwords++){
    ISP_Buf[pwords]=ISP_Read_Progmem(pwords+offset);
  }
}

void ISP_Write_Auto(void){
  uint16_t start_addr = ISP_Read_Progmem(0);
  start_addr/=128;
  uint16_t incoming_pages=ISP_Read_Progmem(5);
  for(uint16_t i=0;i<incoming_pages;i++){
    ISP_Load_Buffer_With_Offset(16+(i*64));
	ISP_Write_PGM_Page(start_addr+i, ISP_Buf);
  }
}
  

uint16_t ISP_Read_PGM(uint16_t addr){
  ISP_Wait_Till_MCU_Busy();
  SPI_Transceive(0x28);
  SPI_Transceive((addr>>8) & 0xFF);
  SPI_Transceive(addr & 0xFF);
  uint8_t h_data = SPI_Transceive(0x00);
  ISP_Wait_Till_MCU_Busy();
  SPI_Transceive(0x20);
  SPI_Transceive((addr>>8) & 0xFF);
  SPI_Transceive(addr & 0xFF);
  uint8_t l_data = SPI_Transceive(0x00);
  ISP_Wait_Till_MCU_Busy();
  uint16_t data=h_data;
  data<<=8;
  data|=l_data;
  return data;
}

uint16_t ISP_Read_Flash_Word(uint16_t addr){
  uint16_t temp = ISP_Read_PGM(addr);
  uint16_t data = (temp & 0xFF);
  data <<= 8;
  data |= (temp >> 8);
  return data;
}

uint8_t ISP_Verify_Flash(void){
  uint8_t sts = 1;
  uint16_t start_addr = ISP_Read_Progmem(0);
  start_addr/=2;
  uint16_t word_cnt=ISP_Read_Progmem(3);

  for(uint16_t i=0; i<word_cnt; i++){
    if(ISP_Read_Flash_Word(start_addr+i) != ISP_Read_Progmem(i+16)){
	  sts=0;
	  break;
	}
  }
  return sts;
}
  
uint16_t ISP_Get_Data_Buffer(uint16_t addr){
  return ISP_Buf[addr];
}

uint8_t ISP_Get_Device_SCK_Setting(void){
  return ISP.SPIClkStep;
}

uint8_t ISP_Get_Device_Loop_Counter(void){
  return ISP.LoopCounter;
}

uint32_t ISP_Get_Device_Signature(void){
  return ISP.DeviceSignature;
}

uint8_t ISP_Get_Fuse_Low_Byte(void){
  return ISP.LFUSE;
}

uint8_t ISP_Get_Fuse_High_Byte(void){
  return ISP.HFUSE;
}

uint8_t ISP_Get_Fuse_Extended_Byte(void){
  return ISP.EFUSE;
}

uint8_t ISP_Get_Lock_Byte(void){
  return ISP.LBITS;
}

uint8_t ISP_Overwrite_LFUSE(void){
  uint8_t sts=0;
  if(ISP_Read_Device_LFUSE()==ISP_DEFAULT_LFUSE){
    if(ISP_Write_Verify_Device_LFUSE(ISP_OVERWRITE_LFUSE)){
	  sts=1;
	}
  }
  return sts;
}

uint8_t ISP_Overwrite_HFUSE(void){
  uint8_t sts=0;
  if(ISP_Read_Device_HFUSE()==ISP_DEFAULT_HFUSE){
    if(ISP_Write_Verify_Device_HFUSE(ISP_OVERWRITE_HFUSE)){
	  sts=1;
	}
  }
  return sts;
}

uint8_t ISP_Overwrite_EFUSE(void){
  uint8_t sts=0;
  if(ISP_Read_Device_EFUSE()==ISP_DEFAULT_EFUSE){
    if(ISP_Write_Verify_Device_HFUSE(ISP_OVERWRITE_EFUSE)){
	  sts=1;
	}
  }
  return sts;
}

uint8_t ISP_Overwrite_LBITS(void){
  uint8_t sts=0;
  if(ISP_Read_Device_LBITS()==ISP_DEFAULT_LBITS){
    if(ISP_Write_Verify_Device_LBITS(ISP_OVERWRITE_LBITS)){
	  sts=1;
	}
  }
  return sts;
}

