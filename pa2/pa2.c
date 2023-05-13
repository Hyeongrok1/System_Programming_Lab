#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/stat.h>

// INPUT OF USER
#define MAX_INPUT 200
// MAX PATH NUMBER
#define MAX_PATH_LEN 200
// MAX ARGUMENT NUMBER
#define MAX_ARG_NUM 200

// CMD

// head
bool command_head(int argc, char *argv[]) {
    int fd;
    int line = 0; 
    int nbytes = 0;
    char c;
    if (argc == 2) {
        if ((fd = open(argv[1], O_RDONLY)) < 0) {
            printf("mini: No such file or directory\n");
            return false;
        }
    }
    else if (argc == 4) {
        if ((fd = open(argv[3], O_RDONLY)) < 0) {
            printf("mini: No such file or directory\n");
            return false;
        }
    }

    if (argc == 1) printf("Usage: head [OPTION] [file]\n");
    else if (argc == 2) line = 10;
    else if (argc == 4) {
        if (strcmp(argv[1], "-n") != 0) {
            printf("Usage: %s option is invalid", argv[1]);
            return false;
        }
        line = atoi(argv[2]);
    }
    else {
        if (close(fd) < 0) printf("mini: File close error\n");
        return false;
    }

    int i = 0;
    while (read(fd, &c, 1) != 0) {
        printf("%c", c);
        if (c == '\n') i++;
        if (i == line) break;
    }

    if (close(fd) < 0) printf("mini: File close error\n");
}

// tail
bool command_tail(int argc, char *argv[]) {
    int fd;
    int line = 0; 
    int nbytes = 0;
    char buf[512];
    char c;
    if (argc == 2) {
        if ((fd = open(argv[1], O_RDONLY)) < 0) {
            printf("mini: No such file or directory\n");
            return false;
        }
    }
    else if (argc == 4) {
        if ((fd = open(argv[3], O_RDONLY)) < 0) {
            printf("mini: No such file or directory\n");
            return false;
        }
    }

    if (argc == 1) printf("Usage: head [OPTION] [file]\n");
    else if (argc == 2) line = 10;
    else if (argc == 4) {
        if (strcmp(argv[1], "-n") != 0) {
            printf("Usage: %s option is invalid", argv[1]);
            return false;
        }
        line = atoi(argv[2]);
    }
    else {
        if (close(fd) < 0) printf("mini: File close error\n");
        return false;
    }

    int i = 0;
    int len = 0;

    lseek(fd, -1, SEEK_END);
    while (read(fd, &c, 1) != 0) {
        if (c == '\n') i++;
        if (i == line) break;
        if (lseek(fd, -2, SEEK_CUR) == -1) break;
    }
    while (read(fd, &c, 1) != 0) {
        printf("%c", c);
    }
    printf("\n");
    if (close(fd) < 0) printf("mini: File close error\n");
}

// cat
bool command_cat(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: cat [file]\n");
        return false;
    }
    
    int fd;
    char buf[1024] = {'\0',};
    int nbytes = 0;
    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        printf("mini: No such file or directory\n");
        return false;        
    }

    while ((nbytes = read(fd, buf, 1024)) > 0) {
        printf("%s", buf);
        memset(buf, 0, 1024);
    }
    printf("\n");
    return true;
}

// cp
bool command_cp(int argc, char *argv[]) {
    if (argc == 1) {
        printf("cp: missing file operand\n");
        return false;
    } else if (argc == 2) {
        printf("cp: missing destination file operand after 'FILE1'\n");
        return false;
    } else if (argc != 3) {
        printf("Usage: cp file file2\n");
        return false;
    }
    
    int fd1;
    int fd2;
    char buf[1024] = {'\0',};
    int nbytes = 0;
    if ((fd1 = open(argv[1], O_RDONLY)) < 0) {
        printf("mini: No such file or directory\n");
        return false;        
    }
    if ((fd2 = open(argv[2], O_RDWR|O_CREAT, 0755)) < 0) {
        printf("mini: No such file or directory\n");
        return false;
    }

    while ((nbytes = read(fd1, buf, 1024)) > 0) {
        write(fd2, buf, 1024);
        memset(buf, 0, 1024);
    }
    return true;
}

// rm
void command_rm(int argc, char *argv[]) {
    if (argc == 1) {
        printf("rm: missing file operand\n");
    } else if (argc != 2) {
        printf("rm usage: rm file\n");
    }

    if (unlink(argv[1]) < 0) {
        printf("rm: unlink error\n");
    }
}

// mv
void command_mv(int argc, char *argv[]) {
    if (argc == 1) {
        printf("mv: missing file operand\n");
    } else if (argc == 2) {
        printf("mv: missing destination file operand after 'SOURCE'\n");
    } else if (argc != 3) {
        printf("mv usage: mv source destination\n");
    }

    struct stat buf;
    char path[MAX_PATH_LEN];
    char *cp_argv[3];
    char *rm_argv[2];
    mode_t types[2];
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
        rm_argv[0] = "rm";
        rm_argv[1] = argv[1];
        command_cp(3, cp_argv);
        command_rm(2, rm_argv);
    } else {
        rename(argv[1], argv[2]);
    }
}

// cd 
bool command_cd(int argc, char *argv[]) {
	if (argc == 1) {
		chdir(getenv("HOME"));
	} 
    else if (argc == 2) {
		if (chdir(argv[1]))
			printf("mini: No such file or directory\n");
	} 
    else printf("USAGE: cd [dir]\n");

	return true;
}
// pwd
void command_pwd(int argc, char *argv[]) {
    if (argc != 1) printf("Usage: pwd\n");
    char buf[100] = {'\0',};
    getcwd(buf, 100);
    printf("%s\n", buf);
}
// exit
bool command_exit(int argc, char *argv[]) {
    if (argc != 2 && argc != 1) {
        printf("Usage: exit [NUM]\n");
    }
    write(STDERR_FILENO, "exit\n", 5);
    if (argc == 1) exit(0);
    if (argc == 2) exit(atoi(argv[1]));
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

        // no pipeline, no redirection
        if (strchr(command, '<') == NULL && strchr(command, '>') == NULL && strchr(command, '|') == NULL) {
            ptr = strtok(command, " ");
            while (ptr != NULL) { 
                argv[argc++] = ptr;
                ptr = strtok(NULL, " ");
            }
            argv[argc] = NULL;

            if (strcmp(argv[0], "head") == 0) command_head(argc, argv);
            else if (strcmp(argv[0], "tail") == 0) command_tail(argc, argv);
            else if (strcmp(argv[0], "cat") == 0) command_cat(argc, argv);
            else if (strcmp(argv[0], "cp") == 0) command_cp(argc, argv);            
            // else if (strcmp(argv[0], "mv") == 0) command_mv(argc, argv);
            // else if (strcmp(argv[0], "rm") == 0) command_rm(argc, argv);
            else if (strcmp(argv[0], "cd") == 0) command_cd(argc, argv);
            else if (strcmp(argv[0], "pwd") == 0) command_pwd(argc, argv);
            else if (strcmp(argv[0], "exit") == 0) command_exit(argc, argv);
            else {
                sprintf(path, "/bin/%s", argv[0]);

                if (fork() == 0) {       // Fork and execute the path
                    execv(path, argv);
                    exit(0);
                }
                else wait(&child_status); // Wait the child process     
            }            
        }

        
    }

    return 0;
}