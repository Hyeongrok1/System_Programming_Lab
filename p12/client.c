#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>

#define MAXLINE 80

int main (int argc, char *argv[]) {
    ssize_t num_bytes;
    int connected_fd;
    struct hostent *host_entry;
    struct sockaddr_in socket_address;
    char buffer[MAXLINE];

    if (argc < 3) {
        printf("Received %d arguments. Please enter host address and port number!\n", argc - 1);
        exit(1);
    }

    char* host = argv[1];
    uint16_t port = (uint16_t)strtol(argv[2], NULL, 10);

    if ((connected_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printf("socket() failed.\n"); 
        exit(2);
    }

    if ((host_entry = gethostbyname(host)) == NULL) {
        printf("invalid hostname %s\n", host); 
        exit(3);
    }

    memset((char *)&socket_address, 0, sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    memcpy((char *)&socket_address.sin_addr.s_addr, (char *)host_entry->h_addr_list[0], (size_t) host_entry->h_length);
    socket_address.sin_port = htons(port);

    char msg[MAXLINE*2] = {'\0',};
    char usr_name[MAXLINE] = {'\0',};
    write(1, "Insert your name : ", 20);
    num_bytes = read(0, usr_name, MAXLINE);
    usr_name[num_bytes-1] = '\0';
    if (connect(connected_fd,(struct sockaddr *)&socket_address,sizeof(socket_address)) < 0) {
        printf("connect() failed.\n");
        exit(4);
    }
    
    write(connected_fd, usr_name, num_bytes);
    int fd_max, fd_num;

    fd_set read_set, copy_set;
    FD_ZERO(&read_set);
    FD_SET(0, &read_set);
    FD_SET(connected_fd, &read_set);
    fd_max = connected_fd;
    while (1) {
        copy_set = read_set;

        if ((fd_num = select(fd_max+1, &copy_set, NULL, NULL, NULL)) < 0) {
            printf("select() failed.\n");
            exit(5);
        }
        for (int i = 0; i < fd_max + 1; i++) {
            memset(buffer, 0, MAXLINE);
            if (FD_ISSET(i, &copy_set)) {
                if (i == STDIN_FILENO) {
                    if ((num_bytes = read(STDIN_FILENO, buffer, MAXLINE) > 0)) {
                        buffer[strlen(buffer)] = '\0';
                        sprintf(msg, "%s:%s", usr_name, buffer);
                        if (strcmp(buffer, "quit\n") == 0) {
                            write(connected_fd, msg, strlen(msg));
                            exit(0);
                        }
                        write(connected_fd, msg, strlen(msg));
                    }
                } else {
                    if ((num_bytes = read(connected_fd, buffer, MAXLINE)) > 0) {
                        write(STDOUT_FILENO, buffer, num_bytes);
                        fflush(stdout);
                    }
                }
            }
        }
    }

    close(connected_fd);

    return 0;
}
