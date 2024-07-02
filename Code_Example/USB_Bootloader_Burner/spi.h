
#define  SCK_DDR       DDRB 
#define  SCK_PORT      PORTB  
#define  SCK_bp        5      

#define  MISO_DDR      DDRB   
#define  MISO_PORT     PORTB
#define  MISO_PIN      PINB 
#define  MISO_bp       4      

#define  MOSI_DDR      DDRB   
#define  MOSI_PORT     PORTB  
#define  MOSI_bp       3 

#define  RESET_DDR     DDRB
#define  RESET_PORT    PORTB
#define  RESET_bp      2 

void     SPI_Struct_Init(void);
void     SPI_SCK_Input(void);
void     SPI_SCK_Output(void);
void     SPI_SCK_Set_Output_State(uint8_t state);
void     SPI_MISO_Input(void);
void     SPI_MISO_Output(void);
void     SPI_MISO_Set_Output_State(uint8_t state);
uint8_t  SPI_MISO_Get_State(void);
void     SPI_MOSI_Input(void);
void     SPI_MOSI_Output(void);
void     SPI_MOSI_Set_Output_State(uint8_t state);
void     SPI_RESET_Input(void);
void     SPI_RESET_Output(void);
void     SPI_RESET_Set_Output_State(uint8_t state);
void     SPI_Reset_Set_Input_Pull_Up(void);
void     SPI_Reset_State(void);

void     SPI_GPIO_Init(void);
void     SPI_Delay_Init(void);
void     SPI_Set_SCK_Frequency_HW(uint8_t sck_step);
void     SPI_Config_Register(uint8_t sck_step);
void     SPI_SW_Delay_Bit(void);
void     SPI_Init(uint8_t sck_step);
void     SPI_Init_Clock(uint8_t sck_step);
uint8_t  SPI_Transceive_HW(uint8_t data);
uint8_t  SPI_Transceive_SW(uint8_t data);
uint8_t  SPI_Transceive(uint8_t data);
uint8_t  SPI_Get_Error(void);
uint8_t  SPI_Get_Ticks(void);
