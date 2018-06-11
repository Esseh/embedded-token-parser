/// Controls the 'tokenization process'. Takes buffer contents and converts it into byte tokens.

#ifndef TOKENIZATION_H
#define TOKENIZATION_H
#include<Arduino.h>
#include"error.hpp"
void get_next_indices(byte* buffer, byte index, byte &start_index, byte &end_index);
const bool is_number(byte *buffer, byte start_index, byte end_index);
const byte get_token(byte* buffer, byte start_index, byte end_index, const byte* lookup_table);
const word get_numerical_token(byte* buffer, byte start_index, byte end_index);
const t_error token_populate(
    byte* byte_buffer,
    byte byte_index,
    byte* token_buffer,
    byte& token_index,
    byte token_buffer_size,
    const byte* lookup_table
);
#endif
