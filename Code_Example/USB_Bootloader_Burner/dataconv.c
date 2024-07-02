
#include <avr/io.h>
#include <util/delay.h>
#include "dataconv.h"



uint8_t Data_Conv_Nibble_To_Hex_Char(uint8_t val){
  uint8_t temp=0;
  if(val<=9){
    temp=val+48;
  }else if(val>=10 && val<=15){
    temp=val+55;
  }
  return temp;
}

void Data_Conv_Byte_To_Hex_Char(uint8_t val, uint8_t *array, uint8_t start){
  array[start]= Data_Conv_Nibble_To_Hex_Char((val & 0xF0)>>4);
  array[start+1]= Data_Conv_Nibble_To_Hex_Char(val & 0x0F);
}


void Data_Conv_Word_To_Hex_Char(uint16_t val, uint8_t *array, uint8_t start){
  uint8_t msb = val >> 8;
  uint8_t lsb = val & 0xFF;
  Data_Conv_Byte_To_Hex_Char(msb, array, start);
  Data_Conv_Byte_To_Hex_Char(lsb, array, start+2);
}

void Data_Conv_Dword_To_Hex_Char(uint32_t val, uint8_t *array, uint8_t start){
  uint16_t msword = val >> 16;
  uint16_t lsword = val & 0xFFFF;
  Data_Conv_Word_To_Hex_Char(msword, array, start);
  Data_Conv_Word_To_Hex_Char(lsword, array, start+4);
}

void Data_Conv_Dword_To_Byte(uint32_t val, uint8_t *array, uint8_t start){
  uint16_t msword = (val >> 16);
  uint16_t lsword = (val & 0xFFFF);
  array[start]    = (msword >> 8);
  array[start+1]  = (msword & 0xFF);
  array[start+2]  = (lsword >> 8);
  array[start+3]  = (lsword & 0xFF);
}

uint8_t Data_Conv_Hex_Char_To_Nibble(uint8_t hex_char){
  if( (hex_char >= 48) && (hex_char <= 57) ){
    return (hex_char - 48);
  }else if( (hex_char >= 65) && (hex_char <= 70) ){
    return (hex_char - 55);
  }else if( (hex_char >= 97) && (hex_char <= 102) ){
    return (hex_char - 87);
  }else{
    return 0x0F;
  }
}

uint8_t Data_Conv_Hex_Char_To_Byte(uint8_t *hex_char, uint8_t start){
  uint8_t temp = 0;
  temp =  Data_Conv_Hex_Char_To_Nibble(hex_char[start]);
  temp <<= 4;
  temp |= Data_Conv_Hex_Char_To_Nibble(hex_char[start+1]);
  return temp;
}

uint16_t Data_Conv_Hex_Char_To_Word(uint8_t *hex_char, uint8_t start){
  uint16_t temp = 0;
  temp = Data_Conv_Hex_Char_To_Byte(hex_char, start);
  temp <<= 8;
  temp |= Data_Conv_Hex_Char_To_Byte(hex_char, start+2);
  return temp;
}






