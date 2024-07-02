#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "dataconv.h"
#include "debug.h"
#include "led.h"
#include "spi.h"
#include "isp.h"

int main(void){
  
  uint8_t success = 0, path_clear = 1;
  debug_init();
  debug_tx_text_nl("Debug Started");
  debug_tx_text("Reset Source : ");
  debug_tx_number_hex_nl(MCUSR);
  debug_tx_nl();
  ISP_Init();
  LED_Init();
  
  if(ISP_Enter_Programming_Mode()){
    debug_tx_text_nl("Programming Mode Enabled");
	ISP_Read_Device_Parameters();
	debug_tx_text("Target State : ");
	debug_tx_number_hex_nl(ISP_Get_Device_Signature());
	debug_tx_text("Fuse Low     : ");
	debug_tx_number_hex_nl(ISP_Get_Fuse_Low_Byte());
	debug_tx_text("Fuse High    : ");
	debug_tx_number_hex_nl(ISP_Get_Fuse_High_Byte());
	debug_tx_text("Fuse Ext     : ");
	debug_tx_number_hex_nl(ISP_Get_Fuse_Extended_Byte());
	debug_tx_text("Fuse Lock    : ");
	debug_tx_number_hex_nl(ISP_Get_Lock_Byte());
	debug_tx_nl();
	
	debug_tx_text_nl("Target State : Writing Fuse");
	if(ISP_Write_Verify_Device_LFUSE(0xE2)){
	  debug_tx_text_nl("Target State : LFUSE Set 0xE2");
	}else{
	  debug_tx_text_nl("Target State : LFUSE Write Failed");
	  path_clear = 0;
	}
	
	if(ISP_Write_Verify_Device_HFUSE(0xD0)){
	  debug_tx_text_nl("Target State : HFUSE Set 0xD0");
	}else{
	  debug_tx_text_nl("Target State : HFUSE Write Failed");
	  path_clear = 0;
	}
	
	
	if(ISP_Enter_Programming_Mode() && (path_clear == 1) ){
	  ISP_Read_Device_Parameters();
	  if(ISP_Get_Device_Signature()==0x001E950F){
	    debug_tx_text_nl("Target State : Chip Erasing");
	    ISP_Chip_Erase();
		debug_tx_text_nl("Target State : Chip Erased");
		debug_tx_text_nl("Target State : Writing Flash");
	    ISP_Write_Auto();
		debug_tx_text_nl("Target State : Flash Write Completed");
		
		debug_tx_text_nl("Target State : Verifying Flash");
		if(ISP_Verify_Flash()){
		  debug_tx_text_nl("Target State : Flash Verified");
		}else{
		  debug_tx_text_nl("Target State : Flash Verification Failed");
		  path_clear = 0;
		}
		
		if(path_clear == 1){
		  if(ISP_Write_Verify_Device_LFUSE(0xFF)){
	        debug_tx_text_nl("Target State : LFUSE Set 0xFF");
	      }else{
	        debug_tx_text_nl("Target State : LFUSE Write Failed");
	        path_clear = 0;
	      }
		}
		
		if(path_clear == 1){
		  if(ISP_Write_Verify_Device_HFUSE(0xD0)){
	        debug_tx_text_nl("Target State : HFUSE Set 0xD0");
	      }else{
	        debug_tx_text_nl("Target State : HFUSE Write Failed");
	        path_clear = 0;
	      }
		}
		
		if(path_clear == 1){
		  if(ISP_Write_Verify_Device_LBITS(0xCF)){
	        debug_tx_text_nl("Target State : LBITS Set 0xCF");
	      }else{
	        debug_tx_text_nl("Target State : LBITS Write Failed");
	        path_clear = 0;
	      }
		}
		
		if(path_clear == 1){
		  success=1;
		  debug_tx_text_nl("Target State : Write Successful");
		  debug_tx_text_nl("Target State : Exiting Programming Mode");
		}
	  }
	}
  }else{
    debug_tx_text_nl("Programming Mode Enter Failed");
	success=0;
  }
  debug_tx_nl();
  debug_tx_nl();
  
  ISP_Reset_Disable();
  
  while(1){
    
	LED_Toggle();
	if(success){
	  _delay_ms(500);
	}else{
	  _delay_ms(100);
	}
    
  }
}