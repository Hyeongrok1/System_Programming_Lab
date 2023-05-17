#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
// rm
int main(int argc, char *argv[]) {
    char *err_msg = NULL;

    if (argc == 1) {
        errno = EINVAL;
        perror("rm");
        exit(1);
    } else if (argc != 2) {
        errno = EINVAL;
        perror("rm");
        exit(1);
    }

    if (unlink(argv[1]) < 0) {
        perror("rm");
        exit(errno);
    }
}