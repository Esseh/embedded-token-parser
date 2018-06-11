#include"progmem_helper.h"

void print_str(const byte * str)
{
    byte c;
    while(c = pgm_read_byte(str++))
    {
        Serial.print((char)c);
    }
}
