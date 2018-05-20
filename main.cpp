/// Barr Standard Reminder: Item Order
/// Comment Block
/// Includes
/// Datatypes
/// Constants
/// Macros
/// Static Declarations
/// Private Prototypes
/// Public Function Bodies
/// Private Function Bodies
#include<Arduino.h>

/// Array Bounds
#define MAX_BUFFER_SIZE 16
#define MAX_INSTRUCTION_SIZE 5

/// Token Definitions
#define tERR     0xFF
#define tEOL     0xFE
#define tokbase  0xFD
#define tD13     tokbase
#define tLED     tD13 -1
#define tRGB     tLED -1
#define tSET     tRGB -1
#define tSTATUS  tSET -1
#define tVERSION tSTATUS -1
#define tHELP    tVERSION -1
#define tON      tHELP -1
#define tOFF     tON -1
#define tBLINK   tOFF -1
#define tGREEN   tBLINK -1
#define tRED     tGREEN -1
// tLEDS is equivalent to tLED when reading in. Unreachable.
// #define tLEDS    tRED -1

byte byte_buffer_index = -1;
byte byte_buffer[MAX_BUFFER_SIZE];

byte token_buffer_index = -1;
byte token_buffer[MAX_INSTRUCTION_SIZE];

byte token_lookup[] = {
    'D', '1', tD13,
    'L', 'E', tLED,
    'S', 'E', tSET,
    'S', 'T', tSTATUS,
    'V', 'E', tVERSION,
    'H', 'E', tHELP,
    'O', 'N', tON,
    'O', 'F', tOFF,
    'B', 'L', tBLINK,
    'G', 'R', tGREEN,
    'R', 'E', tRED,
0};

void read_into_byte_buffer(
    byte*byte_buffer,
    byte& byte_buffer_index, 
    byte*token_buffer, 
    byte& token_buffer_index)
{

}

void setup()
{

}
void loop()
{

}
