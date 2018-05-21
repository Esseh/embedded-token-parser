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

// Global Library Variables
Adafruit_NeoPixel strip;

// Determines if it is time to perform tokenization.
bool command_flag = false;

// State Variables
bool s_d13_on    = false;    // D13 on or off
bool s_d13_blink = false;    // D13 blinking or not
bool s_d13_blink_toggle = false;
bool s_led_red   = true;     // true = red , false  = green
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
#endif

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
                    Serial.print("BUILDING NUMERICAL TOKEN: ");
                    Serial.print(prospective_token);
                    Serial.println("");
                    Serial.print("TOKEN ERROR STATE: ");
                    Serial.print(num_token_err);
                    Serial.println("");
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
                            Serial.println("Instruction is too large");
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

void parse_input(byte* token_buffer,byte& token_buffer_index,bool& command_flag)
{
    byte context;
    byte control;
    control = token_buffer[0];
    /// D13
    if(tD13 == control)
    {
        #ifndef NDEBUG
        Serial.println("D13");
        #endif
        control = token_buffer[1];
        /// D13 > ON
        if(tON == control)
        {
            #ifndef NDEBUG
            Serial.println("D13 > ON");
            #endif

            control = token_buffer[2];
            if(tEOL == control)
            {
                #ifndef NDEBUG
                Serial.println("D13 > ON > EOL");
                #endif
                
                s_d13_on    = true;
                s_d13_blink = false;
            }
            else
            {
                Serial.println("Unrecognized Input");
            }
        }
        /// D13 > OFF
        else if(tOFF == control)
        {
            #ifndef NDEBUG
            Serial.println("D13 > OFF");
            #endif

            control = token_buffer[2];
            if(tEOL == control)
            {
                #ifndef NDEBUG
                Serial.println("D13 > OFF > EOL");
                #endif
                
                s_d13_on    = false;
                s_d13_blink = false;
            }
            else
            {
                Serial.println("Unrecognized Input");
            }
        }
        /// D13 > BLINK
        else if(tBLINK == control)
        {
            #ifndef NDEBUG
            Serial.println("D13 > BLINK");
            #endif

            control = token_buffer[2];
            if(tEOL == control)
            {
                #ifndef NDEBUG
                Serial.println("D13 > BLINK > EOL");
                #endif
                
                s_d13_on    = true;
                s_d13_blink = true;
            }
            else
            {
                Serial.println("Unrecognized Input");
            }
        }
        else
        {
            Serial.println("Unrecognized Input");
        }
    }
    /// LED
    else if(tLED == control)
    {
        #ifndef NDEBUG
        Serial.println("LED");
        #endif
        control = token_buffer[1];
        /// LED > GREEN
        if(tGREEN == control)
        {
            #ifndef NDEBUG
            Serial.println("LED > GREEN");
            #endif

            control = token_buffer[2];
            if(tEOL == control)
            {
                #ifndef NDEBUG
                Serial.println("LED > GREEN > EOL");
                #endif

                s_led_red = false;
                s_led_on  = true;
                s_led_blink = false;
            }
            else
            {
                Serial.println("Unrecognized Input");
            }
        }
        /// LED > RED
        else if(tRED == control)
        {
            #ifndef NDEBUG
            Serial.println("LED > RED");
            #endif

            control = token_buffer[2];
            if(tEOL == control)
            {
                #ifndef NDEBUG
                Serial.println("LED > RED > EOL");
                #endif

                s_led_red   = true;
                s_led_on    = true;
                s_led_blink = false;
            }
            else
            {
                Serial.println("Unrecognized Input");
            }

        }
        /// LED > BLINK
        else if(tBLINK == control)
        {
            #ifndef NDEBUG
            Serial.println("LED > BLINK");
            #endif

            control = token_buffer[2];
            if(tEOL == control)
            {
                #ifndef NDEBUG
                Serial.println("LED > BLINK > EOL");
                #endif

                s_led_on    = true;
                s_led_blink = true;
            }
            else
            {
                Serial.println("Unrecognized Input");
            }

        }
        /// LED > OFF
        else if(tOFF == control)
        {
            #ifndef NDEBUG
            Serial.println("LED > OFF");
            #endif

            control = token_buffer[2];
            if(tEOL == control)
            {
                #ifndef NDEBUG
                Serial.println("LED > OFF > EOL");
                #endif

                s_led_on    = false;
                s_led_blink = false;
            }
            else
            {
                Serial.println("Unrecognized Input");
            }

        }
        else
        {
            Serial.println("Unrecognized Input");
        }
    }
    /// RGB
    else if(tRGB == control)
    {
        #ifndef NDEBUG
        Serial.println("RGB");
        #endif
        if(tEOL == token_buffer[4])
        {
            #ifndef NDEBUG
            Serial.println("RGB > num num num");
            #endif
            s_rgb_red   = token_buffer[1];
            s_rgb_green = token_buffer[2];
            s_rgb_blue  = token_buffer[3];
        }
        else
        {
            Serial.println("Unrecognized Input");
        }
    }
    /// SET
    else if(tSET == control)
    {
        #ifndef NDEBUG
        Serial.println("SET");
        #endif
        /// SET > BLINK
        control = token_buffer[1];
        if(tBLINK == control)
        {
            #ifndef NDEBUG
            Serial.println("SET > BLINK");
            #endif

            if(tEOL == token_buffer[3])
            {
                s_blink_time = token_buffer[2];
            }
            else if(tEOL == token_buffer[4])
            {
                s_blink_time = (token_buffer[2] << 8) + token_buffer[3];
            }
            else
            {
                Serial.println("Unrecognized Input");
            }
        }
        else
        {
            Serial.println("Unrecognized Input");            
        }
    }
    /// STATUS
    else if(tSTATUS == control)
    {
        #ifndef NDEBUG
        Serial.println("STATUS");
        #endif

        control = token_buffer[1];
        /// STATUS > LEDS
        if(tLED == control)
        {
            #ifndef NDEBUG
            Serial.println("STATUS > LEDS");
            #endif

            control = token_buffer[2];
            if(tEOL == control)
            {
                #ifndef NDEBUG
                Serial.println("STATUS > LEDS > EOL");
                #endif

                dump_state();
            }
            else
            {
                Serial.println("Unrecognized Input");
            }
        }
        else
        {
            Serial.println("Unrecognized Input");
        }
    }
    /// VERSION
    else if(tVERSION == control)
    {
        #ifndef NDEBUG
        Serial.println("VERSION");
        #endif

        control = token_buffer[1];
        if(tEOL == control)
        {
            #ifndef NDEBUG
            Serial.println("VERSION > EOL");
            #endif

            Serial.println(VERSION);
        }
        else
        {
            Serial.println("Unrecognized Input");
        }
    }
    /// HELP
    else if(tHELP == control)
    {
        #ifndef NDEBUG
        Serial.println("HELP");
        #endif

        control = token_buffer[1];
        if(tEOL == control)
        {
            #ifndef NDEBUG
            Serial.println("HELP > EOL");
            #endif

            Serial.println("D13 (ON | OFF | BLINK)");
            Serial.println("LED (GREEN | RED | OFF | BLINK)");
            Serial.println("RGB <0-255> <0-255> <0-255>");
            Serial.println("SET BLINK <0-65535>");
            Serial.println("STATUS LEDS");
            Serial.println("VERSION");
            Serial.println("HELP");
        }
        else
        {
            Serial.println("Unrecognized Input");
        }
    }
    token_buffer_index = 0;   
    command_flag = false;
    #ifndef NDEBUG
    dump_state();
    #endif

}

void t_d13()
{
    if(s_d13_on)
    {
        if(s_d13_blink)
        {
            static unsigned long int d13_passed_time = millis();
            if((millis() - d13_passed_time) > s_blink_time)
            {
                if(s_d13_blink_toggle)
                {
                    digitalWrite(ON_BOARD_LED_PIN,HIGH);
                }
                else
                {
                    digitalWrite(ON_BOARD_LED_PIN,LOW);
                }
                s_d13_blink_toggle = !s_d13_blink_toggle;
                d13_passed_time = millis();
            }
        }
        else
        {
            digitalWrite(ON_BOARD_LED_PIN,HIGH);
        }
    }
    else
    {
        digitalWrite(ON_BOARD_LED_PIN,LOW);
    }
}

void t_led()
{
    if(s_led_on)
    {
        if(s_led_blink)
        {
            static unsigned long int led_passed_time = millis();
            if((millis() - led_passed_time) > s_blink_time)
            {
                if(s_led_blink_toggle)
                {
                    if(s_led_red)
                    {
                        digitalWrite(DUAL_RED_PIN,HIGH);
                    }
                    else
                    {
                        digitalWrite(DUAL_GREEN_PIN,HIGH);
                    }
                }
                else
                {
                    if(s_led_red)
                    {
                        digitalWrite(DUAL_RED_PIN,LOW);
                    }
                    else
                    {
                        digitalWrite(DUAL_GREEN_PIN,LOW);
                    }
                }
                s_led_blink_toggle = !s_led_blink_toggle;
                led_passed_time = millis();
            }
        }
        else
        {
            if(s_led_red)
            {
                digitalWrite(DUAL_RED_PIN,HIGH);
                digitalWrite(DUAL_GREEN_PIN,LOW);       
            }
            else
            {
                digitalWrite(DUAL_GREEN_PIN,HIGH);
                digitalWrite(DUAL_RED_PIN,LOW);
            }

        }   
    }
    else
    {
        digitalWrite(DUAL_GREEN_PIN,LOW);
        digitalWrite(DUAL_RED_PIN,LOW);
    }
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
    Serial.println("Press Enter to begin");
    while(!Serial.available() && (13 != Serial.read()))
    {
       /// Wait for Serial I/O to be ready.
    }
    Serial.println("Process Started...");
}

void loop()
{
    static unsigned long int passed_time = millis();
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
        parse_input(token_buffer,token_buffer_index, command_flag);
     }
    else
    {
        read_into_byte_buffer(byte_buffer,byte_buffer_index,command_flag);
    }
    if((millis() - passed_time) > TIME_QUANTUM)
    {
       c_main = (c_main + 1) % 3;
       passed_time = millis();
    }
    if(0 == c_main)
    {
        t_d13();
    }
    if(1 == c_main)
    {
        t_led();
    }
    if(2 == c_main)
    {
        t_rgb();
    }
}
