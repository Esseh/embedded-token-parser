/// Controls the logfile which provides a persistent history of changes in the systemm.

#ifndef LOGFILE_H
#define LOGFILE_H
#include<Arduino.h>
void print_log();
void make_log_entry(byte module, byte status, byte var_type, byte data1, byte data2);
void clear_log();

#endif
