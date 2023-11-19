

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "debug.h"


//#define  DEBUG_GPIO_SWITCH_DEBUGGING

uint16_t BAUD_9600[3] ={1230,1180,590};
uint16_t BAUD_19200[3]={590,570,230};
uint16_t BAUD_38400[3]={280,270,70};

typedef struct debug_t{
  volatile uint8_t   error;
  volatile uint8_t   datareg;
  volatile uint8_t   databsy;
  volatile uint8_t   buf[DEBUG_RX_BUF_SIZE];
  volatile uint8_t   bufindex;
  uint8_t            digits[8];
  uint8_t            input_num_digits;
}debug_t;

debug_t debug;

void debug_struct_init(void){
  debug.error=0;
  debug.datareg=0;
  debug.databsy=1;
  for(uint8_t i=0;i<DEBUG_RX_BUF_SIZE;i++){
    debug.buf[i]=0;
  }
  debug.bufindex=0;
  for(uint8_t i=0;i<8;i++){
    debug.digits[i]=0;
  }
  debug.input_num_digits=0;
}

void debug_timings_init(void){
  #ifdef DEBUG_USE_TIMER0
  TCCR0A=0x00;
  TCCR0B=0x00;
  TIMSK0=0x00;
  TIFR0 =0xff;
  #endif
  
  #ifdef DEBUG_USE_TIMER1
  TCCR1A=0x00;
  TCCR1B=0x00;
  TCCR1C=0x00;
  TIMSK1=0x00;
  TIFR1 =0xff;
  #endif
  
  #ifdef DEBUG_USE_TIMER2
  TCCR2A=0x00;
  TCCR2B=0x00;
  TIMSK2=0x00;
  TIFR2 =0xff;
  #endif
  
  
  PCICR |=(1<<PCIE1);
  PCMSK1|=(1<<PCINT12);
  PCIFR |=(1<<PCIF1);
  sei();
}

void debug_gpio_init(void){
  DEBUG_TX_DDR |= (1<<DEBUG_TX_bp);
  DEBUG_TX_PORT|= (1<<DEBUG_TX_bp);
  DEBUG_RX_DDR &=~(1<<DEBUG_RX_bp);
  DEBUG_RX_PORT|= (1<<DEBUG_RX_bp);
  
  #ifdef DEBUG_GPIO_SWITCH_DEBUGGING
  DEBUG_TEST_DDR|=(1<<DEBUG_TEST_bp);
  DEBUG_TEST_PORT&=~(1<<DEBUG_TEST_bp);
  #endif
}

void debug_tx_high(void){
  DEBUG_TX_PORT|= (1<<DEBUG_TX_bp);
}

void debug_tx_low(void){
  DEBUG_TX_PORT&=~(1<<DEBUG_TX_bp);
}

void debug_tx_set(uint8_t pin_state){
  if(pin_state==1){
    debug_tx_high();
  }else{
    debug_tx_low();
  }
}



uint8_t debug_rx_get(void){
  if(DEBUG_RX_PIN & (1<<DEBUG_RX_bp)){
    return 1;
  }else{
    return 0;
  }
}

void debug_delay(uint16_t val){
  #ifdef DEBUG_USE_DELAY
  _delay_us(val);
  #endif
  
  #ifdef DEBUG_USE_TIMER0
  TCNT0=0;
  TCCR0B=(1<<CS00);
  while(TCNT0<val){}
  TCCR0B=0;
  TCNT0=0;
  #endif
  
  #ifdef DEBUG_USE_TIMER1
  TCNT1=0;
  TCCR1B=(1<<CS10);
  while(TCNT1<val){}
  TCCR1B=0x00;
  #endif
  
  #ifdef DEBUG_USE_TIMER2
  TCNT2=0;
  TCCR2B=(1<<CS20);
  while(TCNT2<val){}
  TCCR2B=0;
  TCNT2=0;
  #endif
}

void debug_tx_byte(uint8_t val){
  uint8_t buf[10]={0,0,0,0,0,0,0,0,0,1};
  for(uint8_t i=0;i<8;i++){
    if(val & (1<<i)){
	  buf[i+1]=1;
	}else{
	  buf[i+1]=0;
	}
  }
  uint8_t sreg=SREG;
  cli();
  for(uint8_t i=0;i<10;i++){
    debug_tx_set(buf[i]);
	
	#ifdef DEBUG_BAUD_RATE_9600
	debug_delay(BAUD_9600[0]);
	#endif
	
	#ifdef DEBUG_BAUD_RATE_19200
	debug_delay(BAUD_19200[0]);
	#endif
	
	#ifdef DEBUG_BAUD_RATE_38400
	debug_delay(BAUD_38400[0]);
	#endif
  }
  SREG=sreg;
}

uint8_t debug_rx_byte(void){
  if(debug_rx_get()==0){
    uint8_t val=0, sts=0;
	#ifdef DEBUG_BAUD_RATE_9600
	debug_delay(BAUD_9600[2]);
	#endif
	
	#ifdef DEBUG_BAUD_RATE_19200
	debug_delay(BAUD_19200[2]);
	#endif
	
	#ifdef DEBUG_BAUD_RATE_38400
	debug_delay(BAUD_38400[2]);
	#endif
	
	for(uint8_t i=0;i<10;i++){
	  if(i==0){
		if(debug_rx_get()==0){
		  #ifdef DEBUG_GPIO_SWITCH_DEBUGGING
		  DEBUG_TEST_PORT|=(1<<DEBUG_TEST_bp);
		  DEBUG_TEST_PORT&=~(1<<DEBUG_TEST_bp);
		  #endif
		  sts=1;
		}
		
		#ifdef DEBUG_BAUD_RATE_9600
	    debug_delay(BAUD_9600[1]);
	    #endif
	
	    #ifdef DEBUG_BAUD_RATE_19200
	    debug_delay(BAUD_19200[1]);
	    #endif
	
	    #ifdef DEBUG_BAUD_RATE_38400
	    debug_delay(BAUD_38400[1]);
	    #endif
		
      }
	  else if(i>=1 && i<=8){
	    if(debug_rx_get()){
		  val|=0x80;
		}else{
		  val|=0x00;
		}
		#ifdef DEBUG_GPIO_SWITCH_DEBUGGING
		DEBUG_TEST_PORT|=(1<<DEBUG_TEST_bp);
		DEBUG_TEST_PORT&=~(1<<DEBUG_TEST_bp);
		#endif
		if(i!=8){
		  val>>=1;
		}
		
		#ifdef DEBUG_BAUD_RATE_9600
	    debug_delay(BAUD_9600[1]);
	    #endif
	
	    #ifdef DEBUG_BAUD_RATE_19200
	    debug_delay(BAUD_19200[1]);
	    #endif
	
	    #ifdef DEBUG_BAUD_RATE_38400
	    debug_delay(BAUD_38400[1]);
	    #endif
		
	  }
	  else if(i==9){
		if(debug_rx_get()==1){
		  #ifdef DEBUG_GPIO_SWITCH_DEBUGGING
		  DEBUG_TEST_PORT|=(1<<DEBUG_TEST_bp);
		  DEBUG_TEST_PORT&=~(1<<DEBUG_TEST_bp);
		  #endif
		  sts&=1;
		  if(sts==1){
			debug.datareg=val;
			debug.error=0x00;
		  }else{
			debug.datareg=0;
			debug.error=0x01;
		  }
		}else{
		  debug.datareg=0;
		  debug.error=0x02;
		}
	  }
	}
  }else{
    debug.error=0x03;
  }
  return debug.datareg;
}

uint8_t debug_rx_byte_get(void){
  return debug.datareg;
}

void debug_rx_byte_clear(void){
  debug.datareg=0;
}

void debug_rx_byte_set(uint8_t val){
  debug.datareg=val;
}

void debug_tx_hex(uint32_t val){
  uint16_t hex_digit, index=0, loop_counter=0;
  if(val <= 0xFF){
    index=8;
    loop_counter=2;
  }else if(val <= 0xFFFF){
    index=16;
    loop_counter=4;     
  }else{
    index=32;
    loop_counter=8;
  }
  debug_tx_byte('0');
  debug_tx_byte('x');
	for(uint8_t i=0;i<loop_counter;i++){
	  index-=4;
	  hex_digit=(uint8_t)((val>>index) & 0x0F);
	  if(hex_digit>9){
	    hex_digit+=55;
	  }else {
	    hex_digit+=48;
	  }
	  debug_tx_byte((uint8_t)hex_digit);
	}
}

void debug_tx_bin(uint32_t val){
  uint8_t loop_counter=0;
  if(val <= 0xFF){
    loop_counter=7;
  }else if(val <= 0xFFFF){
    loop_counter=15;     
  }else{
    loop_counter=31;
  }
  
  debug_tx_byte('0');
  debug_tx_byte('b');
  for(int i=loop_counter; i>=0; i--){
    if( (val>>i) & 1){
      debug_tx_byte( 49 );   
    }else{
      debug_tx_byte( 48 );         
    }
  }
}

void debug_tx_nl(void){
  debug_tx_byte('\r');
  debug_tx_byte('\n');
}

void debug_tx_sp(void){
  debug_tx_byte(' ');
}

void debug_tx_cm(void){
  debug_tx_byte(',');
}

void debug_tx_text(char *str){
    uint8_t i=0;
    while(str[i]!='\0'){
        debug_tx_byte(str[i]);
        i++;
    }
}

void debug_tx_text_nl(char *str){
  debug_tx_text(str);
  debug_tx_nl();
}

void debug_tx_text_sp(char *str){
  debug_tx_text(str);
  debug_tx_sp();
}

void debug_tx_text_cm(char *str){
  debug_tx_text(str);
  debug_tx_cm();
}

void debug_determine_digit_numbers(uint32_t num){
  uint8_t i=0;
  if(num==0){
    debug.digits[0]=0;
    debug.input_num_digits=1;
  }else{
    while(num!=0){
      debug.digits[i]=num%10;
      num/=10;
      i++;
    }
	debug.input_num_digits=i;
  }
}

void debug_tx_number_digits(void){
  for(uint8_t i=debug.input_num_digits; i>0; i--){
    uint8_t temp=i;
    temp-=1;
    temp=debug.digits[temp];
    temp+=48;
    debug_tx_byte(temp);
  }
}

void debug_tx_number(int32_t num){
  if(num<0){
    debug_tx_byte('-');
	num=-num;
  }
  debug_determine_digit_numbers((uint32_t)num);
  debug_tx_number_digits();
}

void debug_tx_number_nl(int32_t num){
  debug_tx_number(num);
  debug_tx_nl();
}

void debug_tx_number_sp(int32_t num){
  debug_tx_number(num);
  debug_tx_sp();
}

void debug_tx_number_cm(int32_t num){
  debug_tx_number(num);
  debug_tx_text(",");
}

void debug_tx_number_hex_nl(int32_t num){
  debug_tx_hex(num);
  debug_tx_nl();
}

void debug_tx_number_hex_sp(int32_t num){
  debug_tx_hex(num);
  debug_tx_sp();
}

void debug_tx_number_hex_cm(int32_t num){
  debug_tx_hex(num);
  debug_tx_text(",");
}

void debug_tx_parameter_nl(char *name, int32_t num){
  debug_tx_text(name);
  debug_tx_sp();
  debug_tx_number_nl(num);
}

void debug_tx_parameter_sp(char *name, int32_t num){
  debug_tx_text(name);
  debug_tx_sp();
  debug_tx_number_sp(num);
}

void debug_tx_parameter_cm(char *name, int32_t num){
  debug_tx_text(name);
  debug_tx_sp();
  debug_tx_number_cm(num);
}

void debug_tx_parameter_hex_nl(char *name, int32_t num){
  debug_tx_text(name);
  debug_tx_sp();
  debug_tx_number_hex_nl(num);
}

void debug_tx_parameter_hex_sp(char *name, int32_t num){
  debug_tx_text(name);
  debug_tx_sp();
  debug_tx_number_hex_sp(num);
}

void Debug_tx_parameter_hex_cm(char *name, int32_t num){
  debug_tx_text(name);
  debug_tx_sp();
  debug_tx_number_hex_cm(num);
}



uint8_t debug_buf_get(uint8_t index){
  return debug.buf[index];
}

uint8_t debug_buf_index_get(void){
  return debug.bufindex;
}

void debug_flush_buf(void){
  for(uint8_t i=0;i<DEBUG_RX_BUF_SIZE;i++){
    debug.buf[i]=0;
  }
  debug.bufindex=0;
}

uint8_t debug_databsy_get(void){
  return debug.databsy;
}

void debug_init(void){
  debug_struct_init();
  debug_timings_init();
  debug_gpio_init();
}


ISR(PCINT1_vect){
  debug.databsy=1;
  uint8_t val=debug_rx_byte();
  if(debug.error==0){
    debug.buf[debug.bufindex]=val;
	debug.bufindex++;
	if(debug.bufindex>DEBUG_RX_BUF_SIZE){
	  debug.bufindex=0;
	}
	
	TCCR1A=0x00;
    TCCR1B=0x00;
    TCCR1C=0x00;
    TIFR1 =0xff;
    TIMSK1=0x00;
    TCNT1 =53000;
    TCCR1B=(1<<CS10);
    TIFR1 =(1<<TOV1);
    TIMSK1=(1<<TOIE1);
  }
}

ISR(TIMER1_OVF_vect){
  TIMSK1=0;
  TCCR1B=0;
  TCNT1 =0;
  #ifdef DEBUG_GPIO_SWITCH_DEBUGGING
  DEBUG_TEST_PORT|= (1<<DEBUG_TEST_bp);
  DEBUG_TEST_PORT&=~(1<<DEBUG_TEST_bp);
  #endif
  debug.databsy=0;
}
  
  
  
  
  
  
  
  