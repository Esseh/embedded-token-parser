/// Contains functions useful for providing verbose debugging information. Should only be used in main in order to reduce needless coupling.

#ifndef VERBOSE_DEBUG_H
#define VERBOSE_DEBUG_H
#include<Arduino.h>
/// Outputs buffer contents as a string.
void dump_buffer_str(byte*buffer, byte index);

/// Outputs buffer contents as numerical data.
void dump_buffer(byte*buffer, byte index);

#endif
