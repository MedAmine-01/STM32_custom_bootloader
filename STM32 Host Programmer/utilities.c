#include "main.h"


uint8_t word_to_byte(uint32_t addr, uint8_t index, uint8_t lowerfirst){
    uint8_t value = (addr>>(8*(index-1))& 0x000000FF);
    return value;
}


uint32_t get_crc(uint8_t *buff, uint32_t len)
{
    uint32_t i;

    uint32_t Crc = 0XFFFFFFFF;

    for(uint32_t n = 0 ; n < len ; n++ )
    {
        uint32_t data = buff[n];
        Crc = Crc ^ data;
        for(i=0; i<32; i++)
        {

        if (Crc & 0x80000000)
            Crc = (Crc << 1) ^ 0x04C11DB7; // Polynomial used in STM32
        else
            Crc = (Crc << 1);
        }

    }

  return(Crc);
}
