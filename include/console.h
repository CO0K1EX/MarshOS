#ifndef CONSOLE_H
#define CONSOLE_H

#define MAX_LINES 55

#include "vga.h"

extern void update_cursor(uint32 vga_cursor_index);

// Clear screen
extern void cls();

extern void print(const char* str);
extern void printChar(char ch, bool updatecursorflag);
extern void printInt(int num);
extern void printColor(const char* str, uint8 fcolor, uint8 bcolor);

extern void printBackSpace();

#endif