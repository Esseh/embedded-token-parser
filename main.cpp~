#include<Arduino.h>
#include<Adafruit_NeoPixel.h>
#include<DHT.h>

#if 0
#define NDEBUG
#endif

/// DHT Settings
#define DHTPIN A0
#define DHTTYPE DHT22

/// Arduino Settings
#define BAUD 115200
#define TIME_QUANTUM 50
#define DUAL_RED_PIN 9
#define DUAL_GREEN_PIN 8
#define ON_BOARD_LED_PIN 13
#define RGB_PIN 10

/// Buffer Sizes
#define MAX_BUFFER_SIZE      20
#define MAX_INSTRUCTION_SIZE 5

/// Error Codes
typedef byte t_error;
#define NO_ERROR        0u
#define ERROR_GENERIC   255u
#define ERROR_NOT_READY ERROR_GENERIC - 1u

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
#define tLEDS    tRED -1
#define tHUMIDITY tLEDS -1
#define tTEMPERATURE tHUMIDITY -1
#define tADD tTEMPERATURE -1

// Global Library Variables
Adafruit_NeoPixel strip;
DHT dht(DHTPIN, DHTTYPE);

// Global Buffers
byte byte_index = 0;
byte byte_buffer[MAX_BUFFER_SIZE];

byte token_index = 0;
byte token_buffer[MAX_INSTRUCTION_SIZE];

typedef union t_flag_set1
{
    byte obj;
    struct f
    {	
        bool s_d13_on:           1; // D13 on or off
        bool s_d13_blink :       1; // D13 blinking or not
        bool s_d13_blink_toggle :1;
        bool s_led_red :         1; // true = red , false  = green
        bool s_led_on :          1; // Dual LED on or off
        bool s_led_blink:        1; // Dual LED blinking or not
        bool s_led_blink_toggle: 1;
        bool PLACEHOLDER:        1;
    } f;
} t_flag_set1;

// State Variables
t_flag_set1 flag_set1 = {0}; // Packed Flags      
byte s_rgb_red   = 0;   // RGB red intensity
byte s_rgb_green = 0;   // RGB green intensity
byte s_rgb_blue  = 0;   // RGB blue intensity
word s_blink_time= 500; // interval between blinks

// Lookup Table
const byte token_lookup[] PROGMEM = {
    'D', '1', 3u, tD13,
    'R', 'G', 3u, tRGB,
    'L', 'E', 3u, tLED,
    'S', 'E', 3u, tSET,
    'S', 'T', 6u, tSTATUS,
    'V', 'E', 7u, tVERSION,
    'H', 'E', 4u, tHELP,
    'O', 'N', 2u, tON,
    'O', 'F', 3u, tOFF,
    'B', 'L', 5u, tBLINK,
    'G', 'R', 5u, tGREEN,
    'R', 'E', 3u, tRED,
    'L', 'E', 4u, tLEDS,
    'H', 'U', 8u, tHUMIDITY,
    'T', 'E', 11u, tTEMPERATURE,
    'A', 'D', 3u, tADD,
0};

#ifndef NDEBUG
void dump_buffer_str(byte*buffer, byte index)
{
    Serial.println(F(""));
    byte i = 0;
    for(i = 0; i < index; i++)
    {
        Serial.print((char)buffer[i]);
    }
    Serial.println(F(""));
}
void dump_buffer(byte*buffer, byte index)
{
    Serial.println(F(""));
    byte i = 0;
    for(i = 0; i < index; i++)
    {
        Serial.print(buffer[i]);
        Serial.print(' ');
    }
    Serial.println(F(""));
}
#endif

bool there_is_room_in(byte index, byte max_size)
{
    return index < max_size;
}


bool in_valid_range(byte chr)
{
    return ((chr >= 'A') && (chr <= 'Z')) || ((chr >= '0') && (chr <= '9')) || (13 == chr) || (' ' == chr);
}


void echo_to_screen(byte chr)
{
    switch(chr)
    {
        case 13:
            Serial.println(F(""));
        break;
        case 127:
            Serial.print(F("\b \b"));
        break;
        default:
            if(in_valid_range(chr))
            {
                 Serial.print((char)chr);
            }
        break;
    }
}

void to_upper(byte &chr)
{
    if(chr >= 'a' && chr <= 'z')
    {
        chr -= ('a' - 'A'); 
    }
}

void place_in_buffer(byte* buffer, byte& byte_index, byte new_byte)
{
    buffer[byte_index] = new_byte;
    byte_index++;
}

t_error buffer_reader(byte* buffer, byte& byte_index, byte max_buffer_size)
{
    t_error err = ERROR_NOT_READY;
    if(there_is_room_in(byte_index, max_buffer_size))
    {
        if(Serial.available())
        {
            byte new_byte = Serial.read();
            to_upper(new_byte);
            echo_to_screen(new_byte);
            if(in_valid_range(new_byte))
            {
                place_in_buffer(buffer, byte_index, new_byte);
            }
            if(13 == new_byte)
            {
                err = NO_ERROR;
            }
            else if((127 == new_byte) && (byte_index > 0))
            {
                byte_index--;
            }
            #ifndef NDEBUG
            dump_buffer_str(buffer, byte_index);
            #endif
        }
    }
    else
    {
        err = ERROR_GENERIC;
    }
    return err;
}

void get_next_indices(byte* buffer, byte index, byte &start_index, byte &end_index)
{
    start_index = end_index;
    while((' ' == buffer[start_index]) || (13 == buffer[start_index]))
    {
        start_index++;
    }
    end_index = start_index;
    while((buffer[end_index] != 13) && (buffer[end_index] != ' ') && (end_index < index))
    {
        end_index++;
    }
}

bool is_number(byte *buffer, byte start_index, byte end_index)
{
    bool result = true;
    if(buffer[0] == '0')
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

byte get_token(byte* buffer, byte start_index, byte end_index, const byte* lookup_table)
{
    byte lookup_iterator;
    for(lookup_iterator = 0; pgm_read_byte_near(lookup_table + lookup_iterator) != 0; lookup_iterator += 4)
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
            hint2 = buffer[start_index+1];
        }
        hint2 = buffer[start_index + 1];
        if(
            (hint1 == pgm_read_byte_near(lookup_table + lookup_iterator)) 
            && (hint2 == pgm_read_byte_near(lookup_table + lookup_iterator + 1))
            && (size == pgm_read_byte_near(lookup_table + lookup_iterator + 2))
        )
        {
            #ifndef NDEBUG
            Serial.println(F(""));
            Serial.print(F("RESULT TOKEN:"));
            Serial.print(pgm_read_byte_near(lookup_table + lookup_iterator) + 3);
            Serial.println(F(""));            
            #endif
            return pgm_read_byte_near(lookup_table + lookup_iterator + 3);
        }
    }
    #ifndef NDEBUG
    Serial.println(F(""));
    Serial.println(F("ERROR TOKEN"));
    #endif
    return tERR;
}

word get_numerical_token(byte* buffer, byte start_index, byte end_index)
{
    word prospective_token = 0;
    byte i;
    for(i = start_index; i < end_index; i++)
    {
        prospective_token *= 10;
        prospective_token += (buffer[i] - '0');
        #ifndef NDEBUG
        Serial.println(F(""));
        Serial.print(F("building numerical token:"));
        Serial.print(prospective_token);
        #endif
    }
    #ifndef NDEBUG
    Serial.println(F(""));
    #endif
    return prospective_token;
}

t_error token_populate(
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
    byte end_index = 0;
    while(end_index < byte_index)
    {    
        get_next_indices(byte_buffer, byte_index, start_index, end_index);
        if(end_index >= byte_index)
        {
            break;
        }
        #ifndef NDEBUG
        Serial.println(F(""));
        Serial.print(F("START INDEX: "));
        Serial.print(start_index);
        Serial.print(F("END INDEX: "));
        Serial.print(end_index);
        Serial.print(F("BYTE INDEX: "));
        Serial.print(byte_index);
        Serial.println(F(""));
        #endif
        if(there_is_room_in(token_index, token_buffer_size))
        {
            byte new_token;
            if(is_number(byte_buffer,start_index, end_index))
            {
                word large_token = get_numerical_token(byte_buffer, start_index, end_index);
                if(large_token > 255)
                {
                    place_in_buffer(token_buffer, token_index, large_token >> 8);
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
    #ifndef NDEBUG
    dump_buffer(token_buffer, token_index);
    #endif
    return err;
}

/// Dumps the entire global program state.
void dump_state ()
{
    Serial.println(F(""));
    Serial.print(F("D13 ON: "));
    Serial.print(flag_set1.f.s_d13_on);
    Serial.println(F(""));
    Serial.print(F("D13 BLINK: "));
    Serial.print(flag_set1.f.s_d13_blink);
    Serial.println(F(""));
    Serial.print(F("LED RED: "));
    Serial.print(flag_set1.f.s_led_red);
    Serial.println(F(""));
    Serial.print(F("LED ON: "));
    Serial.print(flag_set1.f.s_led_on);
    Serial.println(F(""));
    Serial.print(F("LED BLINK: "));
    Serial.print(flag_set1.f.s_led_blink);
    Serial.println(F(""));
    Serial.print(F("RGB RED: "));
    Serial.print(s_rgb_red);
    Serial.println(F(""));
    Serial.print(F("RGB GREEN: "));
    Serial.print(s_rgb_green);
    Serial.println(F(""));
    Serial.print(F("RGB BLUE: "));
    Serial.print(s_rgb_blue);
    Serial.println(F(""));
    Serial.print(F("BLINK TIME: "));
    Serial.print(s_blink_time);
    Serial.println(F(""));
}

t_error token_parse(byte* token_buffer)
{
    t_error err = NO_ERROR;
    byte instruction_length = 0;
    while(token_buffer[instruction_length] != tEOL)
    {
        instruction_length++;
    }
    switch(instruction_length)
    {
        case 1:
            switch(token_buffer[0])
            {
                case tVERSION:
                    Serial.println(F("v0.0"));
                break;
                case tHELP:
                    Serial.println(F("D13 (ON | OFF | BLINK)"));
                    Serial.println(F("LED (GREEN | RED | OFF | BLINK)"));
                    Serial.println(F("RGB <0-255> <0-255> <0-255>"));
                    Serial.println(F("SET BLINK <0-65535>"));
                    Serial.println(F("STATUS LEDS"));
                    Serial.println(F("VERSION"));
                    Serial.println(F("HELP"));
                break;
                default: err = ERROR_GENERIC;
            }
        break;
        case 2:
            switch(token_buffer[0])
            { 
                case tSTATUS:
                    switch(token_buffer[1])
                    {
                        case tLEDS:
                            dump_state();
                        break;
                        case tHUMIDITY:
                            Serial.print(F("Humidity: "));
                            Serial.print(dht.readHumidity());
                            Serial.println(F("%"));
                        break;
                        case tTEMPERATURE:
                            Serial.print(F("Temperature: "));
                            Serial.print(dht.readTemperature(true));
                            Serial.println(F("F"));
                        break;
                        default: err = ERROR_GENERIC;
                    }
                break;
                case tD13:
                    switch(token_buffer[1])
                    {
                        case tON:
                            flag_set1.f.s_d13_on = true;
                            flag_set1.f.s_d13_blink = false;
                        break;
                        case tOFF:
                            flag_set1.f.s_d13_on    = false;
                            flag_set1.f.s_d13_blink = false;
                        break;
                        case tBLINK:
                            flag_set1.f.s_d13_on    = true;
                            flag_set1.f.s_d13_blink = true;
                        break;
                        default: err = ERROR_GENERIC;
                    }
                break;
                case tLED:
                    switch(token_buffer[1])
                    {
                        case tRED:
                            flag_set1.f.s_led_red = true;
                            flag_set1.f.s_led_on  = true;
                            flag_set1.f.s_led_blink = false;
                        break;
                        case tGREEN:
                            flag_set1.f.s_led_red = false;
                            flag_set1.f.s_led_on  = true;
                            flag_set1.f.s_led_blink = false;
                        break;
                        case tOFF:
                            flag_set1.f.s_led_on    = false;
                            flag_set1.f.s_led_blink = false;
                        break;
                        case tBLINK:
                            flag_set1.f.s_led_on    = true;
                            flag_set1.f.s_led_blink = true;
                        break;
                        default: err = ERROR_GENERIC;
                    }
                break;
                default: err = ERROR_GENERIC;
            }
        break;
        case 3:
            switch(token_buffer[0])
            {
                case tSET:
                    switch(token_buffer[1])
                    {
                        case tBLINK:
                            s_blink_time = token_buffer[2];
                        break;
                        default: err = ERROR_GENERIC;
                    }
                break;
                case tADD:
                    Serial.print((word)(token_buffer[1]+token_buffer[2]));
                    Serial.println(F(""));
                break;
                default: err = ERROR_GENERIC;
            }
        break;
        case 4:
            switch(token_buffer[0])
            {
                case tSET:
                    switch(token_buffer[1])
                    {
                        case tBLINK:
                            s_blink_time = (token_buffer[2] << 8) + token_buffer[3];
                        break;
                        default: err = ERROR_GENERIC;
                    }
                break;
                case tRGB:
                    s_rgb_red   = token_buffer[1];
                    s_rgb_green = token_buffer[2];
                    s_rgb_blue  = token_buffer[3];
                break;
                default: err = ERROR_GENERIC;
            }
        break;
        default: err = ERROR_GENERIC;
    }
    return err;
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

void t_d13 ()
{
    if(flag_set1.f.s_d13_blink)
    {
        static unsigned long int d13_passed_time = millis();
        if((millis() - d13_passed_time) > s_blink_time)
        {
            digitalWrite(ON_BOARD_LED_PIN,flag_set1.f.s_d13_blink_toggle);
            flag_set1.f.s_d13_blink_toggle = !flag_set1.f.s_d13_blink_toggle;
            d13_passed_time = millis();
        }
    }
    bool blink_factor = !flag_set1.f.s_d13_blink || flag_set1.f.s_d13_blink_toggle;
    digitalWrite(ON_BOARD_LED_PIN,flag_set1.f.s_d13_on*blink_factor);
}

void t_led()
{
    if(flag_set1.f.s_led_blink)
    {
        static unsigned long int led_passed_time = millis();
        if((millis() - led_passed_time) > s_blink_time)
        {
            flag_set1.f.s_led_blink_toggle = !flag_set1.f.s_led_blink_toggle;
            led_passed_time = millis();
        }
    }
    bool blink_factor = !flag_set1.f.s_led_blink || flag_set1.f.s_led_blink_toggle; 
    digitalWrite(DUAL_RED_PIN,flag_set1.f.s_led_red*flag_set1.f.s_led_on*blink_factor);
    digitalWrite(DUAL_GREEN_PIN,!flag_set1.f.s_led_red*flag_set1.f.s_led_on*blink_factor);
}

void t_rgb()
{
    strip.setPixelColor(0, s_rgb_red, s_rgb_green, s_rgb_blue);
    strip.show();
}

void t_io()
{
    switch(buffer_reader(byte_buffer,byte_index,MAX_BUFFER_SIZE))
    {
        case ERROR_GENERIC:
            /// Empty Buffer
            Serial.println(F("Buffer Overflow"));
            {
                byte_index = 0;
            }
        break;
        case ERROR_NOT_READY:
        break;
        case NO_ERROR:
            switch(token_populate(byte_buffer, byte_index, token_buffer, token_index, MAX_INSTRUCTION_SIZE, token_lookup))
            {
                case NO_ERROR:
                switch(token_parse(token_buffer))
                {
                    case NO_ERROR:
                    break;
                    case ERROR_GENERIC:
                    Serial.println(F("Instruction Not Understood"));
                    break;
                }
                break;
                case ERROR_GENERIC:
                Serial.println(F("Max Instruction Length Exceeded"));
                break;
            }
            // Cleanup
            byte_index = 0;
            token_index = 0;
        break;
    }
}

void loop()
{ 
    static byte c_main = 0;
    static unsigned long int passed_time = millis();
    if((millis() - passed_time) > TIME_QUANTUM)
    {
       c_main = (c_main + 1) % 4;
       passed_time = millis();
    }
    switch(c_main)
    {
        case 0: t_d13(); break;
        case 1: t_led(); break;
        case 2: t_rgb(); break;
        case 3: t_io(); break;
    }
}
