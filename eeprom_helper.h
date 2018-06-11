/// contains helper functions for interacting with eeprom.

#ifndef eeprom_helperH
#define eeprom_helperH
#include<Arduino.h>
void advance_eeprom_byte_cursor(byte cursor_location, byte lowest_index, byte highest_index, byte increment);
#endif
