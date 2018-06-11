/// Contains token information local the current program.

#ifndef TOKENS_HPP
#define TOKENS_HPP

/// State Tokens
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
#define tSENSORS tADD -1
#define tLOG tSENSORS -1
#define tCLEAR tLOG   -1
#define tRTC tCLEAR   -1

/// Log Tokens
#define ltTEMPERATURE 1u
#define ltHUMIDITY    ltTEMPERATURE +1
#define ltWRITE       ltHUMIDITY    +1
#define ltFLOAT       ltWRITE       +1  // 4 Bytes
#define ltPERCENT     ltFLOAT       +1  // Float, percent representation

// Token Lookup Table
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
    'S', 'E', 7u, tSENSORS,
    'L', 'O', 3u, tLOG,
    'C', 'L', 5u, tCLEAR,
    'R', 'T', 3u,  tRTC,
0u};

#endif
