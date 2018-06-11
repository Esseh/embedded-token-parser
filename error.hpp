/// Defines the error interface as well as any related error tokens for processing.

#ifndef t_errorHPP
#include<Arduino.h>
#define t_errorHPP
#define NO_ERROR        0u
#define ERROR_GENERIC   255u
#define ERROR_NOT_READY ERROR_GENERIC - 1u
/// Error Codes
typedef byte t_error;
#endif


