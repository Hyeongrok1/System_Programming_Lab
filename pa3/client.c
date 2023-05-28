#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct _query {
    int user;
    int action;
    int seat;
} query;

int main(int argc, char *argv[]) {
    int client_socket = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;

    if (argc == 3) {
        /* Insert your code for terminal input */
    } else if (argc == 4) {
        /* Insert your code for file input */
    } else {
        printf("Follow the input rule below\n");
        printf("==================================================================\n");
        printf("argv[1]: server address, argv[2]: port number\n");
        printf("or\n");
        printf("argv[1]: server address, argv[2]: port number, argv[3]: input file\n");
        printf("==================================================================\n");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        printf("Connection failed\n");
        exit(1);
    }

    /*
     * Insert your PA3 client code
     *
     * You should handle input query
     *
     * Follow the print format below
     *
     * 1. Log in
     * - On success
     *   printf("Login success\n");
     * - On fail
     *   printf("Login failed\n");
     *
     * 2. Reserve
     * - On success
     *   printf("Seat %d is reserved\n");
     * - On fail
     *   printf("Reservation failed\n");
     *
     * 3. Check reservation
     * - On success
     *   printf("Reservation: %s\n");
     *   or
     *   printf("Reservation: ");
     *   printf("%d, ");
     *   ...
     *   printf("%d\n");
     * - On fail
     *   printf("Reservation check failed\n");
     *
     * 4. Cancel reservation
     * - On success
     *   printf("Seat %d is canceled\n");
     * - On fail
     *   printf("Cancel failed\n");
     *
     * 5. Log out
     * - On success
     *   printf("Logout success\n");
     * - On fail
     *   printf("Logout failed\n");
     *
     */

    close(client_socket);

    return 0;
}
