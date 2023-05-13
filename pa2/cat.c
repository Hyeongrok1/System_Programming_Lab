#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: cat [file]\n");
        return 0;
    }
    
    int fd;
    char buf[1024] = {'\0',};
    int nbytes = 0;
    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        printf("mini: No such file or directory\n");
        return 0;        
    }

    while ((nbytes = read(fd, buf, 1024)) > 0) {
        printf("%s", buf);
        memset(buf, 0, 1024);
    }
    printf("\n");
    return 1;
}