//Header file
#include <stdlib.h>

// Macro
#define MAX_INPUT 1048576
// Type

typedef enum { false, true } bool;

// Functions
int max(int a, int b)
{
    if(a > b)
        return a;
    else
        return b;
}

int length(const char* str)
{
    int cnt;
    for (cnt = 0; str[cnt] != '\0'; cnt++);
    return cnt;
}

void itoa(int num, char *str) {
   int temp = 1000000000;
   int index = 0;

   if (num == 0) { 
      str[0] = '0';
      str[1] = '\0'; 
      return; 
   }

   while(num % temp == num) {
      temp /= 10;
   }
   while(temp >= 1) { 
      str[index] = num / temp + '0';
      index++;
      num -= (num/temp)*temp;
      temp /= 10;
   }
   str[index] = '\0'; 
} 