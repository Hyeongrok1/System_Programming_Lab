#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include <stdio.h>

#define MAX_SEAT 256
#define MAX_CLIENT 1024

#define EXIT 0
#define LOGIN 1
#define RESERVE 2
#define CHECK_RSV 3
#define CANCEL_RSV 4
#define LOGOUT 5

pthread_mutex_t thread_num_lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct _query {
    int user;   // user ID 0~1023
    int action; // action ID 1~5 (0 for termination)
    int data;   // seat
} query;
// If the data field is our of range, server returns -1

int client_of_user[1024];
int thread_num_flag[1024];
int user_of_seat[256];

/**
 * 1. Log in  -data: passwd
 * 2. Reserve -data: seat num
 * 3. Check reservation -data: N/A
 * 4. Cancel reservation -data: seat num
 * 5. Log out -data: N/A
*/

void *client_thread(void *arg) {
    int connected_fd = *((int*) arg);
    int num_bytes = 0;
    int fd = 0;
    char buffer[512];
    char return_buffer[512];
    char *query_argv[3];
    char *ptr = NULL;
    int query_argc = 0;
    int success = 0;
    query new_query;
    while (1) {
        memset(buffer, 0, 512);
        memset(return_buffer, 0, 512);

        if ((num_bytes = read(connected_fd, buffer, 512)) > 0) {
            buffer[num_bytes-1] = '\0';
            
            query_argc = 0;
            ptr = strtok(buffer, ":");
            while (ptr != NULL) { 
                query_argv[query_argc++] = ptr;
                ptr = strtok(NULL, ":");
            }

            new_query.user = atoi(query_argv[0]);
            new_query.action = atoi(query_argv[1]);
            new_query.data = atoi(query_argv[2]);
            
            // exit
            if (new_query.action == EXIT) {

            } else if (new_query.action == LOGIN) {
                int passwd = new_query.data;

                if (success) {
                    strcpy(return_buffer, "1");
                } else {
                    strcpy(return_buffer, "-1");
                }
            } else if (new_query.action == RESERVE) {
                int seat_num = new_query.data;

                if (success) {
                    strcpy(return_buffer, "seat_num");
                } else {
                    strcpy(return_buffer, "-1");
                }
            } else if (new_query.action == CHECK_RSV) {

                if (success) {
                    strcpy(return_buffer, "reservation seats");
                    // use loop and sprintf
                } else {
                    strcpy(return_buffer, "-1");
                }
            } else if (new_query.action == CANCEL_RSV) {
                int seat_num = new_query.data;

                if (success) {
                    strcpy(return_buffer, "seat num");
                } else {
                    strcpy(return_buffer, "-1");
                }
            } else if (new_query.action == LOGOUT) {
                if (success) {
                    strcpy(return_buffer, "1");
                } else {
                    strcpy(return_buffer, "-1");
                }
            }

            write(connected_fd, return_buffer, 512);
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{

    if (argc < 2) {
        printf("Received %d arguments. Please enter port number\n", argc-1);
        exit(1);
    }
    int serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr, clientAddr;
    pthread_t threads[1024];
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[1]));
    serverAddr.sin_addr.s_addr = htons(INADDR_ANY);

    if(bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
	    return 0;

    if(listen(serverSocket, 1024) < 0)
	    return 0;

    /*
     * Insert your PA3 server code
     *
     * You should generate thread when new client accept occurs
     * and process query of client until get termination query
     *
     */
    int connected_fd = 0;
    int connected_address_length = 0;
    int thread_num = 0;
    int *send_val;
    while(1) {
        connected_address_length = sizeof(clientAddr);
        if ((connected_fd = accept(serverSocket, (struct sockaddr *) &clientAddr, (socklen_t *) &connected_address_length)) < 0) {
            printf("accept() failed.\n");
            continue;
        }

        // lock
        pthread_mutex_lock(&thread_num_lock);
        for (int i = 0; i < 1024; i++) {
            if (thread_num_flag[i] == 0) {
                thread_num_flag[i] = 1;  
                thread_num = i;
            }
        }
        // unlock
        pthread_mutex_unlock(&thread_num_lock);
        send_val = &connected_fd;
        if (pthread_create(&threads[thread_num], NULL, client_thread, (void *) send_val)) {
            printf("pthread_create err\n");
            continue;
        }
    }

    return 0;
}
