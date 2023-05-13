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
    char buf[512];
    char c;
    if (argc == 2) {
        if ((fd = open(argv[1], O_RDONLY)) < 0) {
            err_msg = "tail: No such file or directory\n";
            write(STDERR_FILENO, err_msg, strlen(err_msg));
            return 0;
        }
    }
    else if (argc == 4) {
        if ((fd = open(argv[3], O_RDONLY)) < 0) {
            err_msg = "tail: No such file or directory\n";
            write(STDERR_FILENO, err_msg, strlen(err_msg));
            return 0;
        }
    }

    if (argc == 1) printf("Usage: head [OPTION] [file]\n");
    else if (argc == 2) line = 10;
    else if (argc == 4) {
        if (strcmp(argv[1], "-n") != 0) {
            printf("Usage: %s option is invalid", argv[1]);
            return 0;
        }
        line = atoi(argv[2]);
    }
    else {
        if (close(fd) < 0) printf("tail: File close error\n");
        return 0;
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
    if (close(fd) < 0) printf("tail: File close error\n");
}