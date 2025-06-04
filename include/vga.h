#ifndef VGA_H
#define VGA_H

#include "utils.h"

#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 20

#define BUFSIZE 2200

extern uint16* vga_buffer;
extern uint32 vga_index;

extern uint32 next_line_index; // value 1
extern uint8 gBackColor; // default value BLUE
extern uint8 gForeColor; // default value WHITE
//extern digit_ascii_codes[10] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};

extern uint16 entry_vga(unsigned char ch, uint8 fcolor, uint8 bcolor);
extern void init_vga(uint8 fcolor, uint8 bcolor);
extern void clear_vga_buffer(uint16** buffer, uint8 fcolor, uint8 bcolor);

enum vga_color
{
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    GREY,
    DARK_GREY,
    BRIGHT_BLUE,
    BRIGHT_GREEN,
    BRIGHT_CYAN,
    BRIGHT_RED,
    BRIGHT_MAGENTA,
    YELLOW,
    WHITE,
};

#endif