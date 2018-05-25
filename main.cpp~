#include<Arduino.h>
#include<Adafruit_NeoPixel.h>

/// Version Information
#define VERSION "v0.4"

/// Disable / Enable Debug
#if 1
#define NDEBUG
#endif

/// Arduino Configuration
#define BAUD 115200
#define TIME_QUANTUM 50
#define DUAL_RED_PIN 9
#define DUAL_GREEN_PIN 8
#define ON_BOARD_LED_PIN 13
#define RGB_PIN 10

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

/// Debug Strings
#ifndef NDEGBUG

#endif

// Global Library Variables
Adafruit_NeoPixel strip;

/// Bit Indexed State Flags
/// 0 - command_flag
/// 1 - s_d13_on
/// 2 - s_d13_blink
/// 3 - s_d13_blink_toggle
/// 4 - s_led_red
/// 5 - s_led_on
/// 6 - s_led_blink
/// 7 - s_led_blink_toggle
#define SET_BIT(set,index,value) set = (set ^ (1 << index - 1))
#define GET_BIT(set,index) ((set >> index) & 1)
byte s_flags_1;

// Determines if it is time to perform tokenization.
bool command_flag = false;

// State Variables
bool s_d13_on    = false;    // D13 on or off
bool s_d13_blink = false;    // D13 blinking or not
bool s_d13_blink_toggle = false;
bool s_led_red   = false;     // true = red , false  = green
bool s_led_on    = false;    // Dual LED on or off
bool s_led_blink = false;    // Dual LED blinking or not
bool s_led_blink_toggle = false;
byte s_rgb_red   = 0;        // RGB red intensity
byte s_rgb_green = 0;        // RGB green intensity
byte s_rgb_blue  = 0;        // RGB blue intensity
word s_blink_time= 500;      // interval between blinks

// Control Variables
byte c_main = 0;

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


/// Prints out a flash stored string
#define PRINT_MAPLN_CASE(id,str) case id : Serial.println(F(str)); break
#define PRINT_MAP_CASE(id,str) case id : Serial.print(F(str)); break
#define BUFFER_CONTENT 0
#define NEWLINE BUFFER_CONTENT +1
#define BUFFER_INDEX NEWLINE +1
#define BUFFER_SIZE BUFFER_INDEX +1
#define COMMAND_FLAG_STATE BUFFER_SIZE +1
#define D13_ON COMMAND_FLAG_STATE +1
#define D13_BLINK D13_ON +1
#define LED_RED D13_BLINK +1
#define LED_ON LED_RED +1
#define LED_BLINK LED_ON +1
#define RGB_RED LED_BLINK +1
#define RGB_GREEN RGB_RED +1
#define RGB_BLUE RGB_GREEN +1
#define BLINK_TIME RGB_BLUE +1
#define BUFFER_EXCEEDED BLINK_TIME +1
#define NULL_TERMINATOR BUFFER_EXCEEDED +1
#define INSTRUCTION_TOO_LARGE NULL_TERMINATOR +1
#define UNRECOGNIZED_INPUT INSTRUCTION_TOO_LARGE +1
void print_map(int id)
{
    switch(id)
    {
        PRINT_MAPLN_CASE(NEWLINE,"");
        #ifndef NDEFBUG
        PRINT_MAP_CASE(BUFFER_CONTENT,"Buffer Contents: ");
        PRINT_MAP_CASE(BUFFER_INDEX, "Buffer Index: ");
        PRINT_MAP_CASE(BUFFER_SIZE, "Max Buffer Size: ");
        PRINT_MAP_CASE(COMMAND_FLAG_STATE, "Command Flag State: ");
        #endif
        PRINT_MAP_CASE(D13_ON,"D13 ON: ");
        PRINT_MAP_CASE(D13_BLINK,"D13 BLINK: ");
        PRINT_MAP_CASE(LED_RED, "LED RED: ");
        PRINT_MAP_CASE(LED_ON, "LED ON: ");
        PRINT_MAP_CASE(LED_BLINK, "LED BLINK: ");
        PRINT_MAP_CASE(RGB_RED, "RGB RED: ");
        PRINT_MAP_CASE(RGB_GREEN, "RGB GREEN: ");
        PRINT_MAP_CASE(RGB_BLUE, "RGB BLUE: ");
        PRINT_MAP_CASE(BLINK_TIME, "BLINK TIME: ");
        PRINT_MAPLN_CASE(BUFFER_EXCEEDED, "Buffer Size Exceeded, Buffer Dumped");
        PRINT_MAP_CASE(NULL_TERMINATOR, "\\0");
        PRINT_MAPLN_CASE(INSTRUCTION_TOO_LARGE, "Instruction is too large");
        PRINT_MAPLN_CASE(UNRECOGNIZED_INPUT, "Unrecognized Input");
    }
}

/// Debug Functions
#ifndef NDEBUG
/// Dumps state of buffer.
void dump_buffer_state (
    byte*byte_buffer,
    byte byte_buffer_index, 
    bool command_flag
){
    print_map(NEWLINE);
    print_map(BUFFER_CONTENT);
    byte i;
    for(i = 0; i < byte_buffer_index; i++)
    {
        if('\0' == byte_buffer[i])
        {
             print_map(NULL_TERMINATOR);
        }
        else
        {
             Serial.print((char)(byte_buffer[i]));
        }
    }
    print_map(NEWLINE);
    Serial.print(F());
    print_map(BUFFER_INDEX);
    print_map(NEWLINE);
    print_map(BUFFER_SIZE);
    Serial.print(MAX_BUFFER_SIZE);
    print_map(NEWLINE);
    print_map(COMMAND_FLAG_STATE);
    Serial.print(command_flag);
    print_map(NEWLINE);
}

/// Dumps state of buffer without assuming characters.
void dump_buffer_state2 (
    byte*byte_buffer,
    byte byte_buffer_index, 
    bool command_flag
){
    print_map(NEWLINE);
    print_map(BUFFER_CONTENT);
    byte i;
    for(i = 0; i < byte_buffer_index; i++)
    {
        Serial.print((byte_buffer[i]));
        Serial.print(' ');
    }
    print_map(NEWLINE);
    print_map(BUFFER_INDEX)
    Serial.print(byte_buffer_index);
    print_map(NEWLINE);
    print_map(BUFFER_SIZE);
    Serial.print(MAX_BUFFER_SIZE);
    print_map(NEWLINE);
    print_map(COMMAND_FLAG_STATE)
    Serial.print(command_flag);
    print_map(NEWLINE);
}
#endif

/// Dumps the entire global program state.
void dump_state ()
{
    print_map(NEWLINE);
    print_map(D13_ON);
    Serial.print(s_d13_on);

    print_map(NEWLINE);
    print_map(D13_BLINK);
    Serial.print(s_d13_blink);
    print_map(NEWLINE);
    print_map(LED_RED);
    Serial.print(s_led_red);

    print_map(NEWLINE);
    print_map(LED_ON);
    Serial.print(s_led_on);

    print_map(NEWLINE);
    print_map(LED_BLINK);
    Serial.print(s_led_blink);

    print_map(NEWLINE);
    print_map(RGB_RED);
    Serial.print(s_rgb_red);

    print_map(NEWLINE);
    print_map(RGB_GREEN);
    Serial.print(s_rgb_green);

    print_map(NEWLINE);
    print_map(RGB_BLUE);
    Serial.print(s_rgb_blue);

    print_map(NEWLINE);
    print_map(BLINK_TIME);
    Serial.print(s_blink_time);

    print_map(NEWLINE);
}

/// reads any available bytes into the input buffer and sets the command flag if it is ready to parse.
void read_into_byte_buffer ()
{
    if(Serial.available())
    {
        byte new_byte = Serial.read();

        // Echo to screen.
        Serial.print((char)new_byte);

        /// If no actual room, error state.
        if(byte_buffer_index >= MAX_BUFFER_SIZE)
        {
            print_map(BUFFER_EXCEEDED);
            byte_buffer_index = -1;
            command_flag = false;
        }
        /// If RC then set the command flag to signal tokenization.
        else if(13 == new_byte)
        {
            Serial.println(F(""));
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
void tokenize ()
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
                print_map(INSTRUCTION_TOO_LARGE);
                byte_buffer_index = 0;
                token_buffer_index = 0;
                command_flag = false;
                return;
            }
            // read in new token into token buffer.
            byte new_token = get_token(
                byte_buffer[byte_buffer_iterator],
                byte_buffer[byte_buffer_iterator+1],
                token_lookup
            );
            
            /// tERR but it's a number case.
            if(
            (tERR == new_token) 
            && (byte_buffer[byte_buffer_iterator] >= '0') 
            && (byte_buffer[byte_buffer_iterator] <= '9')
            )
            {
                word prospective_token;
                bool num_token_err = false;
                prospective_token = 0;
                while(byte_buffer[byte_buffer_iterator] != '\0')
                {
                    prospective_token *= 10;
                    prospective_token += (byte_buffer[byte_buffer_iterator] - '0');
                    if(
                        (byte_buffer[byte_buffer_iterator] < '0') 
                        || (byte_buffer[byte_buffer_iterator] > '9')
                    )
                    {
                        num_token_err = true;
                    }
                    byte_buffer_iterator++;
                    #ifndef NDEBUG
                    Serial.print(F("BUILDING NUMERICAL TOKEN: "));
                    Serial.print(prospective_token);
                    Serial.println(F(""));
                    Serial.print(F("TOKEN ERROR STATE: "));
                    Serial.print(num_token_err);
                    Serial.println(F(""));
                    #endif
                }
                /// Attempt numerical token so long as it is a valid number.
                if(!num_token_err)
                {
                    /// Word Size Case, insert first byte then set token to second byte.
                    if(prospective_token > 255)
                    {
                        // no extra room, error state
                        if((token_buffer_index + 1) >= MAX_INSTRUCTION_SIZE)
                        {
                            print_map(INSTRUCTION_TOO_LARGE);
                            byte_buffer_index = 0;
                            token_buffer_index = 0;
                            command_flag = false;
                            return;
                        }

                        // pre-emptively insert left 8 bits of word.
                        byte left_byte;
                        left_byte = prospective_token >> 8;             
                        token_buffer[token_buffer_index] = left_byte;

                        token_buffer_index++;

                        // set token from tERR to right 8 bits of word.
                        byte right_byte;
                        right_byte = ((prospective_token) << 8) >> 8;
                        new_token = right_byte;
                    }
                    /// Byte Size Case, set token to created byte. No need for transformation.
                   else
                    {
                        new_token = prospective_token;
                    }
                }
            }

            token_buffer[token_buffer_index] = new_token;
            token_buffer_index++;
            while(byte_buffer[byte_buffer_iterator] != '\0')
            {
                byte_buffer_iterator++;
            }
            #ifndef NDEBUG
            Serial.println(F("TOKEN BUFFER STATE"));
            dump_buffer_state2(token_buffer, token_buffer_index, command_flag);
            #endif
        }
    }
    // no room, error state
    if(token_buffer_index >= MAX_INSTRUCTION_SIZE)
    {
        print_map(INSTRUCTION_TOO_LARGE);
        byte_buffer_index = 0;
        token_buffer_index = 0;
        command_flag = false;
        return;
    }
    else
    {
        // append EOL and clear input buffer.
        token_buffer[token_buffer_index] = get_token(13,13,token_lookup);
        token_buffer_index++;
        byte_buffer_index = 0;
        command_flag = false;
    }
    #ifndef NDEBUG
    Serial.println(F("TOKEN BUFFER STATE"));
    dump_buffer_state2(token_buffer, token_buffer_index, command_flag);
    #endif
}

void parse_input()
{
    if(tEOL == token_buffer[1])
    {
        if(tVERSION == token_buffer[0])
        {
            #ifndef NDEBUG
            Serial.println(F("VERSION > EOL"));
            #endif

            Serial.println(F(VERSION));
        }
        else if(tHELP == token_buffer[0])
        {
            #ifndef NDEBUG
            Serial.println(F("HELP > EOL"));
            #endif

            Serial.println(F("D13 (ON | OFF | BLINK)"));
            Serial.println(F("LED (GREEN | RED | OFF | BLINK)"));
            Serial.println(F("RGB <0-255> <0-255> <0-255>"));
            Serial.println(F("SET BLINK <0-65535>"));
            Serial.println(F("STATUS LEDS"));
            Serial.println(F("VERSION"));
            Serial.println(F("HELP"));
        }
        else
        {
            print_map(UNRECOGNIZED_INPUT);
        }
    }
    else if(tEOL == token_buffer[2])
    {
        if((tD13 == token_buffer[0]) && (tON == token_buffer[1]))
        {
            #ifndef NDEBUG
            Serial.println(F("D13 > ON > EOL"));
            #endif
                
            s_d13_on    = true;
            s_d13_blink = false;
        }
        else if((tD13 == token_buffer[0]) && (tOFF == token_buffer[1]))
        {
            #ifndef NDEBUG
            Serial.println(F("D13 > OFF > EOL"));
            #endif
                
            s_d13_on    = false;
            s_d13_blink = false;
        }
        else if((tD13 == token_buffer[0]) && (tBLINK == token_buffer[1]))
        {
            #ifndef NDEBUG
            Serial.println(F("D13 > BLINK > EOL"));
            #endif
                
            s_d13_on    = true;
            s_d13_blink = true;
        }
        else if((tSTATUS == token_buffer[0]) && (tLED == token_buffer[1]))
        {
            #ifndef NDEBUG
            Serial.println(F("STATUS > LEDS > EOL"));
            #endif

            dump_state();
        }
        else if((tLED == token_buffer[0]) && (tGREEN == token_buffer[1]))
        {
            #ifndef NDEBUG
            Serial.println(F("LED > GREEN > EOL"));
            #endif

            s_led_red = false;
            s_led_on  = true;
            s_led_blink = false;        
        }
        else if((tLED == token_buffer[0]) && (tRED == token_buffer[1]))
        {
            #ifndef NDEBUG
            Serial.println(F("LED > RED > EOL"));
            #endif

            s_led_red   = true;
            s_led_on    = true;
            s_led_blink = false;        
        }
        else if((tLED == token_buffer[0]) && (tOFF == token_buffer[1]))
        {
            #ifndef NDEBUG
            Serial.println(F("LED > OFF > EOL"));
            #endif

            s_led_on    = false;
            s_led_blink = false;      
        }
        else if((tLED == token_buffer[0]) && (tBLINK == token_buffer[1]))
        {
            #ifndef NDEBUG
            Serial.println(F("LED > BLINK > EOL"));
            #endif

            s_led_on    = true;
            s_led_blink = true;
        }
        else
        {
            print_map(UNRECOGNIZED_INPUT);
        }
    }
    else if(tEOL == token_buffer[3])
    {
        if((tSET == token_buffer[0]) && (tBLINK == token_buffer[1]))
        {
            #ifndef NDEBUG
            Serial.println(F("SET > BLINK"));
            #endif
            s_blink_time = token_buffer[2];
        }
        else
        {
            print_map(UNRECOGNIZED_INPUT);
        }
    }
    else if(tEOL == token_buffer[4])
    {
        if((tSET == token_buffer[0]) && (tBLINK == token_buffer[1]))
        {
            #ifndef NDEBUG
            Serial.println(F("SET > BLINK"));
            #endif
            s_blink_time = (token_buffer[2] << 8) + token_buffer[3];
        }
        else if(tRGB == token_buffer[0])
        {
            #ifndef NDEBUG
            Serial.println(F("RGB > num num num"));
            #endif
            s_rgb_red   = token_buffer[1];
            s_rgb_green = token_buffer[2];
            s_rgb_blue  = token_buffer[3];
        }
        else
        {
            print_map(UNRECOGNIZED_INPUT);
        }
    }
    else if(tEOL == token_buffer[0])
    {
        /* Do nothing if only EOL */
    }
    else
    {
        print_map(UNRECOGNIZED_INPUT);
    }
    token_buffer_index = 0;   
    command_flag = false;
    #ifndef NDEBUG
    dump_state();
    #endif

}

void t_d13 ()
{
    if(s_d13_blink)
    {
        static unsigned long int d13_passed_time = millis();
        if((millis() - d13_passed_time) > s_blink_time)
        {
            digitalWrite(ON_BOARD_LED_PIN,s_d13_blink_toggle);
            s_d13_blink_toggle = !s_d13_blink_toggle;
            d13_passed_time = millis();
        }
    }
    bool blink_factor = !s_d13_blink || s_d13_blink_toggle;
    digitalWrite(ON_BOARD_LED_PIN,s_d13_on*blink_factor);
}

void t_led()
{
    if(s_led_blink)
    {
        static unsigned long int led_passed_time = millis();
        if((millis() - led_passed_time) > s_blink_time)
        {
            s_led_blink_toggle = !s_led_blink_toggle;
            led_passed_time = millis();
        }
    }
    bool blink_factor = !s_led_blink || s_led_blink_toggle; 
    digitalWrite(DUAL_RED_PIN,s_led_red*s_led_on*blink_factor);
    digitalWrite(DUAL_GREEN_PIN,!s_led_red*s_led_on*blink_factor);
}

void t_rgb()
{
    strip.setPixelColor(0, s_rgb_red, s_rgb_green, s_rgb_blue);
    strip.show();
}

void setup()
{
    pinMode(DUAL_RED_PIN,OUTPUT);
    pinMode(DUAL_GREEN_PIN,OUTPUT);
    pinMode(ON_BOARD_LED_PIN,OUTPUT);
    strip = Adafruit_NeoPixel(1, RGB_PIN, NEO_GRB + NEO_KHZ800);
    strip.begin();
    strip.show();
    Serial.begin(BAUD);
    Serial.println(F("Press Enter to begin"));
    while(!Serial.available() && (13 != Serial.read()))
    {
       /// Wait for Serial I/O to be ready.
    }
    Serial.println(F("Process Started..."));
}

void loop()
{
    static unsigned long int passed_time = millis();
    if(true == command_flag)
    {
        tokenize();
        parse_input();
     }
    else
    {
        read_into_byte_buffer();
    }
    if((millis() - passed_time) > TIME_QUANTUM)
    {
       c_main = (c_main + 1) % 3;
       passed_time = millis();
    }
    switch(c_main)
    {
        case 0: t_d13(); break;
        case 1: t_led(); break;
        case 2: t_rgb(); break;
    }
}
