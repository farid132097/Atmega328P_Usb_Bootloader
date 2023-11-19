

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

void     debug_loop_counter_inc(void);
uint16_t debug_get_count(void);
uint16_t debug_get_loop_counter(void);
uint8_t  debug_get_loop_counter_ovf(void);
void     debug_init(void);