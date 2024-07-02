



void     ISP_Struct_Init(void);
void     ISP_Flush_Buffer(void);
void     ISP_Init(void);
void     ISP_Reset_MCU(void);
void     ISP_Reset_Disable(void);
uint8_t  ISP_Enter_Programming_Mode(void);

uint32_t ISP_Read_Device_Signature(void);
uint8_t  ISP_Read_MCU_Busy(void);
void     ISP_Wait_Till_MCU_Busy(void);

uint8_t  ISP_Read_Device_LFUSE(void);
uint8_t  ISP_Read_Device_HFUSE(void);
uint8_t  ISP_Read_Device_EFUSE(void);
uint8_t  ISP_Read_Device_LBITS(void);
void     ISP_Read_Device_Parameters(void);

void     ISP_Write_Device_LFUSE(uint8_t val);
void     ISP_Write_Device_HFUSE(uint8_t val);
void     ISP_Write_Device_EFUSE(uint8_t val);
void     ISP_Write_Device_LBITS(uint8_t val);

uint8_t  ISP_Write_Verify_Device_LFUSE(uint8_t val);
uint8_t  ISP_Write_Verify_Device_HFUSE(uint8_t val);
uint8_t  ISP_Write_Verify_Device_EFUSE(uint8_t val);
uint8_t  ISP_Write_Verify_Device_LBITS(uint8_t val);

void     ISP_Chip_Erase(void);
void     ISP_Write_EEPROM(uint16_t addr, uint8_t data);
uint8_t  ISP_Read_EEPROM(uint16_t addr);
void     ISP_Load_PGM_Word(uint8_t page, uint16_t data);
void     ISP_Load_PGM_Page(uint16_t *data);
void     ISP_Write_PGM_Page(uint8_t page, uint16_t *data);
uint16_t ISP_Read_Progmem(uint16_t addr);
void     ISP_Load_Buffer_With_Offset(uint16_t offset);
void     ISP_Write_Auto(void);
uint16_t ISP_Read_PGM(uint16_t addr);
uint16_t ISP_Read_Flash_Word(uint16_t addr);
uint8_t  ISP_Verify_Flash(void);

uint16_t ISP_Get_Data_Buffer(uint16_t addr);
uint8_t  ISP_Get_Device_SCK_Setting(void);
uint8_t  ISP_Get_Device_Loop_Counter(void);
uint32_t ISP_Get_Device_Signature(void);
uint8_t  ISP_Get_Fuse_Low_Byte(void);
uint8_t  ISP_Get_Fuse_High_Byte(void);
uint8_t  ISP_Get_Fuse_Extended_Byte(void);
uint8_t  ISP_Get_Lock_Byte(void);

uint8_t  ISP_Overwrite_LFUSE(void);
uint8_t  ISP_Overwrite_HFUSE(void);
uint8_t  ISP_Overwrite_EFUSE(void);
uint8_t  ISP_Overwrite_LBITS(void);

