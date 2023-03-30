#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    char *cmd = NULL;
    size_t size = 0;
    char path[100];
    char *args[100];

    while (1) {
        printf("$ ");
        /* get input */
        if ((getline(&cmd, &size, stdin)) < 0) {
            perror("getline failed\n");
            exit(1);
        }
	cmd[strlen(cmd)-1] = '\0';
        /* if "quit", exit */
        if (strncmp(cmd, "quit", 4) == 0) {
            free(cmd);
            exit(0);
        }
	
        /* separate command name and options */
        /* ---------your code here---------- */
	int i = 0;
	char *ptr = strtok(cmd, " ");
	
	while (ptr != NULL) {
		args[i++] = ptr;
		ptr = strtok(NULL, " ");
	}
	args[i] = NULL;
	
	sprintf(path, "/bin/%s", args[0]);
        /* execute command using fork()
           you only need to use fork() once! */
        /* ---------your code here---------- */
	
	if (fork() == 0) {
		execv(path, args);
		exit(1);
	}
	wait(NULL);
    }
}
