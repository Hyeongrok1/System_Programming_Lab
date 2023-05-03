#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#define MAX_INPUT 200
#define MAX_PATH_LEN 200
#define MAX_ARG_NUM 200

bool command_cd(int argc, char *argv[])
{
	if (argc == 1) {
		chdir(getenv("HOME"));
	} else if (argc == 2) {
		if (chdir(argv[1]))
			printf("mini: No such file or directory\n");
	} else printf("USAGE: cd [dir]\n");

	return true;
}

void sig_int_handler() {
    // write code
}

void sig_tstp_handler() {
    // write code
}

int main(void) {
    size_t size;    
    char *command, *ptr;
    char path[MAX_PATH_LEN];
    char *argv[MAX_ARG_NUM];
    pid_t pid;
    int child_status;
    signal(SIGINT, sig_int_handler);
    signal(SIGTSTP, sig_tstp_handler);
    while(1) {
        int argc = 0;
        command = NULL;
        printf("> ");
        getline(&command, &size, stdin);
        command[strlen(command)-1] = '\0';

        if (strcmp(command, "exit") == 0) break;


        if (strchr(command, '<') == NULL && strchr(command, '>') == NULL && strchr(command, '|') == NULL) {
            // When there is no pipe or redirection
            ptr = strtok(command, " ");
            while (ptr != NULL) { 
                argv[argc++] = ptr;
                ptr = strtok(NULL, " ");
            }
            argv[argc] = NULL;
            if (strcmp(argv[0], "cd") == 0) {
                command_cd(argc, argv);
            }

            sprintf(path, "/bin/%s", argv[0]);

            if (fork() == 0) {       // Fork and execute the path
                execv(path, argv);
                exit(0);
            }
            else wait(&child_status); // Wait the child process     
        }
    }

    return 0;
}