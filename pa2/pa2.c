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

#define OUTPUT_REDIRECT 0
#define INPUT_REDIRECT 1
#define APPEND_REDIRECT 2
#define PIPELINE 3

char cmd_path[MAX_PATH_LEN] = {'\0',};
// cd 
bool command_cd(int argc, char *argv[]) {
	if (argc == 1) {        // If the command is "cd", go to the home directory
		chdir(getenv("HOME"));
	} 
    else if (argc == 2) {   // If the command is "cd [directory]", go to that directory
        struct stat buf;
        mode_t type;

        if (stat(argv[1], &buf) < 0) {  // check the argv[1] type
            printf("mini: No such file or directory\n");
            return false;
        } 
        type = buf.st_mode;

        if (!S_ISDIR(type)) printf("mini: Not a directory\n");  // If it is not a directory,
		else if (chdir(argv[1])) printf("mini: No such file or directory\n");   // when it is a valid directory
	} 
    else printf("cd: usage: cd [dir]\n");

	return true;
}

// exit
bool command_exit(int argc, char *argv[]) {
    if (argc != 2 && argc != 1) {
        printf("exit: usage: exit [NUM]\n");
    }
    write(STDERR_FILENO, "exit\n", 5);
    if (argc == 1) exit(0); // normal mini shell termination
    if (argc == 2) exit(atoi(argv[1])); // the termination value is argv[1]
}

// execute the command line
void execution(char *argv[MAX_ARG_NUM], int argc) {
    int child_status = 0;
    char err_msg[25];
    char path[MAX_PATH_LEN*2];  // contains the execev path
    // If the command is available executable 
    if (strcmp(argv[0], "head") == 0 || 
        strcmp(argv[0], "tail") == 0 ||
        strcmp(argv[0], "cat") == 0 ||
        strcmp(argv[0], "cp") == 0 ||
        strcmp(argv[0], "mv") == 0 ||
        strcmp(argv[0], "rm") == 0 ||
        strcmp(argv[0], "pwd") == 0
    ) {
        if (fork() == 0) {
            sprintf(path, "%s/%s", cmd_path, argv[0]);
            execv(path, argv);
            exit(0);
        } else wait(&child_status);
    } 
    // If the command is built-in
    else if (strcmp(argv[0], "cd") == 0) command_cd(argc, argv);
    else if (strcmp(argv[0], "exit") == 0) command_exit(argc, argv);
    // If the command is executable, but not implemented by myself
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
    } 
    // When the command is executable that starts from "./"
    else if (argv[0][0] == '.' && argv[0][1] == '/') {
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

// signal handlers
void sig_int_handler() {
    printf("\n");
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
    pid_t pid;
    int child_status;
    int fd[2];
    int file_descriptor = 0;
    int file_descriptor2 = 0;

    // Get the path of current directory (the path of executable command)
    getcwd(cmd_path, 200);
    signal(SIGINT, sig_int_handler);    // singl handling
    signal(SIGTSTP, sig_tstp_handler);

    while(1) {
        int argc = 0;
        command = NULL;
        printf("> ");
        // Get the command
        getline(&command, &size, stdin);    

        if (strlen(command) == 1) command[strlen(command)] = '\0';
        else command[strlen(command)-1] = '\0';

        if (strcmp(command, "\n") == 0) continue;
        
        // no pipeline, no redirection
        if (strchr(command, '<') == NULL && strchr(command, '>') == NULL && strchr(command, '|') == NULL) {
            // tokenize
            ptr = strtok(command, " ");
            while (ptr != NULL) { 
                argv[argc++] = ptr;
                ptr = strtok(NULL, " ");
            }
            argv[argc] = NULL;
            // argv contains argument of command (divided by space)

            // If there is no argument, continue
            if (argc == 0) continue;
            
            // Execute the command
            execution(argv, argc);
        } else {
            // If there are pipelines or redirections
            ptr = strtok(command, " ");
            while (ptr != NULL) { 
                argv[argc++] = ptr;
                ptr = strtok(NULL, " ");
            }
            argv[argc] = NULL;

            int red_or_pipe[MAX_ARG_NUM] = {0,};
            int red_or_pipe_num = 0;
            int pipe_num = 0;
            // count the number of redirections and pipelines
            // and record the location
            for (int i = 0; i < argc; i++) {
                if (strcmp(argv[i], ">") == 0) {
                    red_or_pipe[red_or_pipe_num++] = OUTPUT_REDIRECT;
                } else if (strcmp(argv[i], "<") == 0) {
                    red_or_pipe[red_or_pipe_num++] = INPUT_REDIRECT;
                } else if (strcmp(argv[i], ">>") == 0) {
                    red_or_pipe[red_or_pipe_num++] == APPEND_REDIRECT;
                } else if (strcmp(argv[i], "|") == 0) {
                    pipe_num++;
                    red_or_pipe[red_or_pipe_num++] = PIPELINE;
                }
            }

            // Check if the pipeline and redirection is valid
            bool isValid = true;
            if (red_or_pipe_num > 2 && (red_or_pipe[0] == APPEND_REDIRECT || red_or_pipe[0] == OUTPUT_REDIRECT)) isValid = false;
            for (int i = 1; i < red_or_pipe_num-1; i++) {
                if (red_or_pipe[i] != PIPELINE) isValid = false;
            }
            if (red_or_pipe_num > 2 && red_or_pipe[red_or_pipe_num-1] == INPUT_REDIRECT) isValid = false;

            if (!isValid) {
                printf("mini: invalid pipelins or redirections\n");
                continue;
            }

            int i = 0;
            char *first_argv[MAX_ARG_NUM];
            int first_argc = 0;
            char *second_argv[MAX_ARG_NUM];
            int tmp_argc = 0;
            char **pipe_commands[20];
            while (i < argc) {
                // If it encounters ">",
                if (strcmp(argv[i], ">") == 0) {
                    // open the file
                    if(fork() == 0) {
                        if ((file_descriptor = open(argv[i+1], O_RDWR|O_CREAT, 0755)) < 0) {
                            perror("mini:");
                            exit(1);
                        }
                        // stdin is replaced with file_descriptor
                        dup2(file_descriptor, 1);
                        first_argv[first_argc] = NULL;
                        // execute
                        execution(first_argv, first_argc);
                        exit(0);
                    }
                    else wait(&child_status);
                    i += 2; 
                    first_argc = 0;
                } 
                // If it encounters ">>"
                else if (strcmp(argv[i], ">>") == 0) {
                    if(fork() == 0) {
                        // similar way, but it opens the file with O_APPEND
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
                    int k = 0;

                    if (red_or_pipe_num == 1) {
                        if(fork() == 0) {
                            // Place your code in this if block!
                            if ((file_descriptor = open(argv[i+1], O_RDONLY)) < 0) {
                                printf("mini: No such file or directory\n");
                                exit(1);
                            }
                            dup2(file_descriptor, STDIN_FILENO);
                            first_argv[first_argc] = NULL;
                            close(file_descriptor);
                            execution(first_argv, first_argc);
                            exit(0);
                        } else wait(&child_status);
                        i += 2;
                        first_argc = 0;
                        tmp_argc = 0;
                        continue;
                    } else if (red_or_pipe_num == 2 && (red_or_pipe[red_or_pipe_num-1] == OUTPUT_REDIRECT || red_or_pipe[red_or_pipe_num-1] == OUTPUT_REDIRECT)) {
                        if(fork() == 0) {
                            // Place your code in this if block!
                            if ((file_descriptor = open(argv[i+1], O_RDONLY)) < 0) {
                                printf("mini: No such file or directory\n");
                                exit(1);
                            }
                            if (red_or_pipe[red_or_pipe_num-1] == OUTPUT_REDIRECT) {
                                if ((file_descriptor2 = open(argv[i+3], O_RDWR|O_CREAT, 0755)) < 0) {
                                    exit(1);
                                }
                            } else if (red_or_pipe[red_or_pipe_num-1] == APPEND_REDIRECT) {
                                if ((file_descriptor2 = open(argv[i+3], O_RDWR|O_CREAT|O_APPEND, 0755)) < 0) {
                                    exit(1);
                                }
                            }
                            dup2(file_descriptor, STDIN_FILENO);
                            dup2(file_descriptor2, STDOUT_FILENO);
                            first_argv[first_argc] = NULL;
                            close(file_descriptor);
                            execution(first_argv, first_argc);
                            exit(0);
                        } else wait(&child_status);
                        i += 2;
                        first_argc = 0;
                        tmp_argc = 0;
                        continue;                    
                    } else if (red_or_pipe_num >= 2) {
                        first_argv[first_argc] = NULL;
                        pipe_commands[0] = first_argv;
                        int i_dup = i;
                        i++;

                        for (int j = 1; j < red_or_pipe_num+1; j++) {
                            char **tmp_argv = (char **) malloc(sizeof(char *) * MAX_ARG_NUM);
                            while (i < argc && strcmp(argv[i], "|") != 0 && strcmp(argv[i], ">") != 0 && strcmp(argv[i], ">>") != 0) {
                                tmp_argv[tmp_argc++] = argv[i++];
                            }
                            i++;
                            tmp_argv[tmp_argc] = NULL;
                            pipe_commands[j] = tmp_argv;
                            tmp_argc = 0;
                        }
                        pipe_commands[red_or_pipe_num+1] = NULL;

                        int num = 0;
                        int fd_backup = -1;
                        if (red_or_pipe[red_or_pipe_num-1] == PIPELINE) {
                            if (fork() == 0) {
                                for (int num = 0; num < red_or_pipe_num + 1; num++) {
                                    pipe(fd);
                                    if ((pid = fork()) == -1) {
                                        perror("fork");
                                    } else if (pid == 0) {
                                        if (fd_backup == -1) {
                                            // Place your code in this if block!
                                            if ((file_descriptor = open(argv[i_dup+1], O_RDONLY)) < 0) {
                                                printf("mini: No such file or directory\n");
                                                exit(1);
                                            }
                                            dup2(file_descriptor, STDIN_FILENO);
                                            close(file_descriptor);
                                        }
                                        else dup2(fd_backup, STDIN_FILENO);
                                        
                                        if (pipe_commands[num+1] != NULL) {
                                            dup2(fd[1], STDOUT_FILENO);
                                        } 
                                        close(fd[0]);
                                        execution(pipe_commands[num], 0);
                                        exit(0);
                                    } else {
                                        wait(NULL);
                                        close(fd[1]);
                                        if (fd_backup == -1) num++;
                                        fd_backup = fd[0];
                                    }
                                }
                                exit(0);
                            } else wait(&child_status);
                        } else if (red_or_pipe[red_or_pipe_num-1] == APPEND_REDIRECT || red_or_pipe[red_or_pipe_num-1] == OUTPUT_REDIRECT) {
                            if (fork() == 0) {              // basically, similar
                                for (int num = 0; num < red_or_pipe_num + 1; num++) {
                                    pipe(fd);
                                    if ((pid = fork()) == -1) {
                                        perror("fork");
                                    } else if (pid == 0) {
                                        if (fd_backup == -1) {
                                            // Place your code in this if block!
                                            if ((file_descriptor = open(argv[i_dup+1], O_RDONLY)) < 0) {
                                                printf("mini: No such file or directory\n");
                                                exit(1);
                                            }
                                            dup2(file_descriptor, STDIN_FILENO);
                                            close(file_descriptor);
                                        }
                                        else dup2(fd_backup, STDIN_FILENO);

                                        if (pipe_commands[num+1] != NULL) {
                                            if (pipe_commands[num+2] == NULL) { // If the next command is with APPEND_REDIRECT or OUTPUT_REDIRECT
                                                if (red_or_pipe[red_or_pipe_num-1] == OUTPUT_REDIRECT) {
                                                    if ((file_descriptor = open(pipe_commands[num+1][0], O_RDWR|O_CREAT, 0755)) < 0) {
                                                        perror("Opening fail");
                                                        exit(1);
                                                    }
                                                } else if (red_or_pipe[red_or_pipe_num-1] == APPEND_REDIRECT) {
                                                    if ((file_descriptor = open(pipe_commands[num+1][0], O_RDWR|O_CREAT|O_APPEND, 0755)) < 0) {
                                                        perror("Opening fail");
                                                        exit(1);
                                                    }
                                                }
                                                dup2(file_descriptor, 1);
                                            } else dup2(fd[1], 1);
                                        } 
                                        close(fd[0]);
                                        if (pipe_commands[num+1] != NULL) execution(pipe_commands[num], 0);
                                        exit(0);
                                    } else {
                                        wait(NULL);
                                        close(fd[1]);
                                        if (fd_backup == -1) num++;
                                        fd_backup = fd[0];
                                    }
                                }
                                exit(0);
                            } else wait(&child_status); 
                        }
 
                        for (int j = 1; j < pipe_num+1; j++) free(pipe_commands[j]);
                        i++;
                        first_argc = 0;
                        tmp_argc = 0;
                    }
                } 
                // If it encounters "|" (pipeline)
                else if (strcmp(argv[i], "|") == 0) {
                    first_argv[first_argc] = NULL;
                    pipe_commands[0] = first_argv;
                    i++;

                    // get the pipe commands
                    for (int j = 1; j < red_or_pipe_num+1; j++) {
                        char **tmp_argv = (char **) malloc(sizeof(char *) * MAX_ARG_NUM);
                        while (i < argc && strcmp(argv[i], "|") != 0 && strcmp(argv[i], ">") != 0 && strcmp(argv[i], ">>") != 0) {
                            tmp_argv[tmp_argc++] = argv[i++];
                        }
                        i++;
                        tmp_argv[tmp_argc] = NULL;
                        pipe_commands[j] = tmp_argv;
                        tmp_argc = 0;
                    }
                    pipe_commands[red_or_pipe_num+1] = NULL;

                    int j = 0;
                    int fd_backup = 0;
                    // If the last pipe or redirection is pipeline
                    if (red_or_pipe[red_or_pipe_num-1] == PIPELINE) {
                        if (fork() == 0) {
                            for (int num = 0; num < red_or_pipe_num + 1; num++) {          // Using while loop, execute the pipe commands
                                pipe(fd);
                                if ((pid = fork()) == -1) {
                                    perror("fork");
                                } else if (pid == 0) {      
                                    dup2(fd_backup, 0);     // stdin is replaced with fd_backup
                                    if (pipe_commands[num+1] != NULL) { // if next pipeline command exists, replace stdout with fd[1]
                                        dup2(fd[1], 1);
                                    } 
                                    close(fd[0]);
                                    execution(pipe_commands[num], 0);   
                                    exit(0);
                                } else {
                                    wait(NULL);
                                    close(fd[1]);       
                                    fd_backup = fd[0];      // next backup is fd[0]
                                }
                            }
                            exit(0);
                        } else wait(&child_status);
                    } 
                    // If the last pipe or redirection is APPEND_REDIRECT or OUTPUT_REDIRECT
                    else if (red_or_pipe[red_or_pipe_num-1] == APPEND_REDIRECT || red_or_pipe[red_or_pipe_num-1] == OUTPUT_REDIRECT) {
                        if (fork() == 0) {              // basically, similar
                            for (int num = 0; num < red_or_pipe_num + 1; num++) {
                                pipe(fd);
                                if ((pid = fork()) == -1) {
                                    perror("fork");
                                } else if (pid == 0) {
                                    dup2(fd_backup, 0);
                                    if (pipe_commands[num+1] != NULL) {
                                        if (pipe_commands[num+2] == NULL) { // If the next command is with APPEND_REDIRECT or OUTPUT_REDIRECT
                                            if (red_or_pipe[red_or_pipe_num-1] == OUTPUT_REDIRECT) {
                                                if ((file_descriptor = open(pipe_commands[num+1][0], O_RDWR|O_CREAT, 0755)) < 0) {
                                                    perror("Opening fail");
                                                    exit(1);
                                                }
                                            } else if (red_or_pipe[red_or_pipe_num-1] == APPEND_REDIRECT) {
                                                if ((file_descriptor = open(pipe_commands[num+1][0], O_RDWR|O_CREAT|O_APPEND, 0755)) < 0) {
                                                    perror("Opening fail");
                                                    exit(1);
                                                }
                                            }
                                            dup2(file_descriptor, 1);
                                        } else dup2(fd[1], 1);
                                    } 
                                    close(fd[0]);
                                    if (pipe_commands[num+1] != NULL) execution(pipe_commands[num], 0);
                                    exit(0);
                                } else {
                                    wait(NULL);
                                    close(fd[1]);
                                    fd_backup = fd[0];
                                }
                            }
                            exit(0);
                        } else wait(&child_status); 
                    }

                    // free all the allocated area
                    for (int j = 1; j < pipe_num+1; j++) free(pipe_commands[j]);

                    i++;
                    first_argc = 0;
                    tmp_argc = 0;
                } 
                // If it is not yet encountered with redirection or pipeline
                else {
                    first_argv[first_argc++] = argv[i];
                    i++;
                }
            }
        }
    }

    return 0;
} 