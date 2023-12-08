
#define  nRF24L01P_USE_HW_SPI
 
#define  nRF24L01P_SCK_DDR            DDRB
#define  nRF24L01P_SCK_PORT           PORTB
#define  nRF24L01P_SCK_bp             5

#define  nRF24L01P_MISO_DDR           DDRB
#define  nRF24L01P_MISO_PORT          PORTB
#define  nRF24L01P_MISO_bp            4

#define  nRF24L01P_MOSI_DDR           DDRB
#define  nRF24L01P_MOSI_PORT          PORTB
#define  nRF24L01P_MOSI_bp            3

#define  nRF24L01P_CSN_DDR            DDRB
#define  nRF24L01P_CSN_PORT           PORTB
#define  nRF24L01P_CSN_bp             2

#define  nRF24L01P_CE_DDR             DDRC
#define  nRF24L01P_CE_PORT            PORTC
#define  nRF24L01P_CE_bp              0

#define  nRF24L01P_ERROR_FLAGS        10
#define  nRF24L01P_PACKET_LEN         32
#define  nRF24L01P_TX_ACK_RCV_TIMEOUT 5
#define  nRF24L01P_RX_ACK_SEND_DELAY  500

void     nrf24l01p_struct_init(void);
void     nrf24l01p_set_sck_dd(uint8_t state);
void     nrf24l01p_set_miso_dd(uint8_t state);
void     nrf24l01p_set_mosi_dd(uint8_t state);
void     nrf24l01p_set_csn_dd(uint8_t state);
void     nrf24l01p_set_ce_dd(uint8_t state);

void     nrf24l01p_set_sck(uint8_t state);
void     nrf24l01p_set_miso(uint8_t state);
void     nrf24l01p_set_mosi(uint8_t state);
void     nrf24l01p_set_csn(uint8_t state);
void     nrf24l01p_set_ce(uint8_t state);

void     nrf24l01p_gpio_enable(void);
void     nrf24l01p_gpio_disable(void);
void     nrf24l01p_spi_enable(void);
void     nrf24l01p_spi_disable(void);
void     nrf24l01p_enable(void);
void     nrf24l01p_disable(void);

void     nrf24l01p_error_clear(void);
void     nrf24l01p_error_clear_ticks(void);
void     nrf24l01p_error_clear_buf(void);
void     nrf24l01p_error_clear_all(void);
void     nrf24l01p_error_set(uint8_t val);
uint8_t  nrf24l01p_error_get(void);
uint8_t  nrf24l01p_error_get_index(uint8_t index);
uint8_t  nrf24l01p_error_timeout(uint16_t ticks);
uint8_t  nrf24l01p_error_free(void);

uint16_t nrf24l01p_crc(uint16_t crc, uint8_t data);
uint16_t nrf24l01p_crc_block(uint8_t *buf, uint8_t len);
uint8_t  nrf24l01p_spi_transfer(uint8_t data);

void     nrf24l01p_readwrite_register(uint8_t reg, uint8_t rw, uint8_t *data, uint8_t len);
void     nrf24l01p_read_modify_write_register( uint8_t reg, uint8_t bit_pos, uint8_t bit_val );

void     nrf24l01p_flush_tx_buf(void);
void     nrf24l01p_flush_rx_buf(void);
void     nrf24l01p_write_data_tx_buf(uint8_t *data);
void     nrf24l01p_read_data_rx_buf(uint8_t *data);

uint8_t  nrf24l01p_tx_buf_empty(void);
uint8_t  nrf24l01p_rx_buf_not_empty(void);
uint8_t  nrf24l01p_wait_tx_complete(void);

void     nrf24l01p_mode_set_deep_sleep(void);
void     nrf24l01p_mode_set_sleep(void);
void     nrf24l01p_mode_set_rx(void);
void     nrf24l01p_mode_set_tx(void);
void     nrf24l01p_mode_set(uint8_t mode);
uint8_t  nrf24l01p_mode_get(void);

void     nrf24l01p_channel_set(uint8_t channel);
uint8_t  nrf24l01p_channel_get(void);

void     nrf24l01p_speed_set(uint8_t index);
uint8_t  nrf24l01p_speed_get(void);

void     nrf24l01p_tx_power_set(uint8_t index);
uint8_t  nrf24l01p_tx_power_get(void);

void     nrf24l01p_own_addr_set(uint8_t addr);
uint8_t  nrf24l01p_own_addr_get(void);
void     nrf24l01p_dest_addr_set(uint8_t addr);
uint8_t  nrf24l01p_dest_addr_get(void);
void     nrf24l01p_rxticks_set(uint16_t val);
uint16_t nrf24l01p_rxticks_get(void);
void     nrf24l01p_rx_timeout_set(uint16_t val);
uint16_t nrf24l01p_rx_timeout_get(void);
void     nrf24l01p_max_retransmission_set(uint16_t val);
uint16_t nrf24l01p_max_retransmission_get(void);
 
void     nrf24l01p_tx_basic(uint8_t *buf, uint8_t len);
uint8_t  nrf24l01p_rx_basic(uint8_t *buf);
uint8_t  nrf24l01p_tx_with_ack(uint8_t *buf, uint8_t len);
uint8_t  nrf24l01p_rx_with_ack(uint8_t *buf);


void     nrf24l01p_init(void);



