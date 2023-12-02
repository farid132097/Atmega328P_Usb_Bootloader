
#include <avr/io.h>
#include <util/delay.h>

uint8_t dataconv_data_len=0;

uint8_t dataconv_nibble_to_hex_char(uint8_t val){
  uint8_t hex=0;
  if(val<=9){
    hex = val+48;
  }else if(val>=10 && val<=15){
    hex = val+55;
  }
  return hex;
}

void dataconv_byte_to_hex_char(uint8_t val, uint8_t *array, uint8_t start){
  array[start]= dataconv_nibble_to_hex_char((val & 0xF0)>>4);
  array[start+1]= dataconv_nibble_to_hex_char(val & 0x0F);
}

void dataconv_word_to_hex_char(uint16_t val, uint8_t *array, uint8_t start){
  uint8_t msb = val >> 8;
  uint8_t lsb = val & 0xFF;
  dataconv_byte_to_hex_char(msb, array, start);
  dataconv_byte_to_hex_char(lsb, array, start+2);
}

void dataconv_dword_to_hex_char(uint32_t val, uint8_t *array, uint8_t start){
  uint16_t msword = val >> 16;
  uint16_t lsword = val & 0xFFFF;
  dataconv_word_to_hex_char(msword, array, start);
  dataconv_word_to_hex_char(lsword, array, start+4);
}

void dataconv_dword_to_char(uint32_t val, uint8_t *array, uint8_t start){
  uint16_t msword = (val >> 16);
  uint16_t lsword = (val & 0xFFFF);
  array[start]    = (msword >> 8);
  array[start+1]  = (msword & 0xFF);
  array[start+2]  = (lsword >> 8);
  array[start+3]  = (lsword & 0xFF);
}

void dataconv_dword_to_num(int32_t val, uint8_t *array, uint8_t start){
  uint8_t i=0, digits[8], total_digits=0, array_index=0;;

  if(val<0){
    val=-val;
	array[start]='-';
	array_index++;
  }
  
  if(val==0){
    digits[0]=0;
    total_digits=1;
  }else{
    while(val!=0){
      digits[i]=val%10;
      val/=10;
      i++;
    }
	total_digits=i;
  }
  
  
  for(int k=total_digits; k>0; k--){
    uint8_t temp=k;
    temp-=1;
    temp=digits[temp];
    temp+=48;
	array[start+array_index]=temp;
	array_index++;
  }
  dataconv_data_len=array_index;
}

uint8_t dataconv_dword_to_num_len(void){
  return dataconv_data_len;
}
