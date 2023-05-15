#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    char *err_msg = NULL;
    if (argc != 2) {
        err_msg = "cat: usage: cat [file]\n";
        write(STDERR_FILENO, err_msg, sizeof(err_msg));
        exit(1);
    }
    
    int fd;
    char buf[1024] = {'\0',};
    int nbytes = 0;
    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        err_msg = "cat: No such file or directory\n";
        write(STDERR_FILENO, err_msg, sizeof(err_msg));
        exit(1);
    }

    while ((nbytes = read(fd, buf, 1024)) > 0) {
        printf("%s", buf);
        memset(buf, 0, 1024);
    }

    if (close(fd) < 0) {
        sprintf(err_msg, "cat: Error occurred: %d\n", errno);
        write(STDERR_FILENO, err_msg, strlen(err_msg));
        exit(errno);
    }
    exit(0);
}