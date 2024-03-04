#include <stdio.h>
#include <stdint.h>


#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 


void main(){

    uint8_t ADRESL, ADRESH;
    uint16_t adc_value;

    ADRESH = 0b11111111;  
    ADRESL = 0b11000000;

    printf("Leading text "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(ADRESH<<2));




}