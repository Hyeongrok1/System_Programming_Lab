#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "custom.h"
#include <stdio.h>

int start_idx[500000] = {0,};
int common_line[500000] = {0,};
int idx = 0;
int base_len = 0;
int first_len = 0;
int second_len = 0;
int case_id(int *ptr_cnt, char *tmp_ptr[], char *word, int cnt) {
    int case_id = 1;
    int tmp_cnt = 0;
    char *tmp = NULL;

    if (word[0] == '\"' && word[cnt-1] == '\"') {
        return 3;
    }

    tmp = &word[0];
    // 단어 수가 여러개일 경우 나눈다.
    for (int i = 0; i < cnt; i++) {
        if (word[i] == ' ') {
            case_id = 2;
            word[i] = '\0';
            tmp_ptr[tmp_cnt++] = tmp; 
            tmp = &word[i + 1];
            *ptr_cnt = tmp_cnt;
            continue;
        }
        else if (word[i] == '*') {
            case_id = 4;
            word[i] = '\0';
            tmp_ptr[tmp_cnt++] = &word[i + 1];
            *ptr_cnt = tmp_cnt;
            continue;
        }
    } //tmp_ptr에 각각 단어가 들어간다. ptr_cnt는 단어의 수 + 1이다.
    tmp_ptr[tmp_cnt++] = tmp;
    *ptr_cnt = tmp_cnt;
    return case_id;
}

int BoyerMooreSearch(int offset, int *lines, char *text, char *pattern)
{
    int text_length = length(text);
    int pattern_length = length(pattern);
 
    int badchar[256] = { -1, };     // 모든 배열을 -1로 초기화한다
    for(int i=0 ; i<pattern_length ; i++)
        badchar[(int) pattern[i]] = i;   // 값을 채운다
 
    int s = 0, j;                   // s는 text에 대한 pattern의 이동값
    while(s < text_length-pattern_length)
    {
        j = pattern_length-1;
        
        while(j >= 0) {
            if (text[s+j] >= 65 && text[s+j] <= 90) text[s+j] += 32;
            if (pattern[j] >= 65 && pattern[j] <= 90) pattern[j] += 32;

            if (text[s+j] == pattern[j]) j--;
            else break;
        }
                                    // pattern의 문자와 text가 매칭하면 j를 줄여
                                    // 나쁜 문자를 찾는다
 
        if(j < 0 && (text[s+pattern_length] == ' ' || text[s+pattern_length] == '\n') && (text[s-1] == ' ' || text[s-1] == '\n' || s-1 < 0))                   // 문자열이 일치하였을 때
        {
            char tmp[10];
            char tmp2[20];
            itoa(lines[s] + 1, tmp);
            itoa(offset + s - start_idx[lines[s]], tmp2);
            write(STDOUT_FILENO, tmp, length(tmp));
            write(STDOUT_FILENO, ":", 1);
            write(STDOUT_FILENO, tmp2, length(tmp2));
            write(STDOUT_FILENO, " ", 1);
            s += (s < text_length-pattern_length) ? pattern_length-badchar[(int) text[s+pattern_length]] : 1;
        }
        else
        {
            s += max(1, j-badchar[(int) text[s+j]]);
        }
    }
}

int BoyerMooreSearch2(int offset, int *lines, char *text, char *pattern, int *object_list)
{
    int text_length = length(text);
    int pattern_length = length(pattern);
    idx = 0;
    int badchar[256] = { -1, };     // 모든 배열을 -1로 초기화한다
    for(int i=0 ; i<pattern_length ; i++)
        badchar[(int) pattern[i]] = i;   // 값을 채운다
 
    int s = 0, j;                   // s는 text에 대한 pattern의 이동값
    while(s < text_length-pattern_length)
    {
        j = pattern_length-1;
 
        while(j >= 0) {
            if (text[s+j] >= 65 && text[s+j] <= 90) text[s+j] += 32;
            if (pattern[j] >= 65 && pattern[j] <= 90) pattern[j] += 32;

            if (text[s+j] == pattern[j]) j--;
            else break;
        }
            // pattern의 문자와 text가 매칭하면 j를 줄여
                                    // 나쁜 문자를 찾는다
 
        if(j < 0 && (text[s+pattern_length] == ' ' || text[s+pattern_length] == '\n') && (text[s-1] == ' ' || text[s-1] == '\n'))                   // 문자열이 일치하였을 때
        {
            object_list[idx++] = lines[s] + 1;
            s += (s < text_length-pattern_length) ? pattern_length-badchar[(int) text[s+pattern_length]] : 1;
        }
        else
        {
            s += max(1, j-badchar[(int) text[s+j]]);
        }
    }
}

int BoyerMooreSearch3(int offset, int *lines, char *text, char *pattern, int *line_list, int *offset_list)
{
    int text_length = length(text);
    int pattern_length = length(pattern);
    idx = 0;
    int badchar[256] = { -1, };     // 모든 배열을 -1로 초기화한다
    for(int i=0 ; i<pattern_length ; i++)
        badchar[(int) pattern[i]] = i;   // 값을 채운다
 
    int s = 0, j;                   // s는 text에 대한 pattern의 이동값
    while(s < text_length-pattern_length)
    {
        j = pattern_length-1;
        
        while(j >= 0) {
            if (text[s+j] >= 65 && text[s+j] <= 90) text[s+j] += 32;
            if (pattern[j] >= 65 && pattern[j] <= 90) pattern[j] += 32;

            if (text[s+j] == pattern[j]) j--;
            else break;
        }
                                    // pattern의 문자와 text가 매칭하면 j를 줄여
                                    // 나쁜 문자를 찾는다
 
        if(j < 0 && (text[s+pattern_length] == ' ' || text[s+pattern_length] == '\n') && (text[s-1] == ' ' || text[s-1] == '\n'))                   // 문자열이 일치하였을 때
        {
            line_list[idx] = lines[s] + 1;
            offset_list[idx++] = offset + s - start_idx[lines[s]];
            s += (s < text_length-pattern_length) ? pattern_length-badchar[(int) text[s+pattern_length]] : 1;
        }
        else
        {
            s += max(1, j-badchar[(int) text[s+j]]);
        }
    }
}

int *intersection(int *base_list, int *object_list) {
    int object_len = idx;
    int intersection[base_len];
    int k = 0;
    for (int i = 0; i < base_len; i++) 
    {
        for (int j = 0; j < object_len; j++) 
        {
            if (base_list[i] == object_list[j]) {
                intersection[k++] = base_list[i];
            }
        }
    }
    if (k == 0) {
        base_len = k;
        return base_list;
    }
    int *new = (int *) malloc(sizeof(int) * k);
    for (int i = 0; i < k; i++) {
        new[i] = intersection[i];
    }
    base_len = k;
    free(base_list);

    return new;
}


void *intersection4(int *first_list, int *first_offset_list, int *second_list, int *second_offset_list, int *base_list) {
    int k = 0;
    
    for (int i = 0; i < first_len; i++) 
    {
        for (int j = 0; j < second_len; j++) 
        {
            if (first_list[i] == second_list[j] && second_offset_list[j] - first_offset_list[i] >= 3) {
                base_list[k++] = first_list[i];
            }
        }
    }
    // if (k == 0) {
    //     base_len = k;
    //     return base_list;
    // }
    base_len = k;
}

void bufclean(char *buf) {
    for (int i = 0; i < MAX_INPUT; i++) {
        buf[i] = '\0';
    }
}

int main(int argc, char *argv[])
{
    int fd;     //file descriptor
    int retval; //return value
    int lines[MAX_INPUT];
    int cnt = 0;
    int ptr_cnt = 0;
    int exit_or_not = 1;
    int offset = 0;
    int *base_list = NULL;
    int *object_list = NULL;
    int *first_list = NULL;
    int *second_list = NULL;
    int *first_offset_list = NULL;
    int *second_offset_list = NULL;
    int *base_offset_list = NULL;

    char buf[MAX_INPUT];
    char *tmp_ptr[512];
    char exit_keyword[] = "PA1EXIT";
    char *word = (char*) malloc(sizeof(char) * 512);
    bufclean(buf);
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

        //find the case

        int id = case_id(&ptr_cnt, tmp_ptr, word, cnt);
        if (id == 1) {  //when we should find only one word
            offset = 0; //initially, the offset is 0
            int tmp_cnt = 0;  
            while(read(fd, buf, MAX_INPUT) != 0) {  //repeat reading
                buf[MAX_INPUT] = '\0';
                for (int i = 0; i < MAX_INPUT; i++) {   //count the line
                    if (buf[i] == '\n') {
                        tmp_cnt++;      //if we meet \n, increase tmp_cnt by 1
                        start_idx[tmp_cnt] = offset + i + 1;
                    }

                    lines[i] = tmp_cnt;                
                }

                BoyerMooreSearch(offset, lines, buf, word);
                offset += MAX_INPUT;
                bufclean(buf);
            }
            write(STDOUT_FILENO, "\n", 1);
        }
        else if (id == 2) {

            close(fd);
            if ((fd = open(argv[1], O_RDONLY)) < 0) {
                write(STDERR_FILENO, "open err", 9);
                exit(1);
            }
            offset = 0; //initially, the offset is 0
            int base_lines[1024] = {0,};
            int tmp_cnt = 0;  
            while(read(fd, buf, MAX_INPUT) != 0) {  //repeat reading
                base_list = (int*) malloc(sizeof(int) * 1024);
                object_list = (int*) malloc(sizeof(int) * 1024);
                buf[MAX_INPUT] = '\0';
                for (int i = 0; i < MAX_INPUT; i++) {   //count the line
                    if (buf[i] == '\n') {
                        tmp_cnt++;      //if we meet \n, increase tmp_cnt by 1
                        start_idx[tmp_cnt] = offset + i + 1;
                    }

                    lines[i] = tmp_cnt;                
                }

                for (int i = 0; i < ptr_cnt; i++) {
                    char *tmp = tmp_ptr[i];
                    if (i == 0) {
                        BoyerMooreSearch2(offset, lines, buf, tmp, base_list);
                        base_len = idx;
                    }
                    else {
                        BoyerMooreSearch2(offset, lines, buf, tmp, object_list);
                        base_list = intersection(base_list, object_list); 
                    }
                }
                int tmp_line = -1;
                for (int i = 0; i < base_len; i++) {
                    if (base_list[i] > tmp_line) {
                        char itoa_str[10];
                        itoa(base_list[i], itoa_str);
                        write(STDOUT_FILENO, itoa_str, length(itoa_str));
                        write(STDOUT_FILENO, " ", 1);
                    }
                    tmp_line = base_list[i];
                }

                offset += MAX_INPUT;
                free(object_list);
                free(base_list);
                bufclean(buf);
            }

            write(STDOUT_FILENO, "\n", 1);
        }
        else if (id == 3) {
            char *tmp = &word[1];
            for (int i = 0; i < cnt; i++) {
                if (tmp[i] == '\"') {
                    tmp[i] = '\0';
                    break;
                }
            }

            offset = 0; //initially, the offset is 0

            int tmp_cnt = 0;  
            while(read(fd, buf, MAX_INPUT) != 0) {  //repeat reading
                buf[MAX_INPUT] = '\0';
                for (int i = 0; i < MAX_INPUT; i++) {   //count the line
                    if (buf[i] == '\n') {
                        tmp_cnt++;      //if we meet \n, increase tmp_cnt by 1
                        start_idx[tmp_cnt] = offset + i + 1;
                    }

                    lines[i] = tmp_cnt;                
                }

                BoyerMooreSearch(offset, lines, buf, tmp);
                offset += MAX_INPUT;
                bufclean(buf);
            }
            write(STDOUT_FILENO, "\n", 1);
        }
        else if (id == 4) {
            char *second = tmp_ptr[0];
            char *first = tmp_ptr[1];
            offset = 0; //initially, the offset is 0

            int tmp_cnt = 0; 
            base_list = (int*) malloc(sizeof(int) * 1024);
            first_list = (int*) malloc(sizeof(int) * 1024);
            second_list = (int*) malloc(sizeof(int) * 1024); 
            first_offset_list = (int*) malloc(sizeof(int) * 1024);
            second_offset_list = (int*) malloc(sizeof(int) * 1024); 
            while(read(fd, buf, MAX_INPUT) != 0) {  //repeat reading
                buf[MAX_INPUT] = '\0';
                for (int i = 0; i < MAX_INPUT; i++) {   //count the line
                    if (buf[i] == '\n') {
                        tmp_cnt++;      //if we meet \n, increase tmp_cnt by 1
                        start_idx[tmp_cnt] = offset + i + 1;
                    }

                    lines[i] = tmp_cnt;                
                }

                BoyerMooreSearch3(offset, lines, buf, first, first_list, first_offset_list);
                first_len = idx;
                BoyerMooreSearch3(offset, lines, buf, second, second_list, second_offset_list);
                second_len = idx;
                intersection4(first_list, first_offset_list, second_list, second_offset_list, base_list); 
                offset += MAX_INPUT;

                for (int i = 0; i < base_len; i++) {
                    char itoa_str[10];
                    itoa(base_list[i], itoa_str);
                    write(STDOUT_FILENO, itoa_str, length(itoa_str));
                    write(STDOUT_FILENO, " ", 1);
                    // printf("%d ", base_list[i]);
                }
                bufclean(buf);
            }

            free(first_list);
            free(second_list);
            free(first_offset_list);
            free(second_offset_list);
            write(STDOUT_FILENO, "\n", 1);
        }


        if ((retval = close(fd)) < 0) {
            write(STDERR_FILENO, "close err", 10);
            exit(1);
        }
    }
    free(word);
    return 0;
}