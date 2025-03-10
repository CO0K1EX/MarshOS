#include "kernel.h"
#include "utils.h"
#include "char.h"
#include "types.h"

#include "calculator.h"

uint32 vga_index;
static uint32 next_line_index = 1;
uint8 g_fore_color = WHITE, g_back_color = BLUE;
int digit_ascii_codes[10] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};

typedef void (*command_func)();

int shift_state = 0;
int caps_lock_state = 0;

uint16 vga_entry(unsigned char ch, uint8 fore_color, uint8 back_color) 
{
  uint16 ax = 0;
  uint8 ah = 0, al = 0;

  ah = back_color;
  ah <<= 4;
  ah |= fore_color;
  ax = ah;
  ax <<= 8;
  al = ch;
  ax |= al;

  return ax;
}

void clear_vga_buffer(uint16 **buffer, uint8 fore_color, uint8 back_color)
{
  uint32 i;
  for(i = 0; i < BUFSIZE; i++){
    (*buffer)[i] = vga_entry(NULL, fore_color, back_color);
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

void init_vga(uint8 fore_color, uint8 back_color)
{
  vga_buffer = (uint16*)VGA_ADDRESS;
  clear_vga_buffer(&vga_buffer, fore_color, back_color);
  g_fore_color = fore_color;
  g_back_color = back_color;
}

uint8 inb(uint16 port)
{
  uint8 ret;
  asm volatile("inb %1, %0" : "=a"(ret) : "d"(port));
  return ret;
}

void outb(uint16 port, uint8 data)
{
  asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

void *memcpy(void *dest, const void *src, unsigned long n) {
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    for (unsigned long i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

int memcmp(const void* s1, const void* s2, unsigned long n)
{
  const unsigned char* a = (const unsigned char*)s1;
  const unsigned char* b = (const unsigned char*)s2;

  for (unsigned long i = 0; i < n; i++)
  {
    if (a[i] != b[i])
    {
      return a[i] - b[i];
    }
  }
  return 0;
}

void update_cursor(uint32 vga_cursor_index)
{
  uint16 position = vga_index;
  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8)(position & 0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8)((position >> 8) & 0xFF));
}

void print_new_line()
{
  if(next_line_index >= 55){
    next_line_index = 0;
    clear_vga_buffer(&vga_buffer, g_fore_color, g_back_color);
  }
  vga_index = 80*next_line_index;
  next_line_index++;
}

void print_char(char ch, bool update_cursor_flag)
{
  vga_buffer[vga_index] = vga_entry(ch, g_fore_color, g_back_color);
  vga_index++;
  update_cursor(vga_index);
}

void print_color_string(char* str, uint8 fore_color, uint8 back_color)
{
    uint32 index = 0;
    uint8 fc, bc;

    fc = g_fore_color;
    bc = g_back_color;

    g_fore_color = fore_color;
    g_back_color = back_color;

    while(str[index])
    {
        print_char(str[index], true);
        index++;
    }
    g_fore_color = fc;
    g_back_color = bc;
}

void print_string(char *str)
{
  uint32 index = 0;
  while(str[index]){
    print_char(str[index], true);
    index++;
  }
}

void print_int(int num)
{
  char str_num[digit_count(num)+1];
  itoa(num, str_num);
  print_string(str_num);
  print_new_line();
}

void get_cpu_info(char* buffer)
{
  unsigned int eax, ebx, ecx, edx;

  for (int i = 0; i < 3; i++)
  {
    eax = 0x80000002 + i;
    asm volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "0"(eax));

    memcpy(buffer + i * 10, &eax, 4);
    memcpy(buffer + i * 16 + 4, &ebx, 4);
    memcpy(buffer + i * 16 + 8, &ecx, 4);
    memcpy(buffer + i * 16 + 12, &edx, 4);
  }

  buffer[48] = '\0';
}

char get_input_keycode()
{
  char ch = 0;
  
  while((ch = inb(KEYBOARD_PORT)) != 0){
    if(ch > 0)
    {
      return ch;
    }
  }
  return ch;
}

void sleep(uint32 timer_count)
{
  wait_for_io(timer_count);
}

/*
keep the cpu busy for doing nothing(nop)
so that io port will not be processed by cpu
here timer can also be used, but lets do this in looping counter
*/
void wait_for_io(uint32 timer_count)
{
  while(1){
    asm volatile("nop");
    timer_count--;
    if(timer_count <= 0)
      break;
    }
}

void print_backspace()
{
  if (vga_index > 0)
  {
    vga_index--;
    vga_buffer[vga_index] = vga_entry(' ', g_fore_color, g_back_color);
    update_cursor(vga_index);
  }
}

void shift_pressed(char keycode)
{
  if (keycode == 0x2A || keycode == 0x36)
  {
    shift_state = 1;
  }

  if (keycode == 0xAA || keycode == 0xB6)
  {
    shift_state = 0;
  }
}

void caps_pressed(char keycode)
{
  if (keycode == 0x3A)
  {
    caps_lock_state = !caps_lock_state;
  }
}

void clear_screen()
{
  clear_vga_buffer(&vga_buffer, g_fore_color, g_back_color);

  vga_index = 0;
}

void cmd_help()
{
    print_string("Available commands:");
    print_new_line();
    print_string("- help: Show available commands");
    print_new_line();
    print_string("- intro: For a short information for new users");
    print_new_line();
    print_string("- clear: Clear the screen");
    print_new_line();
    print_string("- calc: Simple calculator");
    print_new_line();
    print_string("- about: About MarshOS");
    print_new_line();
    print_string("- reboot: Reboot computer");
    print_new_line();
    print_string("- shutdown: Shutdown computer (NOT WORKING)");
    print_new_line();
}

void cmd_intro()
{
  cmd_help();
  print_new_line();
  print_string("Tip: Type any command and press Enter to execute it.");
  print_new_line();
  print_string("Enjoy exploring ");
  print_color_string("MarshOS!",GREEN, BLUE);
  print_new_line();
}

void cmd_clear()
{
  clear_screen();
}

void cmd_exit()
{
  cmd_clear();
  main_input();
}

//Simple calculator
void cmd_calculator()
{
  cmd_clear();
  print_string("Simple calculator v0.0.1");
  print_new_line();
  print_new_line();
  print_string("To exit the calculator, write the command: ");
  print_color_string("EXIT",YELLOW, BLUE);
  print_new_line();
  print_new_line();
  print_string("In development");
  //calculator_input();
}

void cmd_about()
{
  char cpu_name[49];
  get_cpu_info(cpu_name);

  cmd_clear();
  print_string("MarshOS-x86(x64) v0.0.1 Build (250113) | Creator by ");
  print_color_string("CO0K1E",BRIGHT_GREEN, BLUE);
  print_new_line();
  print_new_line();
  print_string("System Information");
  print_new_line();
  print_string("------------------");
  print_new_line();
  print_color_string("CPU: ",YELLOW, BLUE);
  print_string(cpu_name);
  print_new_line();
  print_color_string("RAM: ",YELLOW, BLUE);
  print_string("NONE");
  print_new_line();
  print_color_string("Architecture: ",YELLOW, BLUE);
  print_string("x86_64");
  print_new_line();
  print_string("Kernel Mode: ");
  print_color_string("Protected ",RED, BLUE);
  print_string("Mode");
  print_new_line();
  print_new_line();
  print_color_string("Note: ", YELLOW, BLUE);
  print_new_line();
  print_string("This kernel is in early development stages.");
  print_new_line();
  print_string("Expect bugs and missing features.");
  print_new_line();
  print_new_line();
}

//Problems
void cmd_shutdown()
{
  outb(0x64, 0xF4);
}

void cmd_reboot()
{
  outb(0x64, 0xFE);
}

char* commands[] = {
    "help",
    "intro",
    "clear",
    "exit",
    "calc",
    "about",
    "reboot",
    "shutdown"
};

command_func command_functions[] = {
    cmd_help,
    cmd_intro,
    cmd_clear,
    cmd_exit,
    cmd_calculator,
    cmd_about,
    cmd_shutdown,
    cmd_reboot,
};

void execute_command(char* input)
{
  for (int i = 0; commands[i] != NULL; i++)
  {
    if (strcmp(input,commands[i]) == 0)
    {
      command_functions[i]();
      return;
    }
  }

  print_string("Unknown command: ");
  print_string(input);
  print_new_line();
}

/*void calculator_input()
{
  char ch = 0;
  char keycode = 0;
  char buffer[256];
  int buf_index = 0;

  int num1 = 0, num2 = 0;
  char operation = 0;

  do{
    keycode = get_input_keycode();

    //shift_pressed(keycode);
    caps_pressed(keycode);

    if(keycode == KEY_ENTER)
    {
      buffer[buf_index] = '\0';
      print_new_line();

      if (buf_index > 0)
      {
        int i = 0;

        while(i < buf_index && buffer[i] >= '0' && buffer[i] <= '9')
        {
          num1 = num1 * 10 + char_to_int(buffer[i]);
          i++;
        }

        if (i < buf_index)
        {
          operation = buffer[i];
          i++;
        }

        while(i < buf_index && buffer[i] >= '0' && buffer[i] <= '9')
        {
          num2 = num2 * 10 + char_to_int(buffer[i]);
          i++;
        }

        int result = calculator_interface(num1, operation, num2);

        print_int(result);
      }

      buf_index = 0;
      print_string("> ");
    }
    else if (keycode == KEY_BACKSPACE)
    {
      if (buf_index > 0)
      {
        buf_index--;
        print_backspace();
      }
    }
    else
    {
      ch = get_ascii_char(keycode, caps_lock_state);
      
      if ((ch >= '0' && ch <= '9') || ch == '+' || ch == '-' || ch == '*' || ch == '/')
      {
        if (buf_index < sizeof(buffer) - 1)
        {
          buffer[buf_index++] = ch;
          print_char(ch, true);
        }
      }
    }
    //sleep(0x02FFFFFF); //50331647 Original value
    //sleep(0x32AA741F); //850031647 Good for VMware
    //sleep(0xEE72E1F); //250031647 Good for qemu
    sleep(0x858B69F); //140031647 Very good for qemu
  }while(1); //old condition ch > 0
}*/

void main_input()
{
  char ch = 0;
  char keycode = 0;
  char buffer[256];
  int buf_index = 0;

  print_string("> ");

  do{
    keycode = get_input_keycode();

    //shift_pressed(keycode);
    caps_pressed(keycode);

    if(keycode == KEY_ENTER)
    {
      buffer[buf_index] = '\0';
      print_new_line();

      if (buf_index > 0)
      {
        execute_command(buffer);
      }

      buf_index = 0;

      print_string("> ");
    }
    else if (keycode == KEY_BACKSPACE)
    {
      if (buf_index > 0)
      {
        buf_index--;
        print_backspace();
      }
    }
    else
    {
      ch = get_ascii_char(keycode, caps_lock_state);
      
      if (ch >= 32 && ch <= 126)
      {
        if (buf_index < sizeof(buffer) - 1)
        {
          buffer[buf_index++] = ch;
          print_char(ch, true);
        }
      }
    }
    //sleep(0x02FFFFFF); //50331647 Original value
    //sleep(0x32AA741F); //850031647 Good for VMware
    //sleep(0xEE72E1F); //250031647 Good for qemu
    sleep(0x858B69F); //140031647 Very good for qemu
  }while(1); //old condition ch > 0
}

void kernel_entry()
{
  init_vga(WHITE, BLUE);
  print_string("Welcome to ");
  print_color_string("MarshOS-x86(x64) ",GREEN, BLUE);
  print_string("v0.0.1");
  print_new_line();
  print_new_line();
  print_color_string("For a short information for new users: ",WHITE, BLUE);
  print_color_string("INTRO",YELLOW, BLUE);
  print_new_line();
  print_color_string("For supported shell commands: ",WHITE, BLUE);
  print_color_string("HELP",YELLOW, BLUE);
  print_new_line();
  print_new_line();
  main_input();
}