#include"tokenization.h"
#include"token.hpp"
#include"buffer.h"
void get_next_indices(byte* buffer, byte index, byte &start_index, byte &end_index)
{
    start_index = end_index;
    while((' ' == buffer[start_index]) || (13u == buffer[start_index]))
    {
        start_index++;
    }
    end_index = start_index;
    while((buffer[end_index] != 13u) && (buffer[end_index] != ' ') && (end_index < index))
    {
        end_index++;
    }
}

const bool is_number(byte *buffer, byte start_index, byte end_index)
{
    bool result = true;
    if(buffer[0u] == '0')
    {
        result = false;
    }
    byte i;
    for(i = start_index; i < end_index; i++)
    {
        if(buffer[i] < '0' || buffer[i] > '9')
        {
            result = false;
        }
    }
    return result;
}

const byte get_token(byte* buffer, byte start_index, byte end_index, const byte* lookup_table)
{
    byte lookup_iterator;
    for(lookup_iterator = 0u; pgm_read_byte_near(lookup_table + lookup_iterator) != 0u; lookup_iterator += 4u)
    {
        byte size = (end_index - start_index);
        byte hint1 = buffer[start_index];
        byte hint2;
        if(start_index == end_index)
        {
            hint2 = hint1;
        }
        else
        {
            hint2 = buffer[start_index+1u];
        }
        hint2 = buffer[start_index + 1u];
        if(
            (hint1 == pgm_read_byte_near(lookup_table + lookup_iterator)) 
            && (hint2 == pgm_read_byte_near(lookup_table + lookup_iterator + 1u))
            && (size == pgm_read_byte_near(lookup_table + lookup_iterator + 2u))
        )
        {
            return pgm_read_byte_near(lookup_table + lookup_iterator + 3u);
        }
    }
    return tERR;
}

const word get_numerical_token(byte* buffer, byte start_index, byte end_index)
{
    word prospective_token = 0;
    byte i;
    for(i = start_index; i < end_index; i++)
    {
        prospective_token *= 10;
        prospective_token += (buffer[i] - '0');
    }
    return prospective_token;
}

const t_error token_populate(
    byte* byte_buffer,
    byte byte_index,
    byte* token_buffer,
    byte& token_index,
    byte token_buffer_size,
    const byte* lookup_table
)
{
    t_error err = NO_ERROR;
    byte start_index;
    byte end_index = 0u;
    while(end_index < byte_index)
    {    
        get_next_indices(byte_buffer, byte_index, start_index, end_index);
        if(end_index >= byte_index)
        {
            break;
        }
        if(there_is_room_in(token_index, token_buffer_size))
        {
            byte new_token;
            if(is_number(byte_buffer,start_index, end_index))
            {
                word large_token = get_numerical_token(byte_buffer, start_index, end_index);
                if(large_token > 255u)
                {
                    place_in_buffer(token_buffer, token_index, large_token >> 8u);
                }
                new_token = large_token & 0x00FF;
                if(!there_is_room_in(token_index, token_buffer_size))
                {
                    err = ERROR_GENERIC;
                    continue;
                }
            }
            else
            {
                new_token = get_token(byte_buffer, start_index, end_index, lookup_table);
            }
            place_in_buffer(token_buffer, token_index, new_token);
        }
        else
        {
            err = ERROR_GENERIC;
        }
    }
    if(!there_is_room_in(token_index, token_buffer_size))
    {
        err = ERROR_GENERIC;
    }
    else
    {
        place_in_buffer(token_buffer, token_index, tEOL);
    }
    return err;
}

