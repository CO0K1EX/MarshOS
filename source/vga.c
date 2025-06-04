#include "vga.h"

uint16* vga_buffer = (uint16*)VGA_ADDRESS;
uint32 vga_index = 0;

uint32 next_line_index = 1;
uint8 gBackColor = BLACK;
uint8 gForeColor = WHITE;

uint16 entry_vga(unsigned char ch, uint8 fcolor, uint8 bcolor)
{
    uint16 ax = 0;
    uint8 ah = 0, al = 0;

    ah = gBackColor;
    ah <<= 4;
    ah |= fcolor;
    ax = ah;
    ax <<= 8;
    al = ch;
    ax |= al;

    return ax;
}

void init_vga(uint8 fcolor, uint8 bcolor)
{
    clear_vga_buffer(&vga_buffer, fcolor, bcolor);
    gForeColor = fcolor;
    gBackColor = bcolor;
}

void clear_vga_buffer(uint16** buffer, uint8 fcolor, uint8 bcolor)
{
    uint32 i;

    for (i = 0; i < BUFSIZE; i++)
    {
        (*buffer)[i] = entry_vga(NULL, fcolor, bcolor);
    }
    next_line_index = 1;
    vga_index = 0;
}

/*void clear_vga_buffer_with_offset(uint16 **buffer, uint8 fore_color, uint8 back_color, int offset_lines)
{
  uint32 i;

  for (i = offset_lines * VGA_WIDHT; i < BUFSIZE; i++)
  {
    (*buffer)[i] = vga_entry(' ', fore_color, back_color);
  }
}*/