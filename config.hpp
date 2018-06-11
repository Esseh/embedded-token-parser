/// configuration file of the program

#ifndef CONFIG_HPP
#define CONFIG_HPP

/// Disable/Enable (Verbose) Debug
#if 1
#define NDEBUG
#endif

/// Buffer Sizes
#define MAX_BUFFER_SIZE      20
#define MAX_INSTRUCTION_SIZE 5

/// Version
#define VERSION "v2.0.0005"

/// EEPROM Addresses
#define TEMPERATURE_INDEX 0 // 0 - 3
#define HUMIDITY_INDEX    4 // 4 - 7

/// DHT Settings
#define DHTPIN A0
#define DHTTYPE DHT22

/// Arduino Settings
#define BAUD 115200
#define TIME_QUANTUM 100
#define DUAL_RED_PIN 9
#define DUAL_GREEN_PIN 8
#define ON_BOARD_LED_PIN 13
#define RGB_PIN 10

/// Log Configuration
#define logENTRY_SIZE 6u

/// Log Index locations
#define logSINX 8u
#define logCINX 9u

/// Log Bounds
#define logSIZE 120u + logENTRY_SIZE
#define logLOWER_BOUND 10u
#define logUPPER_BOUND logSIZE + logLOWER_BOUND

#endif
