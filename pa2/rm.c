#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
// rm
int main(int argc, char *argv[]) {
    char *err_msg = NULL;

    if (argc == 1) {
        err_msg = "rm: missing file operand\n";
        write(STDERR_FILENO, err_msg, sizeof(err_msg));
        exit(1);
    } else if (argc != 2) {
        err_msg = "rm: usage: rm file\n";
        write(STDERR_FILENO, err_msg, sizeof(err_msg));
        exit(1);
    }

    if (unlink(argv[1]) < 0) {
        sprintf(err_msg, "rm: Error occurred: %d\n", errno);
        write(STDERR_FILENO, err_msg, strlen(err_msg));
        exit(errno);
    }
}