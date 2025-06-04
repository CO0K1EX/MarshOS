#include "utils.h"

uint32 strlen(const char* str)
{
  uint32 length = 0;
  while(str[length])
    length++;
  
  return length;
}

uint32 digit_count(int num)
{
  uint32 count = 0;
  if(num == 0)
    return 1;
  
    while(num > 0)
    {
      count++;
      num = num / 10;
    }

  return count;
}

uint64 long_digit_count(int num)
{
  uint64 count = 0;
  if (num == 0)
     return 1;

  while(num > 0)
  {
    count++;
    num = num / 10;
  }

  return count;
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

void itoa(int num, char *number)
{
  int dgcount = digit_count(num);
  int index = dgcount - 1;
  char x;
  if(num == 0 && dgcount == 1){
    number[0] = '0';
    number[1] = '\0';
  }else{
    while(num != 0){
      x = num % 10;
      number[index] = x + '0';
      index--;
      num = num / 10;
    }
    number[dgcount] = '\0';
  }
}

void *memcpy(void* dest, const void* src, size_t n)
{
  unsigned char* d = (unsigned char*)dest;
  const unsigned char* s = (const unsigned char*)src;

  for (unsigned long i = 0; i < n; i++)
  {
    d[i] = s[i];
  }

  return dest;
}

int memcmp(const void* s1, const void* s2, size_t n)
{
  const unsigned char* a = (const unsigned char*)s1;
  const unsigned char* b = (const unsigned char*)s2;

  for (size_t i = 0; i < n; i++)
  {
    if (a[i] != b[i])
    {
      return a[i] - b[i];
    }
  }

  return 0;
}

char *strncpy(char* dest, const char* src, size_t n)
{
  char* ret = dest;

  while (n > 0 && *src != '\0')
  {
    *dest++ = *src++;
    n--;
  }
  while (n > 0)
  {
    *dest++ = '\0';
    n--;
  }

  return ret;
}

int strcmp(const char* str1, const char* str2) {
    int i = 0;

    // Сравниваем символы по очереди
    while (str1[i] && str1[i] == str2[i]) {
        i++;
    }

    // Возвращаем разницу ASCII значений первых несовпадающих символов
    return str1[i] - str2[i];
}