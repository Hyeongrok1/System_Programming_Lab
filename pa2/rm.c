#include <stdio.h>
#include <unistd.h>

// rm
int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("rm: missing file operand\n");
    } else if (argc != 2) {
        printf("rm usage: rm file\n");
    }

    if (unlink(argv[1]) < 0) {
        printf("rm: unlink error\n");
    }
}