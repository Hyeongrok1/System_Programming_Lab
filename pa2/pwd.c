#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
    char *err_msg = NULL;

    if (argc != 1) {
        err_msg = "pwd: usage: pwd\n";
        write(STDERR_FILENO, err_msg, sizeof(err_msg));
        exit(1);
    }
    char buf[100] = {'\0',};
    getcwd(buf, 100);
    printf("%s\n", buf);
}