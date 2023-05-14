#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

// cp
int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("cp: missing file operand\n");
        return 0;
    } else if (argc == 2) {
        printf("cp: missing destination file operand after 'FILE1'\n");
        return 0;
    } else if (argc != 3) {
        printf("Usage: cp file file2\n");
        return 0;
    }
    
    int fd1;
    int fd2;
    char buf[1024] = {'\0',};
    int nbytes = 0;
    if ((fd1 = open(argv[1], O_RDONLY)) < 0) {
        printf("mini: No such file or directory\n");
        return 0;        
    }
    if ((fd2 = open(argv[2], O_RDWR|O_CREAT, 0755)) < 0) {
        return 0;
    }

    while ((nbytes = read(fd1, buf, 1024)) > 0) {
        write(fd2, buf, 1024);
        memset(buf, 0, 1024);
    }
    return 1;
}