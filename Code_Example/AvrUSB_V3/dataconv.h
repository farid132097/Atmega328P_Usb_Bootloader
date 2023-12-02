/* 
 * File:   dataconv.h
 * Author: MD. Faridul Islam
 * PIC18LF47K40 based data converter
 * Created on October 10, 2023, 7:47 AM
 */
#include <stdint.h>

uint8_t  dataconv_nibble_to_hex_char(uint8_t val);
void     dataconv_byte_to_hex_char(uint8_t val, uint8_t *array, uint8_t start);
void     dataconv_word_to_hex_char(uint16_t val, uint8_t *array, uint8_t start);
void     dataconv_dword_to_hex_char(uint32_t val, uint8_t *array, uint8_t start);
void     dataconv_dword_to_char(uint32_t val, uint8_t *array, uint8_t start);
void     dataconv_dword_to_num(int32_t val, uint8_t *array, uint8_t start);
uint8_t  dataconv_dword_to_num_len(void);
