#include"on_demand_debug.h"
#include"Arduino.h"

extern union t_flag_set1
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
} flag_set1;

extern byte s_rgb_red;
extern byte s_rgb_green;
extern byte s_rgb_blue;
extern word s_blink_time;

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
