#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#define EXIT 0
#define LOGIN 1
#define RESERVE 2
#define CHECK_RSV 3
#define CANCEL_RSV 4
#define LOGOUT 5

typedef struct _query {
    int user;       // user ID 0~1023
    int action;     // action ID 1~5 (0 for termination)
    int data;       // data
} query;

int main(int argc, char *argv[]) {
    int client_socket = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;

    int fd = 0;
    int nbytes = 0;
    char buf[6*100];
    char *queries[100];
    int query_list[100][3] = {0,};
    memset(query_list, -1, sizeof(query_list));

    int query_num = 0;
    if (argc == 3) {
        /* Insert your code for terminal input */
    } else if (argc == 4) {
        /* Insert your code for file input */
        if ((fd = open(argv[3], O_RDONLY)) < 0) {
            perror("open");
            exit(1);
        }

        while ((nbytes = read(fd, buf, sizeof(buf))) > 0) {
            buf[nbytes] = '\0';
            char *ptr = strtok(buf, "\n");
            while (ptr != NULL) {
                queries[query_num++] = ptr;
                ptr = strtok(NULL, "\n");                
            }
            queries[query_num] = NULL;          
        }

        for (int i = 0; i < query_num; i++) {
            int argc_tmp = 0;
            char *ptr = strtok(queries[i], " ");
            while (ptr != NULL) {
                query_list[i][argc_tmp++] = atoi(ptr);
                ptr = strtok(NULL, " ");
            }
        }

        // lastly, exit query
        query_list[query_num][0] = 0;
        query_list[query_num][1] = 0;
        query_list[query_num][2] = 0;

        close(fd);
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

    char *small_query_list[3];
    char query_line[1024];
    char buffer[2048];
    query_num = 0;
    while (1) {
    /*
     * Insert your PA3 client code
     *
     * You should handle input query
     *
     * Follow the print format below
     */
        query send_query;  

        if (argc == 3) {
            memset(query_line, 0, 1024);
            // read the query data from stdin
            if ((nbytes = read(STDIN_FILENO, query_line, 1024)) < 0) {
                printf("Client: read failed.\n");
            }

            // make a query
            int num = 0;
            char *ptr = strtok(query_line, " ");
            while (ptr != NULL) {
                small_query_list[num++] = ptr;
                ptr = strtok(NULL, " ");
            }
            if (num != 3) {
                printf("Invalid query\n");
                continue;
            }
            
            // server will judge if the query data is invalid
            send_query.user = atoi(small_query_list[0]);
            send_query.action = atoi(small_query_list[1]);
            send_query.data = atoi(small_query_list[2]);
        } else if (argc == 4) {  
            send_query.user = query_list[query_num][0];
            send_query.action = query_list[query_num][1];
            send_query.data = query_list[query_num][2];
            query_num++;
            if (send_query.data == -1) {
                printf("Invalid query\n");
                continue;
            }
        }

        memset(buffer, 0, 2048);
        // make query
        sprintf(buffer, "%d:%d:%d", send_query.user, send_query.action, send_query.data);

        // send query
        send(client_socket, &send_query, sizeof(query), 0);

        memset(buffer, 0, 2048);

        // receive response of server
        recv(client_socket, buffer, sizeof(buffer), 0);
        if (send_query.action == LOGIN) {
            if (atoi(buffer) == 1) 
                printf("Login success\n");
            else if (atoi(buffer) == -1) 
                printf("Login failed\n");        
        } else if (send_query.action == RESERVE) {
            int seat_reserve = atoi(buffer);
            if (seat_reserve > 0) 
                printf("Seat %d is reserved\n", seat_reserve);
            else    
                printf("Reservation failed\n");
        } else if (send_query.action == CHECK_RSV) {
            if (strcmp(buffer, "-1") == 0) {
                printf("Reservation check failed\n");
            } else {
                printf("%s\n", buffer);
            }
        } else if (send_query.action == CANCEL_RSV) {
            int seat_cancel = atoi(buffer);
            if (seat_cancel > 0) 
                printf("Seat %d is canceled\n", seat_cancel);
            else 
                printf("Cancel failed\n");
        } else if (send_query.action == LOGOUT) {
            if (atoi(buffer) == 1) 
                printf("Logout success\n");
            else 
                printf("Logout failed\n");
        } else if (send_query.user == EXIT && send_query.action == EXIT && send_query.data == EXIT) {
            if (atoi(buffer) == 256) break;
        } else {
            if (atoi(buffer) == -1) {
                printf("Invalid query\n");
            }
        }
        
     /*
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
    }

    close(client_socket);

    return 0;
}
