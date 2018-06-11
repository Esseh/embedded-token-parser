#include"verbose_debug.h"

void dump_buffer_str(byte*buffer, byte index)
{
    Serial.println(F(""));
    byte i;
    for(i = 0u; i < index; i++)
    {
        Serial.print((char)buffer[i]);
    }
    Serial.println(F(""));
}
void dump_buffer(byte*buffer, byte index)
{
    Serial.println(F(""));
    byte i;
    for(i = 0u; i < index; i++)
    {
        Serial.print(buffer[i]);
        Serial.print(' ');
    }
    Serial.println(F(""));
}

