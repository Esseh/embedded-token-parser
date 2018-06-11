#include"buffer.h"

const bool there_is_room_in(byte index, byte max_size)
{
    return index < max_size;
}


const bool in_valid_range(byte chr)
{
    return ((chr >= 'A') && (chr <= 'Z')) || ((chr >= '0') && (chr <= '9')) || (13u == chr) || (' ' == chr);
}


void echo_to_screen(byte chr)
{
    switch(chr)
    {
        case 13u:
            Serial.println(F(""));
        break;
        case 127u:
            Serial.print(F("\b \b"));
        break;
        default:
            if(in_valid_range(chr))
            {
                 Serial.print((char)chr);
            }
        break;
    }
}

void to_upper(byte &chr)
{
    if(chr >= 'a' && chr <= 'z')
    {
        chr -= ('a' - 'A'); 
    }
}

void place_in_buffer(byte* buffer, byte& byte_index, byte new_byte)
{
    buffer[byte_index] = new_byte;
    byte_index++;
}

const t_error buffer_reader(byte* buffer, byte& byte_index, byte max_buffer_size)
{
    t_error err = ERROR_NOT_READY;
    if(there_is_room_in(byte_index, max_buffer_size))
    {
        if(Serial.available())
        {
            byte new_byte = Serial.read();
            to_upper(new_byte);
            echo_to_screen(new_byte);
            if(in_valid_range(new_byte))
            {
                place_in_buffer(buffer, byte_index, new_byte);
            }
            if(13u == new_byte)
            {
                err = NO_ERROR;
            }
            else if((127u == new_byte) && (byte_index > 0u))
            {
                byte_index--;
            }
        }
    }
    else
    {
        err = ERROR_GENERIC;
    }
    return err;
}

