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

/// Disable / Enable Debug
#if 0
#define NDEBUG
#endif
/// Arduino Configuration
#define BAUD 115200

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

// Determines if it is time to perform tokenization.
bool command_flag = false;

// State Variables
bool s_d13_on    = false;    // D13 on or off
bool s_d13_blink = false;    // D13 blinking or not
bool s_led_red   = true;     // true = red , false  = green
bool s_led_on    = false;    // Dual LED on or off
bool s_led_blink = false;    // Dual LED blinking or not
byte s_rgb_red   = 0;        // RGB red intensity
byte s_rgb_green = 0;        // RGB green intensity
byte s_rgb_blue  = 0;        // RGB blue intensity
word s_blink_time= 500;      // interval between blinks

// Buffers
byte byte_buffer_index = 0;
byte byte_buffer[MAX_BUFFER_SIZE];

byte token_buffer_index = 0;
byte token_buffer[MAX_INSTRUCTION_SIZE];

// Lookup Table
byte token_lookup[] = {
    'D', '1', tD13,
    'R', 'G', tRGB,
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
    13 , 13 , tEOL,
0};

/// Debug Functions
#ifndef NDEBUG
/// Dumps state of buffer.
void dump_buffer_state (
    byte*byte_buffer,
    byte byte_buffer_index, 
    bool command_flag
){
    Serial.println("");
    Serial.print("Buffer Contents: ");
    byte i;
    for(i = 0; i < byte_buffer_index; i++)
    {
        if('\0' == byte_buffer[i])
        {
             Serial.print("\\0");
        }
        else
        {
             Serial.print((char)(byte_buffer[i]));
        }
    }
    Serial.println("");
    Serial.print("Buffer Index: ");
    Serial.print(byte_buffer_index);
    Serial.println("");
    Serial.print("Max Buffer Size: ");
    Serial.print(MAX_BUFFER_SIZE);
    Serial.println("");
    Serial.print("Command Flag State: ");
    Serial.print(command_flag);
    Serial.println("");
}

/// Dumps state of buffer without assuming characters.
void dump_buffer_state2 (
    byte*byte_buffer,
    byte byte_buffer_index, 
    bool command_flag
){
    Serial.println("");
    Serial.print("Buffer Contents: ");
    byte i;
    for(i = 0; i < byte_buffer_index; i++)
    {
        Serial.print((byte_buffer[i]));
        Serial.print(' ');
    }
    Serial.println("");
    Serial.print("Buffer Index: ");
    Serial.print(byte_buffer_index);
    Serial.println("");
    Serial.print("Max Buffer Size: ");
    Serial.print(MAX_BUFFER_SIZE);
    Serial.println("");
    Serial.print("Command Flag State: ");
    Serial.print(command_flag);
    Serial.println("");
}

/// Dumps the entire global program state.
void dump_state ()
{
    Serial.println("");
    Serial.print("D13 ON: ");
    Serial.print(s_d13_on);
    Serial.println("");
    Serial.print("D13 BLINK: ");
    Serial.print(s_d13_blink);
    Serial.println("");
    Serial.print("LED RED: ");
    Serial.print(s_led_red);
    Serial.println("");
    Serial.print("LED ON: ");
    Serial.print(s_led_on);
    Serial.println("");
    Serial.print("LED BLINK: ");
    Serial.print(s_led_blink);
    Serial.println("");
    Serial.print("RGB RED: ");
    Serial.print(s_rgb_red);
    Serial.println("");
    Serial.print("RGB GREEN: ");
    Serial.print(s_rgb_green);
    Serial.println("");
    Serial.print("RGB BLUE: ");
    Serial.print(s_rgb_blue);
    Serial.println("");
    Serial.print("BLINK TIME: ");
    Serial.print(s_blink_time);
    Serial.println("");
}
#endif

/// reads any available bytes into the input buffer and sets the command flag if it is ready to parse.
void read_into_byte_buffer (
    byte*byte_buffer,
    byte& byte_buffer_index, 
    bool& command_flag
)
{
    if(Serial.available())
    {
        /// Get new character to read in.
        byte new_byte = Serial.read();

        /// Echo read to screen.
        Serial.print(new_byte);

        /// If no actual room, error state.
        if(byte_buffer_index >= MAX_BUFFER_SIZE)
        {
            Serial.println("Buffer Size Exceeded, Buffer Dumped");
            byte_buffer_index = -1;
            command_flag = false;
        }
        /// If RC then set the command flag to signal tokenization.
        else if(13 == new_byte)
        {
            Serial.println("");
            command_flag = true;
            byte_buffer[byte_buffer_index] = '\0';
        }
        else
        {
        /// Otherwise load byte into buffer
            if(' ' == new_byte || '\t' == new_byte)
            /// White space, load null terminator.
	    {
                byte_buffer[byte_buffer_index] = '\0';
	    }
            else
            /// General Case, load new character into buffer.
            {
                byte_buffer[byte_buffer_index] = new_byte;
            }
	    /// Prepare for another character in buffer
        }
	byte_buffer_index++;
        #ifndef NDEBUG
        Serial.println("BYTE BUFFER STATE");
        dump_buffer_state(byte_buffer,byte_buffer_index,command_flag);
        #endif
    }
}

// Retreives a token from the lookup table.
byte get_token(byte hint1, byte hint2,byte*lookup_table)
{
    byte lookup_iterator;
    for(lookup_iterator = 0; lookup_table[lookup_iterator] != 0; lookup_iterator += 3)
    {
        if((hint1 == lookup_table[lookup_iterator]) && (hint2 == lookup_table[lookup_iterator+1]))
        {
            return lookup_table[lookup_iterator+2];
        }
    }
    return tERR;
}

/// converts all available substrings in input buffer into byte tokens.
void tokenize (byte*token_buffer,byte& token_buffer_index, byte*byte_buffer, byte& byte_buffer_index, byte*lookup_table, bool& command_flag)
{
    byte byte_buffer_iterator;
    byte_buffer_iterator = 0;
    while(byte_buffer_iterator < byte_buffer_index)
    {
        // First character is null case.
        if(byte_buffer[byte_buffer_iterator] == '\0')
        {
            byte_buffer_iterator++;
        }
        // General case
        else
        {
            // no room, error state
            if(token_buffer_index >= MAX_INSTRUCTION_SIZE)
            {
                Serial.println("Instruction is too large");
                byte_buffer_index = 0;
                token_buffer_index = 0;
                command_flag = false;
                return;
            }
            // read in new token into token buffer.
            byte new_token = get_token(
                byte_buffer[byte_buffer_iterator],
                byte_buffer[byte_buffer_iterator+1],
                lookup_table
            );
            token_buffer[token_buffer_index] = new_token;
            token_buffer_index++;
            while(byte_buffer[byte_buffer_iterator] != '\0')
            {
                byte_buffer_iterator++;
            }
            #ifndef NDEBUG
            Serial.println("TOKEN BUFFER STATE");
            dump_buffer_state2(token_buffer, token_buffer_index, command_flag);
            #endif
        }
    }
    // no room, error state
    if(token_buffer_index >= MAX_INSTRUCTION_SIZE)
    {
        Serial.println("Instruction is too large");
        byte_buffer_index = 0;
        token_buffer_index = 0;
        command_flag = false;
        return;
    }
    else
    {
        // append EOL and clear input buffer.
        token_buffer[token_buffer_index] = get_token(13,13,lookup_table);
        token_buffer_index++;
        byte_buffer_index = 0;
        command_flag = false;
    }
    #ifndef NDEBUG
    Serial.println("TOKEN BUFFER STATE");
    dump_buffer_state2(token_buffer, token_buffer_index, command_flag);
    #endif
}

void setup()
{
    Serial.begin(BAUD);
    Serial.println("Press Enter to begin");
    while(!Serial.available() && (13 != Serial.read()))
    {
       /// Wait for Serial I/O to be ready.
    }
    Serial.println("Process Started...");
}
void loop()
{
    if(true == command_flag)
    {
        tokenize(
            token_buffer, 
            token_buffer_index, 
            byte_buffer, 
            byte_buffer_index, 
            token_lookup, 
            command_flag
        );
        #ifndef NDEBUG
        dump_state();
        #endif
        token_buffer_index = 0;
    }
    else
    {
        read_into_byte_buffer(byte_buffer,byte_buffer_index,command_flag);
    }
}
