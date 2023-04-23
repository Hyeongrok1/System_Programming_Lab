//[include할 다른 헤더 파일 명시]
#ifndef STDLIB_H
# define STDLIB_H
#endif
#include <stdlib.h>

//매크로 선언
#define MAX_INPUT 4096
#define MAX_HEAP 500000
//구조체, type 선언

typedef enum { false, true } bool;

//[전역 변수 선언]

//[함수 선언]



//[전역 변수 선언]

//[함수 선언]
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

void reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = length(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}  

void itoa(int src,char* dst) {
   int temp = 100000000;
   int index = 0;

   if (src == 0) { 
      dst[0] = '0';
      dst[1] = '\0'; 
      return; 
   }

   while(src % temp == src) {
      temp /= 10;
   }
   while(temp >= 1) { 
      dst[index] = src / temp + '0';
      index++;
      src -= (src/temp)*temp;
      temp /= 10;
   }
   dst[index] = '\0'; 
} 

