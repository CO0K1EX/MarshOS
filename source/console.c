#include "console.h"

void update_cursor(uint32 vga_cursor_index)
{
    uint16 position = vga_cursor_index;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8)(position & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8)((position >> 8) & 0xFF));
}

void cls()
{
    clear_vga_buffer(&vga_buffer, gForeColor, gBackColor);

    vga_index = 0;
}

void print(const char* str)
{    
    uint32 index = 0;

    while (str[index])
    {
        if (str[index] == '\n')
        {
            if (next_line_index >= MAX_LINES)
            {
                next_line_index = 0;
                clear_vga_buffer(&vga_buffer, gForeColor, gBackColor);
            }

            vga_index = 80 * next_line_index;
            next_line_index++;
        }
        else
        {
            printChar(str[index], true);
        }
        index++;
    }
}

void printChar(char ch, bool updatecursorflag)
{
    vga_buffer[vga_index] = entry_vga(ch, gForeColor, gBackColor);
    vga_index++;
    update_cursor(vga_index);
}

void printInt(int num)
{
    char str_num[digit_count(num)+1];
    itoa(num, str_num);
    
    print(str_num);
}

void printColor(const char* str, uint8 fcolor, uint8 bcolor)
{
    uint32 index = 0;
    uint8 bc, fc;

    bc = gBackColor;
    fc = gForeColor;

    gBackColor = bcolor;
    gForeColor = fcolor;

    while (str[index])
    {
        if (str[index] == '\n')
        {
            if (next_line_index >= MAX_LINES)
            {
                next_line_index = 0;
                clear_vga_buffer(&vga_buffer, gForeColor, gBackColor);
            }

            vga_index = 80 * next_line_index;
            next_line_index++;
        }
        else
        {
            printChar(str[index], true);
        }
        index++;
    }

    gBackColor = bc;
    gForeColor = fc;
}

void printBackSpace()
{
    if (vga_index > 0)
    {
        vga_index--;
        vga_buffer[vga_index] = entry_vga(' ', gForeColor, gBackColor);
        update_cursor(vga_index);
    }
}