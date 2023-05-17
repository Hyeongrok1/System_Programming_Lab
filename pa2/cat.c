#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    char *err_msg = NULL;
    if (argc != 2) {
        errno = EINVAL;
        perror("cat");
        exit(1);
    }
    
    int fd;
    char buf[1024] = {'\0',};
    int nbytes = 0;
    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        perror("cat");
        exit(1);
    }

    while ((nbytes = read(fd, buf, 1024)) > 0) {
        printf("%s", buf);
        memset(buf, 0, 1024);
    }

    if (close(fd) < 0) {
        perror("cat");
        exit(errno);
    }
    printf("\n");
    exit(0);
}