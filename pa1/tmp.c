#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
int Max(int a, int b)
{
    if(a > b)
        return a;
    else
        return b;
}
 
// 나쁜 문자를 이용한 보이어-무어 알고리즘
int BoyerMooreSearch(char *txt, char *pat)
{
    int lengTxt = strlen(txt);
    int lengPat = strlen(pat);
 
    int badchar[256] = { -1, };     // 모든 배열을 -1로 초기화한다
    for(int i=0 ; i<lengPat ; i++)
        badchar[(int)pat[i]] = i;   // 값을 채운다
 
    int s = 0, j;                   // s는 text에 대한 pattern의 이동값
    while(s < lengTxt-lengPat)
    {
        j = lengPat-1;
 
        while(j >= 0 && txt[s+j] == pat[j])
            j--;                    // pattern의 문자와 text가 매칭하면 j를 줄여
                                    // 나쁜 문자를 찾는다
 
        if(j < 0)                   // 문자열이 일치하였을 때
        {
            printf("문자열이 검색된 index : %d\n", s);
            s += (s<lengTxt-lengPat)? lengPat-badchar[(int)txt[s+lengPat]] : 1;
        }
        else
        {
            s += Max(1, j-badchar[(int)txt[s+j]]);
        }
    }
}
 
int main(void)
{
    char txt[] = "GCTTCTGCTACCTTTTGCGCGCGCGCGGAA";
    char pat[] = "CCTTTTGC";
 
    BoyerMooreSearch(txt, pat); //문자열이 검색된 index : 10
 
    return 0;
}