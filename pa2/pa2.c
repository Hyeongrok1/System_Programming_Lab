#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT 200

int main(void) {
    size_t size;    
    char *command, *ptr;

    pid_t pid;

    while(1) {
        command = NULL;
        printf("> ");
        getline(&command, &size, stdin);
        command[strlen(command)-1] = '\0';

        if (strcmp(command, "exit") == 0) break;
    }

    return 0;
}