#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "custom.h"
#include <stdio.h>

int start_idx[500000] = {0,};
int idx = 0;
int base_len = 0;
int first_len = 0;
int second_len = 0;
int first_write = 1;

// Identify the case (1, 2, 3, 4)
int case_id(int *ptr_cnt, char *tmp_ptr[], char *word, int cnt) {
    int case_id = 1;        //default case is 1
    int tmp_cnt = 0;
    char *tmp = NULL;

    //if there are colons, the case is 3
    if (word[0] == '\"' && word[cnt-1] == '\"') {   
        return 3;
    }


    tmp = &word[0];
    // If there are many words, divide them.
    for (int i = 0; i < cnt; i++) {
        if (word[i] == ' ') {      // If there is space, case 2
            case_id = 2;
            word[i] = '\0';
            tmp_ptr[tmp_cnt++] = tmp; 
            tmp = &word[i + 1];
            *ptr_cnt = tmp_cnt;
            continue;
        }
        else if (word[i] == '*') {  // If '*' is inside the word, case 4
            case_id = 4;
            word[i] = '\0';
            tmp_ptr[tmp_cnt++] = &word[i + 1];
            *ptr_cnt = tmp_cnt;
            continue;
        }
    } //tmp_ptr will contain each words. ptr_cnt is (the number of words)
    tmp_ptr[tmp_cnt++] = tmp;
    *ptr_cnt = tmp_cnt;
    return case_id;
}

// I used Boyer and Moore Algorithm for finding location
// This function is for case 1 and case 3
int BoyerMooreSearch(int offset, int *lines, char *text, char *pattern)
{
    int text_length = length(text);
    int pattern_length = length(pattern);
 
    int characters[256] = { -1, };     
    for(int i=0 ; i<pattern_length ; i++)
        characters[(int) pattern[i]] = i;   
 
    int s = 0, j;                            // s is pattern's stride against text
    while(s < text_length-pattern_length)
    {
        j = pattern_length-1;
        
        while(j >= 0) {     
            if (text[s+j] >= 65 && text[s+j] <= 90) text[s+j] += 32;        // Change all characters to lower letters
            if (pattern[j] >= 65 && pattern[j] <= 90) pattern[j] += 32;

            if (text[s+j] == pattern[j]) j--;       // If the character of pattern is the same as the character of text, j decreases.
            else break; 
        }
                                    

        if(j < 0 && (text[s+pattern_length] == ' ' || text[s+pattern_length] == '\n') && (text[s-1] == ' ' || text[s-1] == '\n'))                   
        {               // If all characters are same and it is a word, print the location using write syscall
            char tmp[10];
            char tmp2[20];
            itoa(lines[s] + 1, tmp);                        // Using itoa, convert integer to string
            itoa(offset + s - start_idx[lines[s]], tmp2);

            if (first_write == 0) write(STDOUT_FILENO, " ", 1); // This is for formatting. If the program writes more than one location, there should be space.
            else first_write = 0;

            write(STDOUT_FILENO, tmp, length(tmp));
            write(STDOUT_FILENO, ":", 1);
            write(STDOUT_FILENO, tmp2, length(tmp2));
            
            s += (s < text_length - pattern_length) ? pattern_length - characters[(int) text[s+pattern_length]] : 1;    // s increases
        }
        else
        {
            s += max(1, j - characters[(int) text[s+j]]);
        }
    }
}
// This BoyerMooreSearch is for case 2
int BoyerMooreSearch2(int offset, int *lines, char *text, char *pattern, int *object_list)
{
    int text_length = length(text);
    int pattern_length = length(pattern);
    idx = 0;
    int characters[256] = { -1, };     
    for(int i=0 ; i<pattern_length ; i++)
        characters[(int) pattern[i]] = i;   
 
    int s = 0, j;                   
    while(s < text_length-pattern_length)
    {
        j = pattern_length-1;
 
        while(j >= 0) {
            if (text[s+j] >= 65 && text[s+j] <= 90) text[s+j] += 32;
            if (pattern[j] >= 65 && pattern[j] <= 90) pattern[j] += 32;

            if (text[s+j] == pattern[j]) j--;
            else break;
        }
 
        if(j < 0 && (text[s+pattern_length] == ' ' || text[s+pattern_length] == '\n') && (text[s-1] == ' ' || text[s-1] == '\n'))           
        {
            object_list[idx++] = lines[s] + 1;  // If we found the pattern, we should remember the line
            s += (s < text_length-pattern_length) ? pattern_length-characters[(int) text[s+pattern_length]] : 1;
        }
        else
        {
            s += max(1, j-characters[(int) text[s+j]]);
        }
    }
}
// This Algorithm is for case 4
int BoyerMooreSearch4(int offset, int *lines, char *text, char *pattern, int *line_list, int *offset_list)
{
    int text_length = length(text);
    int pattern_length = length(pattern);
    idx = 0;
    int badchar[256] = { -1, };     
    for(int i=0 ; i<pattern_length ; i++)
        badchar[(int) pattern[i]] = i;   

    int s = 0, j;                   
    while(s < text_length-pattern_length)
    {
        j = pattern_length-1;
        
        while(j >= 0) {
            if (text[s+j] >= 65 && text[s+j] <= 90) text[s+j] += 32;
            if (pattern[j] >= 65 && pattern[j] <= 90) pattern[j] += 32;

            if (text[s+j] == pattern[j]) j--;
            else break;
        }
 
        if(j < 0 && (text[s+pattern_length] == ' ' || text[s+pattern_length] == '\n') && (text[s-1] == ' ' || text[s-1] == '\n'))                 
        {
            line_list[idx] = lines[s] + 1;
            offset_list[idx++] = offset + s - start_idx[lines[s]];  // We should remember the line and the offset at the same time
            s += (s < text_length-pattern_length) ? pattern_length-badchar[(int) text[s+pattern_length]] : 1;
        }
        else
        {
            s += max(1, j-badchar[(int) text[s+j]]);
        }
    }
}

// This is for case 2
// We should check the common line
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

// This is for case 4
// We should check if there is more than one word between two words.
void *intersection4(int *first_list, int *first_offset_list, int *second_list, int *second_offset_list, int *base_list, int word_len) {
    int k = 0;
    
    for (int i = 0; i < first_len; i++) 
    {
        for (int j = 0; j < second_len; j++) 
        {
            if (first_list[i] == second_list[j] && second_offset_list[j] - first_offset_list[i] - word_len >= 3) {
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

// This function cleans the buffer with '\0'
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
    // Repeat if the input is not "PA1EXIT"
    while (1) { 
        char c;
        ptr_cnt = 0;
        exit_or_not = 1;
        cnt = 0;

        // Open file
        if ((fd = open(argv[1], O_RDONLY)) < 0) {
            perror("File opening error");
            exit(1);
        }
        
        // Input
        while(read(STDIN_FILENO, &c, 1) != 0) {
            if (c == '\n') {
                word[cnt] = '\0';
                break;
            }
            word[cnt++] = c;
        }
        
        // If input is PA1EXIT, exit.
        for (int i = 0; i < cnt; i++) {
            if (word[i] != exit_keyword[i]) {
                exit_or_not = 0;
            }
        }
        if (exit_or_not) break;

        // Find the case
        int id = case_id(&ptr_cnt, tmp_ptr, word, cnt);

        // Act differently according to the case id
        if (id == 1) {          // When we should find only one word
            offset = 0;         // Initially, the offset is 0
            int tmp_cnt = 0;  
            char sliced_str[length(word)];
            while(read(fd, buf, MAX_INPUT) != 0) {      // Repeat reading
                buf[MAX_INPUT] = '\0';
                for (int i = 0; i < MAX_INPUT; i++) {   // Count the line
                    if (buf[i] == '\n') {
                        tmp_cnt++;                      // If we meet \n, increase tmp_cnt by 1
                        start_idx[tmp_cnt] = offset + i + 1;
                    }

                    lines[i] = tmp_cnt;                 // lines array contains the line value according to the index value
                }

                BoyerMooreSearch(offset, lines, buf, word); // Finds the location
                offset += MAX_INPUT;                        // offset increases by MAX_INPUT
                
                bufclean(buf);                              // Clean the buffer
            }
            write(STDOUT_FILENO, "\n", 1);                  
            first_write = 1;
        }
        else if (id == 2) {

            close(fd);
            if ((fd = open(argv[1], O_RDONLY)) < 0) {
                write(STDERR_FILENO, "open err", 9);
                exit(1);
            }
            offset = 0; 
            int base_lines[1024] = {0,};
            int tmp_cnt = 0;  
            while(read(fd, buf, MAX_INPUT) != 0) {  // Repeat reading
                base_list = (int*) malloc(sizeof(int) * 1024);
                object_list = (int*) malloc(sizeof(int) * 1024);
                buf[MAX_INPUT] = '\0';
                for (int i = 0; i < MAX_INPUT; i++) {   // Count the line
                    if (buf[i] == '\n') {
                        tmp_cnt++;                      // If we meet \n, increase tmp_cnt by 1
                        start_idx[tmp_cnt] = offset + i + 1;
                    }

                    lines[i] = tmp_cnt;                
                }

                for (int i = 0; i < ptr_cnt; i++) {     // 
                    char *tmp = tmp_ptr[i];
                    if (i == 0) {
                        BoyerMooreSearch2(offset, lines, buf, tmp, base_list);  // The first word is base
                        base_len = idx;
                    }
                    else {
                        BoyerMooreSearch2(offset, lines, buf, tmp, object_list);// Others are object
                        base_list = intersection(base_list, object_list);       // Base_list will contain intersection of two word's lines
                    }
                }

                int tmp_line = -1;
                for (int i = 0; i < base_len; i++) {    // Write the lines to the STDOUT
                    if (base_list[i] > tmp_line) {      // excluding the same line number
                        char itoa_str[10];
                        itoa(base_list[i], itoa_str);

                        if (first_write == 0) write(STDOUT_FILENO, " ", 1);
                        else first_write = 0;
                        write(STDOUT_FILENO, itoa_str, length(itoa_str));
                    }
                    tmp_line = base_list[i];
                }

                offset += MAX_INPUT;
                free(object_list);
                free(base_list);
                bufclean(buf);
            }
            first_write = 1;
            write(STDOUT_FILENO, "\n", 1);
        }
        else if (id == 3) {
            char *tmp = &word[1];   // tmp will contain the word without \"
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
            first_write = 1;
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

                BoyerMooreSearch4(offset, lines, buf, first, first_list, first_offset_list);
                first_len = idx;
                BoyerMooreSearch4(offset, lines, buf, second, second_list, second_offset_list);
                second_len = idx;
                intersection4(first_list, first_offset_list, second_list, second_offset_list, base_list, length(first)); 
                offset += MAX_INPUT;

                int tmp_line = -1;
                for (int i = 0; i < base_len; i++) {
                    if (base_list[i] > tmp_line) {
                        char itoa_str[10];
                        itoa(base_list[i], itoa_str);

                        if (first_write == 0) write(STDOUT_FILENO, " ", 1);
                        else first_write = 0;
                        write(STDOUT_FILENO, itoa_str, length(itoa_str));
                    }
                    tmp_line = base_list[i];
                }
                bufclean(buf);
            }

            free(first_list);
            free(second_list);
            free(first_offset_list);
            free(second_offset_list);
            first_write = 1;
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