#include "kernel.h"

typedef void (*command_func)();

int shift_state = 0;
int caps_lock_state = 0;

void main_input();
void calculator_input();

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

void sleep(uint32 timer_count)
{
  wait_for_io(timer_count);
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

void cmd_help()
{
    print("Available commands:\n");
    print("- help: Show available commands\n");
    print("- intro: For a short information for new users\n");
    print("- clear: Clear the screen\n");
    print("- calc: Simple calculator\n");
    print("- fm: Simple file manager\n");
    print("- about: About MarshOS\n");
    print("- reboot: Reboot computer\n");    
    print("- shutdown: Shutdown computer (NOT WORKING)\n");
}

void cmd_intro()
{
  cmd_help();
  print("Tip: Type any command and press Enter to execute it.\n");
  print("Enjoy exploring ");
  printColor("MarshOS!\n", GREEN, gBackColor);
}

void cmd_clear()
{
  cls();
}

void cmd_fm()
{
  cmd_clear();
  
  print("Simple file manager\n");

  fs_init();

  if (fs_create("log.txt"))
  {
    fs_write("log.txt", "Hello, world!", 14);
  }
}

//Simple calculator
void cmd_calculator()
{
  cls();
  print("Simple calculator v0.0.1\n");
  print("\nTo exit the calculator, pressed key: ");
  printColor("ESC\n",YELLOW, gBackColor);
  calculator_input();
}

void cmd_about()
{
  char cpu_name[49];
  get_cpu_info(cpu_name);

  cls();
  print("MarshOS-x86(x64) v0.0.1 Build (250601) | Creator by ");
  printColor("CO0K1E\n\n",BRIGHT_GREEN, gBackColor);
  print("System Information\n");
  print("------------------\n");
  printColor("CPU: ",YELLOW, gBackColor);
  print(cpu_name);
  printColor("\nRAM: ",YELLOW, gBackColor);
  print("NONE\n");
  printColor("Architecture: ",YELLOW, gBackColor);
  print("x86_64\n");
  printColor("Kernel Mode: ", YELLOW, gBackColor);
  printColor("Protected ",RED, gBackColor); // Псевдо режим
  print("Mode\n\n");
  printColor("Note: \n", YELLOW, gBackColor);
  print("This kernel is in early development stages.\n\n");
  print("Expect bugs and missing features.\n");
}

void cmd_shutdown()
{
  outb(0x604, 0x2000);

  print("System halted\n");

  while(1)
  {
    asm volatile ("hlt");
  }
}

void cmd_reboot()
{
  while((inb(0x64) & 0x02) != 0); // Ждем, пока буфер не освободится

  outb(0x64, 0xFE);
}

char* commands[] = {
    "help",
    "intro",
    "clear",
    "fm",
    "calc",
    "about",
    "reboot",
    "shutdown"
};

command_func command_functions[] = {
    cmd_help,
    cmd_intro,
    cmd_clear,
    cmd_fm,
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

  print("Unknown command: ");
  print(input);
}

void calculator_input()
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

    if (keycode == KEY_ESC)
    {
      break;
    }

    if(keycode == KEY_ENTER)
    {
      buffer[buf_index] = '\0';
      //print("\n");

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

        print("=");
        printInt(result);

        num1 = 0;
        num2 = 0;
        operation = 0;
      }

      buf_index = 0;
      print("\n> ");
    }
    else if (keycode == KEY_BACKSPACE)
    {
      if (buf_index > 0)
      {
        buf_index--;
        printBackSpace();
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
          printChar(ch, true);
        }
      }
    }
    //sleep(0x02FFFFFF); //50331647 Original value
    //sleep(0x32AA741F); //850031647 Good for VMware
    //sleep(0xEE72E1F); //250031647 Good for qemu
    sleep(0x858B69F); //140031647 Very good for qemu
  }while(1); //old condition ch > 0
}

void main_input()
{
  char ch = 0;
  char keycode = 0;
  char buffer[256];
  int buf_index = 0;

  print("> ");

  do{
    keycode = get_input_keycode();

    //shift_pressed(keycode);
    caps_pressed(keycode);

    if(keycode == KEY_ENTER)
    {
      buffer[buf_index] = '\0';
      print("\n");

      if (buf_index > 0)
      {
        execute_command(buffer);
      }

      buf_index = 0;

      print("\n> ");
    }
    else if (keycode == KEY_BACKSPACE)
    {
      if (buf_index > 0)
      {
        buf_index--;
        printBackSpace();
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
          printChar(ch, true);
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
  init_vga(WHITE, gBackColor);
  print("Welcome to ");
  printColor("MarshOS-x86(x64) ",GREEN, gBackColor);
  print("v0.0.1\n\n");
  printColor("For a short information for new users: ",WHITE, gBackColor);
  printColor("INTRO\n",YELLOW, gBackColor);
  printColor("For supported shell commands: ",WHITE, gBackColor);
  printColor("HELP\n\n",YELLOW, gBackColor);
  main_input();
}