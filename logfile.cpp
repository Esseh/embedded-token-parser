#include<Arduino.h>
#include<EEPROM.h>
#include"eeprom_helper.h"
#include"config.hpp"
#include"token.hpp"

void print_log()
{
    byte sinx = EEPROM.read(logSINX);
    byte cinx = EEPROM.read(logCINX);
    while(sinx != cinx)
    {
        byte token = EEPROM.read(sinx);
        switch(token)
        {
            union {
                byte seq[4];
                float val;
            } floatType;
            case ltTEMPERATURE: Serial.print(F("Temperature ")); sinx++; break;
            case ltHUMIDITY: Serial.print(F("Humidity ")); sinx++; break;
            case ltWRITE: Serial.print(F("WRITE ")); sinx++; break;
            case ltFLOAT: 
                floatType.seq[3] = EEPROM.read(sinx+1);
                floatType.seq[2] = EEPROM.read(sinx+2);
                sinx += 3;
                Serial.print(floatType.val);
            break;
            case ltPERCENT: 
                floatType.seq[3] = EEPROM.read(sinx+1);
                floatType.seq[2] = EEPROM.read(sinx+2);
                sinx += 3;
                Serial.print(floatType.val);
                Serial.print('%');
            break;
            case tEOL: Serial.println(F("")); sinx++; break;
            default: sinx++;
        }
        if(sinx < logLOWER_BOUND)
        {
            sinx += logLOWER_BOUND;
        }
    }
}

void make_log_entry(byte module, byte status, byte var_type, byte data1, byte data2)
{
    byte sinx = EEPROM.read(logSINX);
    byte cinx = EEPROM.read(logCINX);

    EEPROM.write(cinx + 0u, module);
    EEPROM.write(cinx + 1u, status);
    EEPROM.write(cinx + 2u, var_type);
    EEPROM.write(cinx + 3u, data1);
    EEPROM.write(cinx + 4u, data2);
    EEPROM.write(cinx + 5u, tEOL);

    advance_eeprom_byte_cursor(logCINX,logLOWER_BOUND,logUPPER_BOUND,logENTRY_SIZE);
    cinx = EEPROM.read(logCINX);
    if(cinx == sinx)
    {
        advance_eeprom_byte_cursor(logSINX,logLOWER_BOUND,logUPPER_BOUND,logENTRY_SIZE);
    }
}

void clear_log()
{
    EEPROM.write(logSINX,logLOWER_BOUND);
    EEPROM.write(logCINX,logLOWER_BOUND);
    for (byte i = logLOWER_BOUND ; i < logUPPER_BOUND; i++) {
        EEPROM.write(i, 0);
    }

}

