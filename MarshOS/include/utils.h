#ifndef UTILS_H
#define UTILS_H

#include "types.h"

extern uint32 strlen(const char*);
extern uint32 digit_count(int);
extern uint64 long_digit_count(int);
extern void itoa(int, char *);
extern int strcmp(char* str1, char* str2);

#endif