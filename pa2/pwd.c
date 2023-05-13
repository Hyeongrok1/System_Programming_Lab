#include <stdio.h>
#include <unistd.h>

int main (int argc, char *argv[]) {
    if (argc != 1) printf("Usage: pwd\n");
    char buf[100] = {'\0',};
    getcwd(buf, 100);
    printf("%s\n", buf);
}