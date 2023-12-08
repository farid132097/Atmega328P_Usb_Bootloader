

uint8_t  EEPROM_Read(uint16_t addr);
void     EEPROM_Write(uint16_t addr, uint8_t data);
void     EEPROM_Erase(uint16_t addr);
uint16_t EEPROM_Read_Word(uint16_t addr);
void     EEPROM_Write_Word(uint16_t addr, uint16_t data);
uint32_t EEPROM_Read_Dword(uint16_t addr);
void     EEPROM_Write_Dword(uint16_t addr, uint32_t data);
void     EEPROM_Erase_Region(uint16_t start_addr, uint16_t stop_addr);
void     EEPROM_Clear_Data_Space(void);
void     EEPROM_Init(void);