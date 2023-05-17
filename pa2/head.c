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
            perror("head");
            exit(1);
        }
    } else if (argc == 4) {
        if ((fd = open(argv[3], O_RDONLY)) < 0) {
            perror("head");
            exit(1);
        }
    } else {
        errno = EINVAL;
        perror("head");
        exit(1);
    }

    if (argc == 1) {
        errno = EINVAL;
        perror("head");
        exit(1);
    } else if (argc == 2) line = 10;
    else if (argc == 4) {
        if (strcmp(argv[1], "-n") != 0) {
            errno = EINVAL;
            perror("head");
            exit(1);
        }
        line = atoi(argv[2]);
    }
    else {
        if (close(fd) < 0) {
            perror("head");
            exit(errno);
        }
        exit(0);
    }

    if (line == 0) exit(0);

    int i = 0;
    while (read(fd, &c, 1) != 0) {
        if (c == '\n') i++;
        if (i == line) {
            printf("\n");
            break;
        }
        printf("%c", c);
    }
    if (close(fd) < 0) {
        perror("head");
        exit(errno);
    }

    return 0;
}