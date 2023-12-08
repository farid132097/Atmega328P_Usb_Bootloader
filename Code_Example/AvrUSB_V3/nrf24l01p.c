

#include <avr/io.h>
#include <util/delay.h>
#include "nrf24l01p.h"


typedef struct nrf24l01p_sp_alloc_t{
  uint8_t              crcmsb        ;
  uint8_t              crclsb        ;
  uint8_t              len           ;
  uint8_t              dest          ;
  uint8_t              own           ;
  uint8_t              ack           ;
  uint8_t              pid           ;
  uint8_t              crclen        ;
  uint8_t              blocks        ;
} nrf24l01p_sp_alloc_t;

typedef struct nrf24l01p_address_t{
  uint8_t              own           ;
  uint8_t              dest          ;
} nrf24l01p_address_t;

typedef struct nrf24l01p_config_t{
  uint16_t             rxtimeout     ;
  uint16_t             rxticks       ;
  uint8_t              maxdatalen    ;
  uint16_t             maxretry      ;
  uint16_t             retryoccured  ;
} nrf24l01p_config_t;

typedef struct nrf24l01p_packet_t{
  uint8_t              txpid         ;
  uint8_t              rxpid         ;
  uint8_t              lastrxpid     ;
  uint8_t              ackreq        ;
} nrf24l01p_packet_t;

typedef struct nrf24l01p_blocks_t{
  uint8_t              remaining     ;
  uint8_t              failedpos     ;
} nrf24l01p_blocks_t;

typedef struct nrf24l01p_t{
  uint8_t              mode          ;
  uint8_t              commen        ;
  uint8_t              tempbuf[2]    ;
  nrf24l01p_address_t  address       ;
  nrf24l01p_config_t   config        ;
  nrf24l01p_packet_t   packet        ;
  nrf24l01p_blocks_t   blocks        ;
  nrf24l01p_sp_alloc_t spacealloc    ;
  uint16_t             errorticks    ;
  uint8_t              error         ;
  uint8_t              errors[nRF24L01P_ERROR_FLAGS];
} nrf24l01p_t;

nrf24l01p_t nrf24l01p;



/* initialize structure of the driver */

void nrf24l01p_struct_init(void){
  nrf24l01p.mode = 0x00;
  nrf24l01p.commen = 0x01;
  nrf24l01p.tempbuf[0] = 0x00;
  nrf24l01p.tempbuf[1] = 0x00;
  nrf24l01p.address.own = 0x01;
  nrf24l01p.address.dest = 0x02;
  nrf24l01p.config.rxticks = 0;
  nrf24l01p.config.rxtimeout = 10;
  nrf24l01p.config.maxdatalen = 20;
  nrf24l01p.config.maxretry = 10;
  nrf24l01p.config.retryoccured = 0;
  nrf24l01p.packet.txpid = 0x01;
  nrf24l01p.packet.rxpid = 0x00;
  nrf24l01p.packet.lastrxpid = 0x00;
  nrf24l01p.packet.ackreq = 1;
  nrf24l01p.blocks.remaining = 0;
  nrf24l01p.blocks.failedpos = 0;
  nrf24l01p.spacealloc.crclsb = nRF24L01P_PACKET_LEN-1;
  nrf24l01p.spacealloc.crcmsb = nRF24L01P_PACKET_LEN-2;
  nrf24l01p.spacealloc.crclen = nRF24L01P_PACKET_LEN-2;
  nrf24l01p.spacealloc.len = nRF24L01P_PACKET_LEN-3;
  nrf24l01p.spacealloc.dest = nRF24L01P_PACKET_LEN-4;
  nrf24l01p.spacealloc.own = nRF24L01P_PACKET_LEN-5;
  nrf24l01p.spacealloc.ack = nRF24L01P_PACKET_LEN-6;
  nrf24l01p.spacealloc.pid = nRF24L01P_PACKET_LEN-7;
  nrf24l01p.spacealloc.blocks = nRF24L01P_PACKET_LEN-8;
  nrf24l01p.errorticks = 0;
  nrf24l01p.error = 0;
  for(uint8_t i = 0; i < nRF24L01P_ERROR_FLAGS; i++){
    nrf24l01p.errors[0] = 0;
  }
}




/* set data direction of associated gpio */

void nrf24l01p_set_sck_dd(uint8_t state){
  if(state){
    nRF24L01P_SCK_DDR |= (1<<nRF24L01P_SCK_bp);
  }else{
    nRF24L01P_SCK_DDR &=~(1<<nRF24L01P_SCK_bp);
  }
}

void nrf24l01p_set_miso_dd(uint8_t state){
  if(state){
    nRF24L01P_MISO_DDR |= (1<<nRF24L01P_MISO_bp);
  }else{
    nRF24L01P_MISO_DDR &=~(1<<nRF24L01P_MISO_bp);
  }
}

void nrf24l01p_set_mosi_dd(uint8_t state){
  if(state){
    nRF24L01P_MOSI_DDR |= (1<<nRF24L01P_MOSI_bp);
  }else{
    nRF24L01P_MOSI_DDR &=~(1<<nRF24L01P_MOSI_bp);
  }
}


void nrf24l01p_set_csn_dd(uint8_t state){
  if(state){
    nRF24L01P_CSN_DDR |= (1<<nRF24L01P_CSN_bp);
  }else{
    nRF24L01P_CSN_DDR &=~(1<<nRF24L01P_CSN_bp);
  }
}

void nrf24l01p_set_ce_dd(uint8_t state){
  if(state){
    nRF24L01P_CE_DDR |= (1<<nRF24L01P_CE_bp);
  }else{
    nRF24L01P_CE_DDR &=~(1<<nRF24L01P_CE_bp);
  }
}




/* set output state of associated gpio */

void nrf24l01p_set_sck(uint8_t state){
  if(state){
    nRF24L01P_SCK_PORT |= (1<<nRF24L01P_SCK_bp);
  }else{
    nRF24L01P_SCK_PORT &=~(1<<nRF24L01P_SCK_bp);
  }
}

void nrf24l01p_set_miso(uint8_t state){
  if(state){
    nRF24L01P_MISO_PORT |= (1<<nRF24L01P_MISO_bp);
  }else{
    nRF24L01P_MISO_PORT &=~(1<<nRF24L01P_MISO_bp);
  }
}

void nrf24l01p_set_mosi(uint8_t state){
  if(state){
    nRF24L01P_MOSI_PORT |= (1<<nRF24L01P_MOSI_bp);
  }else{
    nRF24L01P_MOSI_PORT &=~(1<<nRF24L01P_MOSI_bp);
  }
}

void nrf24l01p_set_csn(uint8_t state){
  if(state){
    nRF24L01P_CSN_PORT |= (1<<nRF24L01P_CSN_bp);
  }else{
    nRF24L01P_CSN_PORT &=~(1<<nRF24L01P_CSN_bp);
  }
}

void nrf24l01p_set_ce(uint8_t state){
  if(state){
    nRF24L01P_CE_PORT |= (1<<nRF24L01P_CE_bp);
  }else{
    nRF24L01P_CE_PORT &=~(1<<nRF24L01P_CE_bp);
  }
}




/* configuration of gpio for different states */

void nrf24l01p_gpio_enable(void){
  nrf24l01p_set_sck_dd(1);
  nrf24l01p_set_miso_dd(0);
  nrf24l01p_set_mosi_dd(1);
  nrf24l01p_set_csn_dd(1);
  nrf24l01p_set_ce_dd(1);
  nrf24l01p_set_csn(1);
  nrf24l01p_set_ce(0);
  #ifdef nRF24L01P_USE_HW_SPI
  DDRB |= (1<<2);
  #endif
}

void nrf24l01p_gpio_disable(void){
  nrf24l01p_set_sck_dd(1);
  nrf24l01p_set_miso_dd(0);
  nrf24l01p_set_mosi_dd(1);
  nrf24l01p_set_csn_dd(1);
  nrf24l01p_set_ce_dd(1);
  nrf24l01p_set_sck(0);
  nrf24l01p_set_miso(0);
  nrf24l01p_set_mosi(0);
  nrf24l01p_set_csn(1);
  nrf24l01p_set_ce(0);
}




/* configuration of spi for different states */

void nrf24l01p_spi_enable(void){
  #ifdef nRF24L01P_USE_HW_SPI
  SPCR = (1<<SPE)|(1<<MSTR);                     
  //SPSR = (1<<SPI2X);
  SPSR = 0;
  #endif
}

void nrf24l01p_spi_disable(void){
  #ifdef nRF24L01P_USE_HW_SPI
  SPCR = 0x00;                     
  SPSR = 0x00;
  #endif
}




/* configuration of gpio & spi for different states */

void nrf24l01p_enable(void){
  nrf24l01p_gpio_enable();
  nrf24l01p_spi_enable();
  nrf24l01p.commen = 1;
}

void nrf24l01p_disable(void){
  nrf24l01p_gpio_disable();
  nrf24l01p_spi_disable();
  nrf24l01p.commen = 0;
}



/* error handling */

void nrf24l01p_error_clear(void){
  nrf24l01p.error = 0;
}

void nrf24l01p_error_clear_ticks(void){
  nrf24l01p.errorticks = 0;
}

void nrf24l01p_error_clear_buf(void){
  for(uint8_t i = 0; i < nRF24L01P_ERROR_FLAGS; i++){
    nrf24l01p.errors[i] = 0;
  }
}

void nrf24l01p_error_clear_all(void){
  nrf24l01p_error_clear();
  nrf24l01p_error_clear_ticks();
  nrf24l01p_error_clear_buf();
}

void nrf24l01p_error_set(uint8_t val){
  nrf24l01p.error = val;
}

uint8_t nrf24l01p_error_get(void){
  return nrf24l01p.error;
}

uint8_t nrf24l01p_error_get_index(uint8_t index){
  return nrf24l01p.errors[index];
}

uint8_t nrf24l01p_error_timeout(uint16_t ticks){
  _delay_us(100);
  nrf24l01p.errorticks++;
  if(nrf24l01p.errorticks>ticks){
	return 1;
  }
  else{
    return 0;
  }
}

uint8_t nrf24l01p_error_free(void){
  if(nrf24l01p_error_get() == 0){
    return 1;
  }
  else{
    return 0;
  }
}



/* crc calcultions and validation */

uint16_t nrf24l01p_crc(uint16_t crc, uint8_t data){
  crc=crc^((uint16_t)data<<8);
  for(uint8_t i = 0; i < 8; i++){
    if(crc & 0x8000){
	  crc = (crc<<1)^0x1021;
	}
    else{
	  crc <<= 1;
	}
  }
  return crc;
}

uint16_t nrf24l01p_crc_block(uint8_t *buf, uint8_t len){
  uint16_t crc = 0;
  for(uint8_t i = 0; i < len; i++){
    crc = nrf24l01p_crc(crc,buf[i]);
  }
  return crc;
}



/* spi transfer */

uint8_t nrf24l01p_spi_transfer(uint8_t data){
  if(nrf24l01p_error_free()){
    SPDR = data;
    uint8_t i = 0;
    while(!(SPSR & (1 << SPIF))){
	  _delay_us(10);
	  i++;
	  if(i > 200){
	    nrf24l01p_error_set(0x01);
	    return 0;
	  }
    }
	return SPDR;
  }else{
    return 0;
  }
}


/* register read write */

void nrf24l01p_readwrite_register(uint8_t reg, uint8_t rw, uint8_t *data, uint8_t len){
  if(nrf24l01p_error_free()){
    if(nrf24l01p.commen == 0){
      nrf24l01p_enable();
    }
    nrf24l01p_set_csn(0);
    if(rw == 0){
      reg |= 0x20;
	  nrf24l01p_spi_transfer(reg);
	  for(uint8_t i = 0; i < len; i++){
	    nrf24l01p_spi_transfer(data[i]);
	  }
    }else{
      nrf24l01p_spi_transfer(reg);
      for(uint8_t i = 0; i < len; i++){
        data[i] = nrf24l01p_spi_transfer(0xFF);
      }
    }
    nrf24l01p_set_csn(1);
  }
}

void nrf24l01p_read_modify_write_register( uint8_t reg, uint8_t bit_pos, uint8_t bit_val ){
  if(nrf24l01p_error_free()){
    nrf24l01p_readwrite_register( reg, 1, nrf24l01p.tempbuf, 1);
    if(bit_val){
      nrf24l01p.tempbuf[0]|=(1<<bit_pos);
    }else{
      nrf24l01p.tempbuf[0]&=~(1<<bit_pos);
    }
    nrf24l01p_readwrite_register( reg, 0, nrf24l01p.tempbuf, 1 );
  }
}


/* tx and rx buffer flush */

void nrf24l01p_flush_tx_buf(void){
  nrf24l01p_readwrite_register( 0xE1, 0, nrf24l01p.tempbuf, 0 );
  if( !nrf24l01p_error_free() ){
    nrf24l01p_error_set(0x02);
  }
}

void nrf24l01p_flush_rx_buf(void){
  nrf24l01p_readwrite_register( 0xE2, 0, nrf24l01p.tempbuf, 0 );
  if(!nrf24l01p_error_free()){
    nrf24l01p_error_set(0x03);
  }
}



/* read and write in tx and rx fifo */

void nrf24l01p_write_data_tx_buf(uint8_t *data){
  nrf24l01p_readwrite_register( 0xA0, 0, data, nRF24L01P_PACKET_LEN );
  if( !nrf24l01p_error_free() ){
    nrf24l01p_error_set(0x04);
  }
}

void nrf24l01p_read_data_rx_buf(uint8_t *data){
  nrf24l01p_readwrite_register( 0x61, 1, data, nRF24L01P_PACKET_LEN );
  if( !nrf24l01p_error_free() ){
    nrf24l01p_error_set(0x05);
  }
}



/* read tx and rx fifo status */

uint8_t nrf24l01p_tx_buf_empty(void){
  if(nrf24l01p_error_free()){
    nrf24l01p.tempbuf[0] = 0;
    nrf24l01p_readwrite_register( 0x17, 1, nrf24l01p.tempbuf, 1);
    if(nrf24l01p.tempbuf[0] & (1<<4)){
      return 1;
    }else{
      return 0;
    }
  }else{
    return 0;
  }
}


uint8_t nrf24l01p_rx_buf_not_empty(void){
  if(nrf24l01p_error_free()){
    nrf24l01p.tempbuf[0] = 0;
    nrf24l01p_readwrite_register( 0x17, 1, nrf24l01p.tempbuf, 1);
    if((nrf24l01p.tempbuf[0] & (1<<0)) == 0){
      return 1;
    }else{
      return 0;
    }
  }else{
    return 0;
  }
}


uint8_t nrf24l01p_wait_tx_complete(void){
  if(nrf24l01p_error_free()){
    nrf24l01p_error_clear_ticks();
    while( !nrf24l01p_tx_buf_empty() ){
	  if(nrf24l01p_error_timeout(100)){
	    nrf24l01p_error_set(0x06);
	    return 0;
	  }
    }
	return 1;
  }else{
    return 0;
  }
}



/* set modes */

void nrf24l01p_mode_set_deep_sleep(void){
  if(nrf24l01p_error_free()){
    nrf24l01p.tempbuf[0] = 0x00;
    nrf24l01p_readwrite_register( 0x00, 0, nrf24l01p.tempbuf, 1 );
    nrf24l01p.mode = 0x00;
  }
  nrf24l01p_disable();
}

void nrf24l01p_mode_set_sleep(void){
  if(nrf24l01p.commen == 0){
    nrf24l01p_enable();
  }
  if(nrf24l01p_error_free()){
    nrf24l01p.tempbuf[0] = 0x00;
	nrf24l01p_set_ce(0);
    nrf24l01p_readwrite_register( 0x00, 0, nrf24l01p.tempbuf, 1 );
    nrf24l01p.mode = 0x01;
  }
}

void nrf24l01p_mode_set_rx(void){
  if(nrf24l01p.commen == 0){
    nrf24l01p_enable();
  }
  if(nrf24l01p_error_free()){
    nrf24l01p.tempbuf[0] = 0x73;
    nrf24l01p_set_ce(1);
    nrf24l01p_readwrite_register( 0x00, 0, nrf24l01p.tempbuf, 1 );
	nrf24l01p.mode = 0x02;
  }
}

void nrf24l01p_mode_set_tx(void){
  if(nrf24l01p.commen == 0){
    nrf24l01p_enable();
  }
  if(nrf24l01p_error_free()){
    nrf24l01p.tempbuf[0] = 0x72;
    nrf24l01p_set_ce(0);
    nrf24l01p_readwrite_register( 0x00, 0, nrf24l01p.tempbuf, 1 );
	nrf24l01p_set_ce(1);
	nrf24l01p.mode = 0x03;
  }
}

void nrf24l01p_mode_set(uint8_t mode){
  if(nrf24l01p_error_free()){
    if(mode == 0){
	  nrf24l01p_mode_set_deep_sleep();
	}else if(mode == 1){
	  nrf24l01p_mode_set_sleep();
	}else if(mode == 2){
	  nrf24l01p_mode_set_rx();
	}else if(mode ==3 ){
	  nrf24l01p_mode_set_tx();
	}else{
	  nrf24l01p_mode_set_rx();
	}
  }
}

uint8_t nrf24l01p_mode_get(void){
  if(nrf24l01p_error_free()){
    nrf24l01p_readwrite_register( 0x00, 1, nrf24l01p.tempbuf, 1 );
    if(nrf24l01p.tempbuf[0] & (1<<1)){
      if(nrf24l01p.tempbuf[0] & (1<<0)){
	    nrf24l01p.mode = 0x02;
	    return 2; //rx mode
	  }else{
	    nrf24l01p.mode = 0x03;
	    return 3; //tx mode
	  }
    }else{
      nrf24l01p.mode = 0x01;
      return 1;   //pwr down
    }
  }else{
    return 0;
  }
}



void nrf24l01p_channel_set(uint8_t channel){
  if(nrf24l01p_error_free()){
    if(channel > 120){
      channel = 120;
    }
    nrf24l01p.tempbuf[0] = channel;
    nrf24l01p_readwrite_register( 0x05, 0, nrf24l01p.tempbuf, 1 );
  }
}

uint8_t nrf24l01p_channel_get(void){
  if(nrf24l01p_error_free()){
    nrf24l01p_readwrite_register( 0x05, 1, nrf24l01p.tempbuf, 1 );
    return nrf24l01p.tempbuf[0];
  }else{
    return 0;
  }
}



void nrf24l01p_speed_set(uint8_t index){
  if(index == 0){       //250kbps
    nrf24l01p_read_modify_write_register( 0x06, 5, 1 );
    nrf24l01p_read_modify_write_register( 0x06, 3, 0 );
  }
  else if(index == 1){  //1Mbps
    nrf24l01p_read_modify_write_register( 0x06, 5, 0 );
    nrf24l01p_read_modify_write_register( 0x06, 3, 0 );
  }
  else if(index == 2){  //2Mbps
    nrf24l01p_read_modify_write_register( 0x06, 5, 0 );
    nrf24l01p_read_modify_write_register( 0x06, 3, 1 );
  }else{                //2Mbps
    nrf24l01p_read_modify_write_register( 0x06, 5, 0 );
    nrf24l01p_read_modify_write_register( 0x06, 3, 1 );
  }
}

uint8_t nrf24l01p_speed_get(void){
  if(nrf24l01p_error_free()){
    nrf24l01p_readwrite_register( 0x06, 1, nrf24l01p.tempbuf, 1 );
    nrf24l01p.tempbuf[1] = (nrf24l01p.tempbuf[0] >> 3) & 0x01;
    nrf24l01p.tempbuf[0] >>= 4;
    nrf24l01p.tempbuf[0] &= 0x02;
    nrf24l01p.tempbuf[1] |= nrf24l01p.tempbuf[0];
    if      (nrf24l01p.tempbuf[1] == 0x02){
      nrf24l01p.tempbuf[0] = 0;
    }else if(nrf24l01p.tempbuf[1] == 0x01){
      nrf24l01p.tempbuf[0] = 1;
    }else if(nrf24l01p.tempbuf[1] == 0x00){
      nrf24l01p.tempbuf[0] = 2;
    }
    return nrf24l01p.tempbuf[0];
  }else{
    return 0;
  }
}



void nrf24l01p_tx_power_set(uint8_t index){
  if(nrf24l01p_error_free()){
    if(index == 0){
      nrf24l01p_read_modify_write_register( 0x06, 2, 0 );
      nrf24l01p_read_modify_write_register( 0x06, 1, 0 );
    }
    else if(index == 1){
      nrf24l01p_read_modify_write_register( 0x06, 2, 0 );
      nrf24l01p_read_modify_write_register( 0x06, 1, 1 );
    }
    else if(index == 2){
      nrf24l01p_read_modify_write_register( 0x06, 2, 1 );
      nrf24l01p_read_modify_write_register( 0x06, 1, 0 );
    }
    else if(index == 3){
      nrf24l01p_read_modify_write_register( 0x06, 2, 1 );
      nrf24l01p_read_modify_write_register( 0x06, 1, 1 );
    }else{
      nrf24l01p_read_modify_write_register( 0x06, 2, 1 );
      nrf24l01p_read_modify_write_register( 0x06, 1, 1 );
    }
  }
}

uint8_t nrf24l01p_tx_power_get(void){
  if(nrf24l01p_error_free()){
    nrf24l01p_readwrite_register( 0x06, 1, nrf24l01p.tempbuf, 1 );
    nrf24l01p.tempbuf[0] >>= 1;
    nrf24l01p.tempbuf[0] &= 0x03;
    return nrf24l01p.tempbuf[0];
  }else{
    return 0;
  }
}  

void nrf24l01p_own_addr_set(uint8_t addr){
  nrf24l01p.address.own=addr;
}

uint8_t nrf24l01p_own_addr_get(void){
  return nrf24l01p.address.own;
}

void nrf24l01p_dest_addr_set(uint8_t addr){
  nrf24l01p.address.dest=addr;
}

uint8_t nrf24l01p_dest_addr_get(void){
  return nrf24l01p.address.dest;
}

void nrf24l01p_rxticks_set(uint16_t val){
  nrf24l01p.config.rxticks=val;
}

uint16_t nrf24l01p_rxticks_get(void){
  return nrf24l01p.config.rxticks;
}

void nrf24l01p_rx_timeout_set(uint16_t val){
  nrf24l01p.config.rxtimeout=val;
}

uint16_t nrf24l01p_rx_timeout_get(void){
  return nrf24l01p.config.rxtimeout;
}

void nrf24l01p_max_retransmission_set(uint16_t val){
  nrf24l01p.config.maxretry=val;
}

uint16_t nrf24l01p_max_retransmission_get(void){
  return nrf24l01p.config.maxretry;
}

void nrf24l01p_tx_basic(uint8_t *buf, uint8_t len){
  nrf24l01p_error_clear();
  nrf24l01p_flush_tx_buf();
  buf[nrf24l01p.spacealloc.blocks] = nrf24l01p.blocks.remaining;      //24
  buf[nrf24l01p.spacealloc.pid   ] = nrf24l01p.packet.txpid;          //25
  buf[nrf24l01p.spacealloc.ack   ] = nrf24l01p.packet.ackreq;         //26
  buf[nrf24l01p.spacealloc.own   ] = nrf24l01p.address.own;           //27
  buf[nrf24l01p.spacealloc.dest  ] = nrf24l01p.address.dest;          //28
  buf[nrf24l01p.spacealloc.len   ] = len;                             //29
  uint16_t temp = nrf24l01p_crc_block( buf, nrf24l01p.spacealloc.crclen );
  buf[nrf24l01p.spacealloc.crcmsb] = (temp & 0xFF00)>>8;              //30
  buf[nrf24l01p.spacealloc.crclsb] = (temp & 0x00FF);                 //31
  nrf24l01p_write_data_tx_buf(buf);
  nrf24l01p_mode_set(3);                                               //tx
  nrf24l01p_wait_tx_complete(); 
  nrf24l01p_mode_set(2);                                               //rx
}


uint8_t nrf24l01p_rx_basic(uint8_t *buf){
  uint8_t sts=0;
  nrf24l01p_error_clear();
  nrf24l01p_rxticks_set(0);
  while(nrf24l01p_rxticks_get() < nrf24l01p_rx_timeout_get()){
    if(nrf24l01p_rx_buf_not_empty()){
      nrf24l01p_read_data_rx_buf(buf);
	  uint16_t rec_crc=buf[nrf24l01p.spacealloc.crcmsb];               //30
	  rec_crc<<=8;
	  rec_crc|=buf[nrf24l01p.spacealloc.crclsb];                       //31
      uint16_t calc_crc=nrf24l01p_crc_block(buf, nrf24l01p.spacealloc.crclen);
      if(rec_crc == calc_crc){
	    nrf24l01p.packet.rxpid = buf[nrf24l01p.spacealloc.pid];        //25
	    nrf24l01p.blocks.remaining = buf[nrf24l01p.spacealloc.blocks]; //24
		sts=1;
		break;
	  }
    }else{
      _delay_us(100);
      nrf24l01p.config.rxticks++;
	}
  }
  return sts;
}


uint8_t nrf24l01p_tx_with_ack(uint8_t *buf, uint8_t len){
  uint8_t  sts = 0, ack_buf[32];
  uint16_t timeout_mem = nrf24l01p_rx_timeout_get();
  nrf24l01p_rx_timeout_set(nRF24L01P_TX_ACK_RCV_TIMEOUT);
  nrf24l01p_tx_basic(buf, len);
  if( nrf24l01p.packet.ackreq ){                    //need to improve ack type
    if( nrf24l01p_rx_basic(ack_buf) ){
      if( nrf24l01p_own_addr_get() == ack_buf[nrf24l01p.spacealloc.dest] ){
        sts=1;
      }
    }
  }
  nrf24l01p_rx_timeout_set(timeout_mem);
  return sts;
}

uint8_t nrf24l01p_rx_with_ack(uint8_t *buf){
  uint8_t sts=0, ack_buf[32]="~^~";
  if( nrf24l01p_rx_basic(buf) ){
    if( ((buf[nrf24l01p.spacealloc.ack] & 1)==1) && ( nrf24l01p_own_addr_get() == buf[nrf24l01p.spacealloc.dest]) ){ //check ack type
      _delay_us(nRF24L01P_RX_ACK_SEND_DELAY);                                                                                               //check delay
	  nrf24l01p_dest_addr_set(buf[nrf24l01p.spacealloc.own]);
	  nrf24l01p.packet.txpid = buf[nrf24l01p.spacealloc.pid];
	  nrf24l01p.blocks.remaining = buf[nrf24l01p.spacealloc.blocks];
      nrf24l01p_tx_basic(ack_buf, 3);
      sts=1;
    }
  }
  return sts;
}

void nrf24l01p_init(void){
  nrf24l01p_struct_init();
  nrf24l01p_enable();
  nrf24l01p.tempbuf[0] = 0x00;  nrf24l01p_readwrite_register( 0x00, 0, nrf24l01p.tempbuf, 1 );
  nrf24l01p.tempbuf[0] = 0x00;  nrf24l01p_readwrite_register( 0x01, 0, nrf24l01p.tempbuf, 1 );
  nrf24l01p.tempbuf[0] = 0x03;  nrf24l01p_readwrite_register( 0x02, 0, nrf24l01p.tempbuf, 1 );
  nrf24l01p.tempbuf[0] = 0x01;  nrf24l01p_readwrite_register( 0x03, 0, nrf24l01p.tempbuf, 1 );
  nrf24l01p.tempbuf[0] = 0x00;  nrf24l01p_readwrite_register( 0x04, 0, nrf24l01p.tempbuf, 1 );
  nrf24l01p.tempbuf[0] = 0x02;  nrf24l01p_readwrite_register( 0x05, 0, nrf24l01p.tempbuf, 1 );
  nrf24l01p.tempbuf[0] = 0x26;  nrf24l01p_readwrite_register( 0x06, 0, nrf24l01p.tempbuf, 1 ); 
  nrf24l01p.tempbuf[0] = 0x70;  nrf24l01p_readwrite_register( 0x07, 0, nrf24l01p.tempbuf, 1 );
  nrf24l01p.tempbuf[0] = nRF24L01P_PACKET_LEN;  
  nrf24l01p_readwrite_register( 0x11, 0, nrf24l01p.tempbuf, 1 );
  nrf24l01p.tempbuf[0] = nRF24L01P_PACKET_LEN;  
  nrf24l01p_readwrite_register( 0x12, 0, nrf24l01p.tempbuf, 1 );
  nrf24l01p.tempbuf[0] = 0x00;  nrf24l01p_readwrite_register( 0x1C, 0, nrf24l01p.tempbuf, 1 );
  nrf24l01p.tempbuf[0] = 0x00;  nrf24l01p_readwrite_register( 0x1D, 0, nrf24l01p.tempbuf, 1 );
  nrf24l01p_readwrite_register( 0x10, 0, (uint8_t*)"ACK00", 5 );
  nrf24l01p_readwrite_register( 0x0A, 0, (uint8_t*)"ACK00", 5 );
  nrf24l01p_mode_set(2);
}



