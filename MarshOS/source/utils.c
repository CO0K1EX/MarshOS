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
  while(num > 0){
    count++;
    num = num/10;
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
    num = num/10;
  }
  return count;
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

int strcmp(const char* str1, const char* str2) {
    int i = 0;

    // We compare the characters one at a time
    while (str1[i] && str1[i] == str2[i]) {
        i++;
    }

    // We return the difference in the ASCII values of the first mismatched characters.
    return str1[i] - str2[i];
}