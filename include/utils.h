#ifndef UTILS_H
#define UTILS_H

#include "types.h"

extern uint32 digit_count(int);
extern uint64 long_digit_count(int);

extern uint8 inb(uint16 port);
extern void outb(uint16 port, uint8 data);

extern void itoa(int, char*);
extern void *memcpy(void* dest, const void* src, size_t n);
extern int memcmp(const void* s1, const void* s2, size_t n);
extern uint32 strlen(const char*);
extern int strcmp(const char* str1, const char* str2);
extern char *strncpy(char* dest, const char* src, size_t n);

#endif