

uint8_t  Data_Conv_Nibble_To_Hex_Char(uint8_t val);
void     Data_Conv_Byte_To_Hex_Char(uint8_t val, uint8_t *array, uint8_t start);
void     Data_Conv_Word_To_Hex_Char(uint16_t val, uint8_t *array, uint8_t start);
void     Data_Conv_Dword_To_Hex_Char(uint32_t val, uint8_t *array, uint8_t start);
void     Data_Conv_Dword_To_Byte(uint32_t val, uint8_t *array, uint8_t start);
uint8_t  Data_Conv_Hex_Char_To_Nibble(uint8_t hex_char);
uint8_t  Data_Conv_Hex_Char_To_Byte(uint8_t *hex_char, uint8_t start);
uint16_t Data_Conv_Hex_Char_To_Word(uint8_t *hex_char, uint8_t start);