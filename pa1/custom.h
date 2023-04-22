//[include할 다른 헤더 파일 명시]

//매크로 선언
#define MAX_INPUT 1024

//구조체, type 선언


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

