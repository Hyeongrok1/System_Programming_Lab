#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

// mv
void main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("mv: missing file operand\n");
    } else if (argc == 2) {
        printf("mv: missing destination file operand after 'SOURCE'\n");
    } else if (argc != 3) {
        printf("mv: usage: mv source destination\n");
    }

    struct stat buf;
    char path[200] = {'\0',};
    char *cp_argv[3];
    char *rm_argv[2];
    mode_t types[2];
    int child_status;

    for (int i = 1; i < 3; i++) {
        if (stat(argv[i], &buf) < 0) {
            printf("mv: \n");
        } 
        types[i-1] = buf.st_mode;
    }

    if (S_ISDIR(types[0]) && S_ISDIR(types[1])) {
        rename(argv[1], argv[2]);
    } else if (S_ISDIR(types[1])) {
        cp_argv[0] = "cp";
        cp_argv[1] = argv[1];
        cp_argv[2] = argv[2];
        if (strchr(cp_argv[2], '/') == NULL) {
            sprintf(path, "./%s/%s", argv[2], argv[1]);
            cp_argv[2] = path;
        }
        rm_argv[0] = "rm";
        rm_argv[1] = argv[1];
        if (fork() == 0) {
            execv(path, cp_argv);
            exit(0);
        }
        else wait(&child_status);
        if (fork() == 0) {
            execv(path, rm_argv);
            exit(0);
        }
        else wait(&child_status);
    } else {
        rename(argv[1], argv[2]);
    }
}