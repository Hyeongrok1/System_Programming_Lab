#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int fd;
    int line = 0; 
    int nbytes = 0;
    char *err_msg = NULL;
    char c;
    if (argc == 2) {
        if ((fd = open(argv[1], O_RDONLY)) < 0) {
            err_msg = "head: No such file or directory\n";
            write(STDERR_FILENO, err_msg, strlen(err_msg));
            exit(1);
        }
    }
    else if (argc == 4) {
        if ((fd = open(argv[3], O_RDONLY)) < 0) {
            err_msg = "head: No such file or directory\n";
            write(STDERR_FILENO, err_msg, strlen(err_msg));
            exit(1);
        }
    }

    if (argc == 1) {
        err_msg = "head: usage: head [OPTION] [file]\n";
        write(STDERR_FILENO, err_msg, strlen(err_msg));
        exit(1);
    }
    else if (argc == 2) line = 10;
    else if (argc == 4) {
        if (strcmp(argv[1], "-n") != 0) {
            err_msg = "head: Invalid option\n";
            write(STDERR_FILENO, err_msg, strlen(err_msg));
            exit(1);
        }
        line = atoi(argv[2]);
    }
    else {
        if (close(fd) < 0) {
            sprintf(err_msg, "head: Error occurred: %d\n", errno);
            write(STDERR_FILENO, err_msg, strlen(err_msg));
            exit(errno);
        }
        exit(0);
    }

    int i = 0;
    while (read(fd, &c, 1) != 0) {
        printf("%c", c);
        if (c == '\n') i++;
        if (i == line) break;
    }

    if (close(fd) < 0) {
        sprintf(err_msg, "head: Error occurred: %d\n", errno);
        write(STDERR_FILENO, err_msg, strlen(err_msg));
        exit(errno);
    }
    return 0;
}