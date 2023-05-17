#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main (int argc, char *argv[]) {
    char *err_msg = NULL;

    if (argc != 1) {
        errno = EINVAL;
        perror("pwd");
        exit(1);
    }
    char buf[100] = {'\0',};
    getcwd(buf, 100);
    printf("%s\n", buf);
}