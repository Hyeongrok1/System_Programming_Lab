#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include "custom.h"

int case_id(int *ptr_cnt, char *tmp_ptr[], char *word, int cnt) {
    int case_id = 1;

    if (word[0] == '\"' && word[cnt-1] == '\"') {
        return 3;
    }

    tmp_ptr[*(ptr_cnt)++] = word;
    // 단어 수가 여러개일 경우 나눈다.
    for (int i = 0; i < cnt; i++) {
        if (word[i] == ' ') {
            case_id = 2;
            word[i] = '\0';
            tmp_ptr[*(ptr_cnt)++] = &word[i + 1];
            continue;
        }
        else if (word[i] == '*') {
            case_id = 4;
            word[i] = '\0';
            tmp_ptr[*(ptr_cnt)++] = &word[i + 1];
            continue;
        }
    } //tmp_ptr에 각각 단어가 들어간다. ptr_cnt는 단어의 수 + 1이다.

    return case_id;
}

int main(int argc, char *argv[])
{
    int fd;     //file descriptor
    int retval; //return value
    int exist = 0;
    int start_index = 0;
    int skip_table[256];
    int cnt = 0;
    int ptr_cnt = 0;
    int exit_or_not = 1;
    
    char buf[MAX_INPUT];
    char *tmp_ptr[512];
    char exit_keyword[] = "PA1EXIT";
    char *word = (char*) malloc(sizeof(char) * 512);;

    while (1) {
        char c;
        ptr_cnt = 0;
        exit_or_not = 1;
        cnt = 0;

        //open file
        if ((fd = open(argv[1], O_RDONLY)) < 0) {
            write(STDERR_FILENO, "open err", 9);
            exit(1);
        }
        
        //input
        while(read(STDIN_FILENO, &c, 1) != 0) {
            if (c == '\n') {
                word[cnt] = '\0';
                break;
            }
            word[cnt++] = c;
        }
        
        //if input is PA1EXIT, exit.
        for (int i = 0; i < cnt; i++) {
            if (word[i] != exit_keyword[i]) {
                exit_or_not = 0;
            }
        }
        if (exit_or_not) break;

        int id = case_id(&ptr_cnt, tmp_ptr, word, cnt);

        if (id == 1) {
            
        }
        else if (id == 2) {

        }
        else if (id == 3) {

        }
        else if (id == 4) {

        }


        if ((retval = close(fd)) < 0) {
            write(STDERR_FILENO, "close err", 10);
            exit(1);
        }
        
    }
    free(word);
    return 0;
}