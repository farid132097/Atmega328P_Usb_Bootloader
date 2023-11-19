
#define  DEBUG_TX_DDR     DDRC
#define  DEBUG_TX_PORT    PORTC
#define  DEBUG_TX_PIN     PINC
#define  DEBUG_TX_bp      5U

#define  DEBUG_RX_DDR     DDRC
#define  DEBUG_RX_PORT    PORTC
#define  DEBUG_RX_PIN     PINC
#define  DEBUG_RX_bp      4U

#define  DEBUG_TEST_DDR   DDRC
#define  DEBUG_TEST_PORT  PORTC
#define  DEBUG_TEST_PIN   PINC
#define  DEBUG_TEST_bp    3U

#define  DEBUG_BAUD_RATE  57600
#define  DEBUG_BAUD_TIME  (1000000/DEBUG_BAUD_RATE)


//#define  DEBUG_USE_DELAY
//#define  DEBUG_USE_TIMER0
#define  DEBUG_USE_TIMER1
//#define  DEBUG_USE_TIMER2


//#define  DEBUG_TIMER_DELAY_TICKS 85   /*Calculated for 115200*/
#define  DEBUG_TIMER_DELAY_TICKS  1230  /*Calculated for 9600*/
#define  DEBUG_TIMER_HDELAY_TICKS 605   /*Calculated for 9600*/

#define  DEBUG_RX_DELAY_TICKS  1180     /*Calculated for 9600*/
#define  DEBUG_RX_HDELAY_TICKS 590      /*Calculated for 9600*/


#define  DEBUG_RX_BUF_SIZE 32

typedef struct debug_t{
  volatile uint8_t   error;
  volatile uint8_t   datareg;
  volatile uint8_t   databsy;
  volatile uint8_t   buf[DEBUG_RX_BUF_SIZE];
  volatile uint8_t   bufindex;
  uint8_t            digits[8];
  uint8_t            input_num_digits;
  uint16_t           count;
  uint16_t           loop_counter;
  uint8_t            loop_counter_sts;
  uint8_t            reset_sts;
}debug_t;


void     debug_struct_init(void);
void     debug_timings_init(void);
void     debug_gpio_init(void);
void     debug_tx_high(void);
void     debug_tx_low(void);
void     debug_tx_set(uint8_t pin_state);
uint8_t  debug_rx_get(void);

void     debug_delay(uint16_t val);

void     debug_tx_byte(uint8_t val);
uint8_t  debug_rx_byte(void);
uint8_t  debug_rx_byte_get(void);
void     debug_rx_byte_clear(void);
void     debug_rx_byte_set(uint8_t val);

void     debug_tx_hex(uint32_t val);
void     debug_tx_bin(uint32_t val);

void     debug_tx_nl(void);
void     debug_tx_sp(void);
void     debug_tx_cm(void);

void     debug_tx_text(char *str);
void     debug_tx_text_nl(char *str);
void     debug_tx_text_sp(char *str);
void     debug_tx_text_cm(char *str);

void     debug_determine_digit_numbers(uint32_t num);
void     debug_tx_number_digits(void);

void     debug_tx_number(int32_t num);
void     debug_tx_number_nl(int32_t num);
void     debug_tx_number_sp(int32_t num);
void     debug_tx_number_cm(int32_t num);

void     debug_tx_number_hex_nl(int32_t num);
void     debug_tx_number_hex_sp(int32_t num);
void     debug_tx_number_hex_cm(int32_t num);

void     debug_tx_parameter_nl(char *name, int32_t num);
void     debug_tx_parameter_sp(char *name, int32_t num);
void     debug_tx_parameter_cm(char *name, int32_t num);

void     debug_tx_parameter_hex_nl(char *name, int32_t num);
void     debug_tx_parameter_hex_sp(char *name, int32_t num);
void     debug_tx_parameter_hex_cm(char *name, int32_t num);

uint8_t  debug_buf_get(uint8_t index);
uint8_t  debug_buf_index_get(void);
void     debug_flush_buf(void);
uint8_t  debug_databsy_get(void);

void     debug_init(void);