

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define  DEBUG_TX_DDR     DDRD
#define  DEBUG_TX_PORT    PORTD
#define  DEBUG_TX_PIN     PIND
#define  DEBUG_TX_bp      6U

#define  DEBUG_RX_DDR     DDRD
#define  DEBUG_RX_PORT    PORTD
#define  DEBUG_RX_PIN     PIND
#define  DEBUG_RX_bp      5U 

#define  DEBUG_BAUD_RATE  57600
#define  DEBUG_BAUD_TIME  (1000000/DEBUG_BAUD_RATE)

#define  DEBUG_LP_CNT_LMT 100

//#define  DEBUG_USE_DELAY
//#define  DEBUG_USE_TIMER0
#define  DEBUG_USE_TIMER1
//#define  DEBUG_USE_TIMER2


//#define  DEBUG_TIMER_DELAY_TICKS 85   /*Calculated for 115200*/
#define  DEBUG_TIMER_DELAY_TICKS  1230  /*Calculated for 9600*/
#define  DEBUG_TIMER_HDELAY_TICKS 605   /*Calculated for 9600*/

#define  DEBUG_RX_DELAY_TICKS  1180     /*Calculated for 9600*/
#define  DEBUG_RX_HDELAY_TICKS 590      /*Calculated for 9600*/


typedef struct debug_t{
  volatile uint8_t   error;
  volatile uint8_t   dr;
  uint8_t            digits[8];
  uint8_t            input_num_digits;
  uint16_t           count;
  uint16_t           loop_counter;
  uint8_t            loop_counter_sts;
  uint8_t            reset_sts;
}debug_t;


debug_t debug;

void debug_struct_init(void){
  debug.error=0;
  debug.dr=0;
  for(uint8_t i=0;i<8;i++){
    debug.digits[i]=0;
  }
  debug.input_num_digits=0;
  debug.count=0;
  debug.loop_counter=0;
  debug.loop_counter_sts=0;
  debug.reset_sts=0;
}

void debug_timings_init(void){
  #ifdef DEBUG_USE_TIMER0
  TCCR0A=0x00;
  TCCR0B=(1<<CS00);
  TIMSK0=0x00;
  TIFR0=0x00;
  #define DEBUG_TIMER0_DELAY_TICKS DEBUG_TIMER_DELAY_TICKS
  #endif
  
  #ifdef DEBUG_USE_TIMER1
  TCCR1A=0x00;
  TCCR1B=(1<<CS10);
  TCCR1C=0x00;
  TIMSK1=0x00;
  TIFR1=0x00;
  #define  DEBUG_TIMER1_DELAY_TICKS DEBUG_TIMER_DELAY_TICKS-10
  #endif
  
  #ifdef DEBUG_USE_TIMER2
  TCCR2A=0x00;
  TCCR2B=(1<<CS20);
  TIMSK2=0x00;
  TIFR2=0x00;
  #define DEBUG_TIMER2_DELAY_TICKS DEBUG_TIMER_DELAY_TICKS
  #endif
  
  PCICR |=(1<<PCIE2);
  PCMSK2|=(1<<PCINT21);
  sei();
}

void debug_gpio_init(void){
  DEBUG_TX_DDR |= (1<<DEBUG_TX_bp);
  DEBUG_TX_PORT|= (1<<DEBUG_TX_bp);
  DEBUG_RX_DDR &=~(1<<DEBUG_RX_bp);
  DEBUG_RX_PORT|= (1<<DEBUG_RX_bp);
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
  while(TCNT0<val){}
  #endif
  
  #ifdef DEBUG_USE_TIMER1
  TCNT1=0;
  while(TCNT1<val){}
  #endif
  
  #ifdef DEBUG_USE_TIMER2
  TCNT2=0;
  while(TCNT2<val){}
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
  if(sreg & (1<<7)){
    cli();
  }
  for(uint8_t i=0;i<10;i++){
    debug_tx_set(buf[i]);
	debug_delay(DEBUG_TIMER_DELAY_TICKS);
  }
  if(sreg & (1<<7)){
    sei();
  }
}

uint8_t debug_rx_byte(void){
  return debug.dr;
}

void debug_rx_byte_clear(void){
  debug.dr=0;
}

void debug_rx_byte_set(uint8_t val){
  debug.dr=val;
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

void debug_tx_parameter_hex_cm(char *name, int32_t num){
  debug_tx_text(name);
  debug_tx_sp();
  debug_tx_number_hex_cm(num);
}

void debug_loop_counter_inc(void){
  debug.loop_counter++;
  if(debug.loop_counter>DEBUG_LP_CNT_LMT){
    debug.count++;
	if(debug.count>65000){
	  debug.count=0;
	}
    debug.loop_counter=0;
	debug.loop_counter_sts=1;
  }
}

uint16_t debug_get_count(void){
  return debug.count;
}

uint16_t debug_get_loop_counter(void){
  return debug.loop_counter;
}

uint8_t debug_get_loop_counter_ovf(void){
  if(debug.loop_counter_sts){
    debug.loop_counter_sts=0;
	return 1;
  }else{
    return 0;
  }
}


void debug_init(void){
  debug_struct_init();
  debug_timings_init();
  debug_gpio_init();
}


ISR(PCINT2_vect){
  if(debug_rx_get()==0){
	uint8_t val=0, sts=0;
	debug_delay(DEBUG_RX_HDELAY_TICKS);
	for(uint8_t i=0;i<10;i++){
	  if(i==0){
		if(debug_rx_get()==0){
		  debug_tx_set(0);
		  debug_tx_set(1);
		  sts=1;
		}
		debug_delay(DEBUG_RX_DELAY_TICKS);
      }
	  else if(i>=1 && i<=8){
		val|=debug_rx_get();
		debug_tx_set(0);
		debug_tx_set(1);
		if(i!=8){
		  val<<=1;
		}
		debug_delay(DEBUG_RX_DELAY_TICKS);
	  }
	  else if(i==9){
		if(debug_rx_get()==1){
		  sts&=1;
		  if(sts==1){
			debug_rx_byte_set(val);
		  }else{
			debug_rx_byte_clear();
		  }
		}else{
		  debug_rx_byte_clear();
		}
	  }
	}
  }
}
  
  
  
  
  
  
  
  
  