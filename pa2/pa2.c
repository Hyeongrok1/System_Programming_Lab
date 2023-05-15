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
#include <errno.h>

// INPUT OF USER
#define MAX_INPUT 200
// MAX PATH NUMBER
#define MAX_PATH_LEN 200
// MAX ARGUMENT NUMBER
#define MAX_ARG_NUM 200

#define RIGHT_REDIRECT 0
#define LEFT_REDIRECT 1
#define RIGHT_APPEND 2
#define PIPELINE 3

// CMD
// head
// bool command_head(int argc, char *argv[]) {
//     int fd;
//     int line = 0; 
//     int nbytes = 0;
//     char *err_msg = NULL;
//     char c;
//     if (argc == 2) {
//         if ((fd = open(argv[1], O_RDONLY)) < 0) {
//             err_msg = "head: No such file or directory\n";
//             write(STDERR_FILENO, err_msg, strlen(err_msg));
//             return false;
//         }
//     }
//     else if (argc == 4) {
//         if ((fd = open(argv[3], O_RDONLY)) < 0) {
//             err_msg = "head: No such file or directory\n";
//             write(STDERR_FILENO, err_msg, strlen(err_msg));
//             return false;
//         }
//     }

//     if (argc == 1) {
//         err_msg = "head: No such file or directory\n";
//         write(STDERR_FILENO, err_msg, strlen(err_msg));
//         return false;
//     }
//     else if (argc == 2) line = 10;
//     else if (argc == 4) {
//         if (strcmp(argv[1], "-n") != 0) {
//             err_msg = "head: Invalid option\n";
//             write(STDERR_FILENO, err_msg, strlen(err_msg));
//             return false;
//         }
//         line = atoi(argv[2]);
//     }
//     else {
//         if (close(fd) < 0) {
//             sprintf(err_msg, "head: Error occurred: %d\n", errno);
//             write(STDERR_FILENO, err_msg, strlen(err_msg));
//         }
//         return false;
//     }

//     int i = 0;
//     while (read(fd, &c, 1) != 0) {
//         printf("%c", c);
//         if (c == '\n') i++;
//         if (i == line) break;
//     }

//     if (close(fd) < 0) {
//         sprintf(err_msg, "head: Error occurred: %d\n", errno);
//         write(STDERR_FILENO, err_msg, strlen(err_msg));
//     }
//     return true;
// }

// // tail
// bool command_tail(int argc, char *argv[]) {
//     int fd;
//     int line = 0; 
//     int nbytes = 0;
//     char *err_msg = NULL;
//     char buf[512];
//     char c;
//     if (argc == 2) {
//         if ((fd = open(argv[1], O_RDONLY)) < 0) {
//             err_msg = "tail: No such file or directory\n";
//             write(STDERR_FILENO, err_msg, strlen(err_msg));
//             return false;
//         }
//     }
//     else if (argc == 4) {
//         if ((fd = open(argv[3], O_RDONLY)) < 0) {
//             err_msg = "tail: No such file or directory\n";
//             write(STDERR_FILENO, err_msg, strlen(err_msg));
//             return false;
//         }
//     }

//     if (argc == 1) printf("Usage: head [OPTION] [file]\n");
//     else if (argc == 2) line = 10;
//     else if (argc == 4) {
//         if (strcmp(argv[1], "-n") != 0) {
//             printf("Usage: %s option is invalid", argv[1]);
//             return false;
//         }
//         line = atoi(argv[2]);
//     }
//     else {
//         if (close(fd) < 0) printf("tail: File close error\n");
//         return false;
//     }

//     int i = 0;
//     int len = 0;

//     lseek(fd, -1, SEEK_END);
//     while (read(fd, &c, 1) != 0) {
//         if (c == '\n') i++;
//         if (i == line) break;
//         if (lseek(fd, -2, SEEK_CUR) == -1) break;
//     }
//     while (read(fd, &c, 1) != 0) {
//         printf("%c", c);
//     }
//     printf("\n");
//     if (close(fd) < 0) printf("tail: File close error\n");
// }

// // cat
// bool command_cat(int argc, char *argv[]) {
//     if (argc != 2) {
//         printf("Usage: cat [file]\n");
//         return false;
//     }
    
//     int fd;
//     char buf[1024] = {'\0',};
//     int nbytes = 0;
//     if ((fd = open(argv[1], O_RDONLY)) < 0) {
//         printf("mini: No such file or directory\n");
//         return false;        
//     }

//     while ((nbytes = read(fd, buf, 1024)) > 0) {
//         printf("%s", buf);
//         memset(buf, 0, 1024);
//     }
//     printf("\n");
//     return true;
// }

// // cp
// bool command_cp(int argc, char *argv[]) {
//     if (argc == 1) {
//         printf("cp: missing file operand\n");
//         return false;
//     } else if (argc == 2) {
//         printf("cp: missing destination file operand after 'FILE1'\n");
//         return false;
//     } else if (argc != 3) {
//         printf("Usage: cp file file2\n");
//         return false;
//     }
    
//     int fd1;
//     int fd2;
//     char buf[1024] = {'\0',};
//     int nbytes = 0;
//     if ((fd1 = open(argv[1], O_RDONLY)) < 0) {
//         printf("mini: No such file or directory\n");
//         return false;        
//     }
//     if ((fd2 = open(argv[2], O_RDWR|O_CREAT, 0755)) < 0) {
//         return false;
//     }

//     while ((nbytes = read(fd1, buf, 1024)) > 0) {
//         write(fd2, buf, 1024);
//         memset(buf, 0, 1024);
//     }
//     return true;
// }

// // rm
// void command_rm(int argc, char *argv[]) {
//     if (argc == 1) {
//         printf("rm: missing file operand\n");
//     } else if (argc != 2) {
//         printf("rm usage: rm file\n");
//     }

//     if (unlink(argv[1]) < 0) {
//         printf("rm: unlink error\n");
//     }
// }

// // mv
// void command_mv(int argc, char *argv[]) {
//     if (argc == 1) {
//         printf("mv: missing file operand\n");
//     } else if (argc == 2) {
//         printf("mv: missing destination file operand after 'SOURCE'\n");
//     } else if (argc != 3) {
//         printf("mv usage: mv source destination\n");
//     }

//     struct stat buf;
//     char path[MAX_PATH_LEN] = {'\0',};
//     char *cp_argv[3];
//     char *rm_argv[2];
//     mode_t types[2];
//     for (int i = 1; i < 3; i++) {
//         if (stat(argv[i], &buf) < 0) {
//             printf("mv: \n");
//         } 
//         types[i-1] = buf.st_mode;
//     }

//     if (S_ISDIR(types[0]) && S_ISDIR(types[1])) {
//         rename(argv[1], argv[2]);
//     } else if (S_ISDIR(types[1])) {
//         cp_argv[0] = "cp";
//         cp_argv[1] = argv[1];
//         cp_argv[2] = argv[2];
//         if (strchr(cp_argv[2], '/') == NULL) {
//             sprintf(path, "./%s/%s", argv[2], argv[1]);
//             cp_argv[2] = path;
//         }
//         rm_argv[0] = "rm";
//         rm_argv[1] = argv[1];
//         command_cp(3, cp_argv);
//         command_rm(2, rm_argv);
//     } else {
//         rename(argv[1], argv[2]);
//     }
// }
// // pwd
// void command_pwd(int argc, char *argv[]) {
//     if (argc != 1) printf("Usage: pwd\n");
//     char buf[100] = {'\0',};
//     getcwd(buf, 100);
//     printf("%s\n", buf);
// }

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

// exit
bool command_exit(int argc, char *argv[]) {
    if (argc != 2 && argc != 1) {
        printf("Usage: exit [NUM]\n");
    }
    write(STDERR_FILENO, "exit\n", 5);
    if (argc == 1) exit(0);
    if (argc == 2) exit(atoi(argv[1]));
}

// execute the command line
void execution(char *argv[MAX_ARG_NUM], int argc) {
    int child_status = 0;
    char err_msg[25];
    char path[MAX_PATH_LEN];
    if (strcmp(argv[0], "head") == 0 ||
        strcmp(argv[0], "tail") == 0 ||
        strcmp(argv[0], "cat") == 0 ||
        strcmp(argv[0], "cp") == 0 ||
        strcmp(argv[0], "mv") == 0 ||
        strcmp(argv[0], "rm") == 0 ||
        strcmp(argv[0], "pwd") == 0
    ) {
        if (fork() == 0) {
            execv(argv[0], argv);
            exit(0);
        } else wait(&child_status);
    } else if (strcmp(argv[0], "cd") == 0) command_cd(argc, argv);
    else if (strcmp(argv[0], "exit") == 0) command_exit(argc, argv);
    else if (strcmp(argv[0], "ls") == 0 ||
        strcmp(argv[0], "man") == 0 ||
        strcmp(argv[0], "grep") == 0 ||
        strcmp(argv[0], "sort") == 0 ||
        strcmp(argv[0], "awk") == 0 ||
        strcmp(argv[0], "bc") == 0
    ) {
        sprintf(path, "/bin/%s", argv[0]);

        if (fork() == 0) {       // Fork and execute the path
            execv(path, argv);
            exit(0);
        } else wait(&child_status); // Wait the child process     
    } else if (argv[0][0] == '.' && argv[0][1] == '/') {
        for (int i = 0; i < strlen(argv[0])-1; i++) argv[0][i] = argv[0][i+2];

        if (fork() == 0) {
            execv(argv[0], argv);
            exit(0);
        } else wait(&child_status);
    }
    else {
        sprintf(err_msg, "mini: command not found\n");
        write(STDERR_FILENO, err_msg, sizeof(err_msg));
    }
}

void sig_int_handler() {
}

void sig_tstp_handler() {
    int status;
    waitpid(-1, &status, WNOHANG | WUNTRACED);
    if (WIFSTOPPED(status)) kill(-1, SIGKILL);
}

int main(void) {
    size_t size;    
    char *command, *ptr;
    char path[MAX_PATH_LEN];
    char *argv[MAX_ARG_NUM];
    char *pipe_or_red[MAX_ARG_NUM];
    pid_t pid;
    int child_status;
    int fd[2];
    int file_descriptor = 0;
    int pipe_or_red_num = 0;

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

            execution(argv, argc);
        } else {
            ptr = strtok(command, " ");
            while (ptr != NULL) { 
                argv[argc++] = ptr;
                ptr = strtok(NULL, " ");
            }
            argv[argc] = NULL;

            int red_or_pipe[MAX_ARG_NUM] = {0,};
            int red_or_pipe_num = 0;
            int pipe_num = 0;
            for (int i = 0; i < argc; i++) {
                if (strcmp(argv[i], ">") == 0) {
                    red_or_pipe[red_or_pipe_num++] = RIGHT_REDIRECT;
                } else if (strcmp(argv[i], "<") == 0) {
                    red_or_pipe[red_or_pipe_num++] = LEFT_REDIRECT;
                } else if (strcmp(argv[i], ">>") == 0) {
                    red_or_pipe[red_or_pipe_num++] == RIGHT_APPEND;
                } else if (strcmp(argv[i], "|") == 0) {
                    pipe_num++;
                    red_or_pipe[red_or_pipe_num++] = PIPELINE;
                }
            }

            // bool isValid = true;
            // if (red_or_pipe_num > 2 && strcmp(argv[red_or_pipe_num - 1], ">") == 0) isValid = false;
            // for (int i = 1; i < argc-1; i++) {
            //     if (strcmp(argv[i], "|") != 0) {
            //         isValid = false;
            //         break;
            //     }
            // }
            // if (red_or_pipe_num > 2 && strcmp(argv[red_or_pipe_num-1], "<") == 0) isValid = false;
            // if (!isValid) {
            //     printf("mini: invalid pipe or redirection\n");
            //     continue;
            // }

            int i = 0;
            char *first_argv[MAX_ARG_NUM];
            int first_argc = 0;
            char *second_argv[MAX_ARG_NUM];
            int second_argc = 0;
            char **pipe_commands[20];
            while (i < argc) {
                if (strcmp(argv[i], ">") == 0) {
                    if(fork() == 0) {
                        if ((file_descriptor = open(argv[i+1], O_RDWR|O_CREAT, 0755)) < 0) {
                            perror("mini:");
                            exit(1);
                        }
                        dup2(file_descriptor, 1);
                        first_argv[first_argc] = NULL;
                        execution(first_argv, first_argc);
                        exit(0);
                    }
                    else wait(&child_status);
                    i += 2;
                    first_argc = 0;
                } else if (strcmp(argv[i], ">>") == 0) {
                    if(fork() == 0) {
                        if ((file_descriptor = open(argv[i+1], O_RDWR|O_CREAT|O_APPEND, 0755)) < 0) {
                            perror("Opening fail");
                            exit(1);
                        }
                        dup2(file_descriptor, 1);
                        first_argv[first_argc] = NULL;
                        execution(first_argv, first_argc);
                        exit(0);
                    }
                    else wait(&child_status);
                    i += 2;
                    first_argc = 0;
                } else if (strcmp(argv[i], "<") == 0) {
                    if(fork() == 0) {
                        // Place your code in this if block!
                        if ((file_descriptor = open(argv[i+1], O_RDWR)) < 0) {
                            printf("mini: No such file or directory\n");
                            exit(1);
                        }
                        dup2(file_descriptor, 0);
                        first_argv[first_argc] = NULL;

                        execution(first_argv, first_argc);
                    } else wait(&child_status);
                    i += 2;
                    first_argc = 0;
                    second_argc = 0;
                } else if (strcmp(argv[i], "|") == 0) {
                    int first_pid = 0;
                    int second_pid = 0;
                    first_argv[first_argc] = NULL;
                    pipe_commands[0] = first_argv;
                    i++;

                    for (int j = 1; j < red_or_pipe_num+1; j++) {
                        char **tmp_argv = (char **) malloc(sizeof(char *) * MAX_ARG_NUM);
                        while (i < argc && strcmp(argv[i], "|") != 0 && strcmp(argv[i], ">") != 0 && strcmp(argv[i], ">>") != 0) {
                            tmp_argv[second_argc++] = argv[i++];
                        }
                        i++;
                        tmp_argv[second_argc] = NULL;
                        pipe_commands[j] = tmp_argv;
                        second_argc = 0;
                    }
                    pipe_commands[red_or_pipe_num+1] = NULL;

                    int j = 0;
                    int fd_backup = 0;
                    char ***cmd = pipe_commands;
                    if (red_or_pipe[red_or_pipe_num-1] == PIPELINE) {
                        if (fork() == 0) {
                            int t = 1;
                            while (*cmd != NULL) {
                                pipe(fd);
                                if ((pid = fork()) == -1) {
                                    perror("fork");
                                } else if (pid == 0) {
                                    dup2(fd_backup, 0);
                                    if (*(cmd+1) != NULL) {
                                        dup2(fd[1], 1);
                                    } 
                                    close(fd[0]);
                                    execvp(*(cmd)[0], *cmd);
                                    exit(0);
                                } else {
                                    wait(NULL);
                                    close(fd[1]);
                                    fd_backup = fd[0];
                                    cmd++;
                                }
                                t++;
                            }
                            exit(0);
                        } else wait(&child_status);
                    } else if (red_or_pipe[red_or_pipe_num-1] == RIGHT_APPEND || red_or_pipe[red_or_pipe_num-1] == RIGHT_REDIRECT) {
                        if (fork() == 0) {
                            int t = 1;
                            while (*cmd != NULL) {
                                pipe(fd);
                                if ((pid = fork()) == -1) {
                                    perror("fork");
                                } else if (pid == 0) {
                                    dup2(fd_backup, 0);
                                    if (*(cmd+1) != NULL) {
                                        if (*(cmd+2) == NULL) {
                                            printf("%s", *(cmd+1)[0]);
                                            if ((file_descriptor = open(*(cmd+1)[0], O_RDWR|O_CREAT, 0755)) < 0) {
                                                perror("Opening fail");
                                                exit(1);
                                            }
                                            dup2(file_descriptor, 1);
                                        } else dup2(fd[1], 1);
                                    } 
                                    close(fd[0]);
                                    execvp(*(cmd)[0], *cmd);
                                    exit(0);
                                } else {
                                    wait(NULL);
                                    close(fd[1]);
                                    fd_backup = fd[0];
                                    cmd++;
                                }
                                t++;
                            }
                            exit(0);
                        } else wait(&child_status); 
                    }


                    for (int j = 1; j < pipe_num+1; j++) {
                        free(pipe_commands[j]);
                    }
                    i++;
                    first_argc = 0;
                    second_argc = 0;
                } else {
                    first_argv[first_argc++] = argv[i];
                    i++;
                }
            }
        }
    }

    return 0;
} 