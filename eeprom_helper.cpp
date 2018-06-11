#include"eeprom_helper.h"
#include<EEPROM.h>

void advance_eeprom_byte_cursor(byte cursor_location, byte lowest_index, byte highest_index, byte increment)
{
    byte inx = EEPROM.read(cursor_location);
    inx -= lowest_index;
    inx += increment;
    byte reduced_upper_bound = highest_index - lowest_index;
    inx %= reduced_upper_bound;
    inx += lowest_index;
    EEPROM.write(cursor_location, inx);
}

