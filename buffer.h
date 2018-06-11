/// Controls the receiving of data through the input buffer.

#ifndef BUFFER_H
#define BUFFER_H
#include<Arduino.h>
#include"error.hpp"
const bool there_is_room_in(byte index, byte max_size);
const bool in_valid_range(byte chr);
void echo_to_screen(byte chr);
void to_upper(byte &chr);
void place_in_buffer(byte* buffer, byte& byte_index, byte new_byte);
const t_error buffer_reader(byte* buffer, byte& byte_index, byte max_buffer_size);
#endif
