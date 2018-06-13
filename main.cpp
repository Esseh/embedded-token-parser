#include<Arduino.h>
#include<Adafruit_NeoPixel.h>
#include<SimpleDHT.h>
#include<DS3231_Simple.h>
#include<EEPROM.h>
#include"config.hpp"
#include"token.hpp"
#include"logfile.h"
#include"error.hpp"
#include"buffer.h"
#include"tokenization.h"
#include"on_demand_debug.h"


// Global Library Variables
SimpleDHT22 dht22;
DS3231_Simple ds3231;
Adafruit_NeoPixel strip;

/// On Board LEDs
Adafruit_NeoPixel alarm1_led;
Adafruit_NeoPixel network_led;
Adafruit_NeoPixel power_led;

// Global Buffers
byte byte_index = 0u;
byte byte_buffer[MAX_BUFFER_SIZE];

byte token_index = 0u;
byte token_buffer[MAX_INSTRUCTION_SIZE];

// Alarm Buffer
#define aCOMFORTABLE 0
#define aMINOR_UNDER aCOMFORTABLE +1
#define aMAJOR_UNDER aMINOR_UNDER +1
#define aMINOR_OVER aMAJOR_UNDER  +1
#define aMAJOR_OVER aMINOR_OVER   +1
byte alarms[1];

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
byte s_rgb_red   = 0u;   // RGB red intensity
byte s_rgb_green = 0u;   // RGB green intensity
byte s_rgb_blue  = 0u;   // RGB blue intensity
word s_blink_time= 500u; // interval between blinks

const t_error token_parse(byte* token_buffer)
{
    t_error err = NO_ERROR;
    byte instruction_length = 0u;
    while(token_buffer[instruction_length] != tEOL)
    {
        instruction_length++;
    }
    switch(instruction_length)
    {
        case 1u:
            switch(token_buffer[0u])
            {
                case tRTC:
                    ds3231.printDateTo_DMY(Serial);
                    Serial.print(F("--"));
                    ds3231.printTimeTo_HMS(Serial);
                    Serial.println();
                break;
                case tLOG:
                    print_log();
                    break;
                case tVERSION:
                    Serial.println(F(VERSION));
                break;
                case tHELP:
                    Serial.println(F("D13 (ON | OFF | BLINK)"));
                    Serial.println(F("LED (GREEN | RED | OFF | BLINK)"));
                    Serial.println(F("RGB <0-255> <0-255> <0-255>"));
                    Serial.println(F("SET (BLINK <0-65535> | RTC)"));
                    Serial.println(F("STATUS (LEDS | TEMPERATURE | HUMIDITY | SENSORS)"));
                    Serial.println(F("VERSION"));
                    Serial.println(F("HELP"));
                    Serial.println(F("LOG"));
                    Serial.println(F("CLEAR LOG"));
                    Serial.println(F("RTC"));
                break;
                default: err = ERROR_GENERIC;
            }
        break;
        case 2u:
            switch(token_buffer[0u])
            { 
                case tSET:
                    switch(token_buffer[1u])
                    {
                        case tRTC:
                            ds3231.promptForTimeAndDate(Serial);
                            Serial.println();
                        break;
                        default: err = ERROR_GENERIC;
                    }
                break;
                case tCLEAR:
                    switch(token_buffer[1u])
                    {
                        case tLOG:
                            clear_log();
                            Serial.println(F("Log Erased"));
                        break;
                        default: err = ERROR_GENERIC;
                    }
                break;
                case tSTATUS:
                    switch(token_buffer[1u])
                    {
                        float temperature, humidity;
                        case tLEDS:
                            dump_state();
                        break;
                        case tHUMIDITY:
                            Serial.print(F("Humidity: "));
                            EEPROM.get(HUMIDITY_INDEX,humidity);
                            Serial.print(humidity);
                            Serial.println(F("%"));
                        break;
                        case tTEMPERATURE:
                            Serial.print(F("Temperature: "));
                            EEPROM.get(TEMPERATURE_INDEX, temperature);
                            Serial.print(temperature);
                            Serial.println(F("F"));
                        break;
                        case tSENSORS:
                            Serial.print(F("Temperature: "));
                            EEPROM.get(TEMPERATURE_INDEX, temperature);
                            Serial.print(temperature);
                            Serial.println(F("F"));
                            Serial.print(F("Humidity: "));
                            EEPROM.get(HUMIDITY_INDEX,humidity);
                            Serial.print(humidity);
                            Serial.println(F("%"));                            
                        break;
                        default: err = ERROR_GENERIC;
                    }
                break;
                case tD13:
                    switch(token_buffer[1u])
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
                    switch(token_buffer[1u])
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
        case 3u:
            switch(token_buffer[0u])
            {
                case tSET:
                    switch(token_buffer[1u])
                    {
                        case tBLINK:
                            s_blink_time = token_buffer[2u];
                        break;
                        default: err = ERROR_GENERIC;
                    }
                break;
                case tADD:
                    Serial.print((word)(token_buffer[1u]+token_buffer[2u]));
                    Serial.println(F(""));
                break;
                default: err = ERROR_GENERIC;
            }
        break;
        case 4u:
            switch(token_buffer[0u])
            {
                case tSET:
                    switch(token_buffer[1u])
                    {
                        case tBLINK:
                            s_blink_time = (token_buffer[2u] << 8u) + token_buffer[3u];
                        break;
                        default: err = ERROR_GENERIC;
                    }
                break;
                case tRGB:
                    s_rgb_red   = token_buffer[1u];
                    s_rgb_green = token_buffer[2u];
                    s_rgb_blue  = token_buffer[3u];
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
    Serial.begin(BAUD);
    pinMode(DUAL_RED_PIN,OUTPUT);
    pinMode(DUAL_GREEN_PIN,OUTPUT);
    pinMode(ON_BOARD_LED_PIN,OUTPUT);

    #if 0
    strip = Adafruit_NeoPixel(1u, RGB_PIN, NEO_GRB + NEO_KHZ800);
    strip.begin();
    strip.show();
    #endif

    alarm1_led = Adafruit_NeoPixel(1u, ALARM_1_LED, NEO_GRB + NEO_KHZ800);
    alarm1_led.begin();
    alarm1_led.show();

    network_led = Adafruit_NeoPixel(1u, RX_TX_LED, NEO_GRB + NEO_KHZ800);
    network_led.begin();
    network_led.show();

    power_led = Adafruit_NeoPixel(1u, POWER_LED, NEO_GRB + NEO_KHZ800);
    power_led.begin();
    power_led.show();

    ds3231.begin();

    /// Set EEPROM Indices if neccessary
    byte EEPROM_sampler;
    EEPROM_sampler = EEPROM.read(logSINX);
    #ifndef NDEBUG
    Serial.print(F("SINX INITIAL: "));
    Serial.print(EEPROM_sampler);
    Serial.println(F(""));
    #endif
    if(0 == EEPROM_sampler)
    {
        EEPROM.write(logSINX,logLOWER_BOUND);
        #ifndef NDEBUG
        Serial.println(F("Wrote to SINX"));
        #endif
    }
    EEPROM_sampler = EEPROM.read(logCINX);
    #ifndef NDEBUG
    Serial.print(F("CINX INITIAL: "));
    Serial.print(EEPROM_sampler);    
    Serial.println(F(""));
    #endif

    if(0 == EEPROM_sampler)
    {
        EEPROM.write(logCINX,logLOWER_BOUND);
        #ifndef NDEBUG
        Serial.println(F("Wrote to CINX"));
        #endif
    }

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
    strip.setPixelColor(0u, s_rgb_red, s_rgb_green, s_rgb_blue);
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
                byte_index = 0u;
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
            byte_index = 0u;
            token_index = 0u;
        break;
    }
}

void t_dht22()
{
    static unsigned long int passed_time = millis();
    if(millis() - passed_time > 6000)
    {
        union {
            byte seq[4];
            float val;
        } temperature_float;
        union {
            byte seq[4];
            float val;
        } humidity_float;
        dht22.read2(A0, &(temperature_float.val), &(humidity_float.val), NULL);
        temperature_float.val = temperature_float.val*(9.f/5.f)+23.f;
        EEPROM.put(TEMPERATURE_INDEX,temperature_float.val);
        EEPROM.put(HUMIDITY_INDEX,humidity_float.val);
        make_log_entry(ltTEMPERATURE, ltWRITE, ltFLOAT, temperature_float.seq[3], temperature_float.seq[2]);
        make_log_entry(ltHUMIDITY, ltWRITE, ltPERCENT, humidity_float.seq[3], humidity_float.seq[2]);
        passed_time = millis();
    }
}

void t_dht22_alarms()
{
    float temperature;
    EEPROM.get(TEMPERATURE_INDEX,temperature);
    byte prev = alarms[0];
    if(temperature >= 90)
    {   
        alarms[0] = aMAJOR_OVER;
        if(prev != alarms[0])
        Serial.println(F("MAJOR OVER"));
    }
    else if(temperature >= 81)
    {
        alarms[0] = aMINOR_OVER;
        if(prev != alarms[0])
        Serial.println(F("MINOR OVER"));
    }
    else if(temperature >= 71)    
    {
        alarms[0] = aCOMFORTABLE;
        if(prev != alarms[0])
        Serial.println(F("COMFORTABLE"));
    }
    else if(temperature >= 61)
    {
        alarms[0] = aMINOR_UNDER;
        if(prev != alarms[0])
        Serial.println(F("MINOR UNDER"));
    }
    else
    {
        alarms[0] = aMAJOR_UNDER;
        if(prev != alarms[0])
        Serial.println(F("MAJOR UNDER"));
    }
}

byte s_rx_led = false;
byte s_tx_led = false;
byte s_power_led = true;


t_system_leds()
{
    if(s_power_led)
    {
        power_led.setPixelColor(0u, 0, 200, 0);
        power_led.show();
    }
    if(s_rx_led)
    {
        network_led.setPixelColor(0u, 0, 200, 0);       
        network_led.show();
        s_rx_led = false;
        delay(30);
    }
    if(s_tx_led)
    {
        network_led.setPixelColor(0u, 238, 118, 0);       
        network_led.show();
        s_tx_led = false;
        delay(30);    
    }
    if(!s_rx_led && !s_tx_led)
    {
        network_led.setPixelColor(0u, 0, 0, 0);
        network_led.show();
        delay(30);
    }
    {
         switch(alarms[0])
         {
             case aMAJOR_UNDER: alarm1_led.setPixelColor(0u,128,0,128); break; // purple
             case aMINOR_UNDER: alarm1_led.setPixelColor(0u,0,0,255);   break; // blue
             case aCOMFORTABLE: alarm1_led.setPixelColor(0u,0,255,0);   break; // green
             case aMINOR_OVER:  alarm1_led.setPixelColor(0u,238,118,0); break; // orange
             case aMAJOR_OVER:  alarm1_led.setPixelColor(0u,255,0,0);   break; // red
         }
         alarm1_led.show();
    }
}

void loop()
{
    static byte c_main = 0u;

    static unsigned long int passed_time = millis();
    if((millis() - passed_time) > TIME_QUANTUM)
    {
       c_main = (c_main + 1u) % 4u;
       passed_time = millis();
    }

    t_io();
    switch(c_main)
    {
        #if 0
        case 0u: t_d13(); break;
        case 1u: t_led(); break;
        case 2u: t_rgb(); break;
        #endif
        case 0u: t_dht22(); break;
        case 1u: t_dht22_alarms(); break;
        case 2u: t_system_leds(); break;
        case 3u:
            s_rx_led = rand() % 2;
            s_tx_led = rand() % 2;
            break;
    }
}
