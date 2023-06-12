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
// use when access user_of_seat
pthread_mutex_t seat_lock[MAX_SEAT] = {PTHREAD_MUTEX_INITIALIZER,};
pthread_mutex_t user_login_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t passwd_of_user_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t user_of_seat_lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct _query {
    int user;   // user ID 0~1023
    int action; // action ID 1~5 (0 for termination)
    int data;   // seat
} query;
// If the data field is our of range, server returns -1

// check if user is logged in
int is_user_login[1024];

// store the password of user
int passwd_of_user[1024];

// store the userId of the seat
int user_of_seat[257];

// check if the thread_t is used
int thread_using_flag[1024];

/**
 * 1. Log in  -data: passwd
 * 2. Reserve -data: seat num
 * 3. Check reservation -data: N/A
 * 4. Cancel reservation -data: seat num
 * 5. Log out -data: N/A
*/

void *client_thread(void *arg) {
    // get the connected_fd
    
    int connected_fd = *((int*) arg);
    int receive = 0;
    query *recv_query = (query *) malloc(sizeof(query));
    char return_buffer[2048];
    int success = 0;
    int is_logged_in = -1;
    while (1) {
        memset(return_buffer, 0, 2048);

        // receive from the connected_fd
        if ((receive = recv(connected_fd, recv_query, sizeof(query), 0)) > 0) {
            if (recv_query->user < 0 || recv_query->user > 1023 
                || recv_query->action < 0 || recv_query->action > 5) {
                    send(connected_fd, "-1", 3, 0);
                    continue;
            }

            // exit
            if (recv_query->action == EXIT && recv_query->user == EXIT && recv_query->data == EXIT) {

                for (int i = 0; i < 1024; i++) {
                    pthread_mutex_lock(&user_login_lock);
                    if (is_user_login[i] == connected_fd) {
                        is_user_login[i] = -1;
                    }
                    pthread_mutex_unlock(&user_login_lock);
                }
                
                sprintf(return_buffer, "%d", 256);
            } else if (recv_query->action == LOGIN) {   // LOGIN
                int user_id = recv_query->user;
                int passwd = recv_query->data;
                
                success = 1;

                // passwd should not be negative
                if (passwd < 0) success = 0;
                
                // If the user is already logged in, fail
                pthread_mutex_lock(&user_login_lock);
                if (is_user_login[user_id] != -1) success = 0;
                pthread_mutex_unlock(&user_login_lock);

                pthread_mutex_lock(&passwd_of_user_lock);
                // If passwd is -1, it is first log in
                if (success == 1) {
                    if (passwd_of_user[user_id] == -1) {
                        passwd_of_user[user_id] = passwd;
                    } else { // Else if passwd is not -1, check if passwd is right
                        if (passwd_of_user[user_id] != passwd) success = 0;
                    }
                }
                pthread_mutex_unlock(&passwd_of_user_lock);

                // If successful, return_buffer is "1"
                if (success) {
                    pthread_mutex_lock(&user_login_lock);
                    is_user_login[user_id] = connected_fd;
                    pthread_mutex_unlock(&user_login_lock);

                    is_logged_in = user_id;
                    strcpy(return_buffer, "1");
                } else { // else return_buffer is "-1"
                    strcpy(return_buffer, "-1");
                }
            } else if (recv_query->action == RESERVE) { // RESERVE
                int user_id = recv_query->user;
                int seat_num = recv_query->data;
                success = 1;

                // if current logged in ID is not the requested id, fail
                if (is_logged_in != user_id) success = 0;

                // if the seat num is invalid, fail
                if (seat_num < 0 || seat_num > 255) success = 0;
                if (is_user_login[user_id] != connected_fd) success = 0;

                // if the seat is already reserved by another user
                pthread_mutex_lock(&seat_lock[seat_num]);
                if (user_of_seat[seat_num] != -1 && user_of_seat[seat_num] != user_id) {
                    success = 0;
                }
                
                if (success) {
                    user_of_seat[seat_num] = user_id;
                    sprintf(return_buffer, "%d", seat_num);
                } else {
                    strcpy(return_buffer, "-1");
                }
                pthread_mutex_unlock(&seat_lock[seat_num]);
            } else if (recv_query->action == CHECK_RSV) { // CHECK_RSV
                int user_id = recv_query->user;            
                success = 1;

                // if current logged in ID is not the requested id, fail
                if (is_logged_in != user_id) success = 0;
                int seat_arr[256];
                int seat_cnt = 0;

                if (is_user_login[user_id] != connected_fd) success = 0;

                // Check all seats
                for (int i = 0; i < MAX_SEAT; i++) {
                    pthread_mutex_lock(&seat_lock[i]);
                    if (user_of_seat[i] == user_id) {
                        seat_arr[seat_cnt++] = i;
                    }
                    pthread_mutex_unlock(&seat_lock[i]);
                }
                // if the user has no reserved seats
                if (seat_cnt == 0) success = 0;

                if (success) {
                    // use loop and sprintf
                    sprintf(return_buffer, "%s%d", "Reservation: ", seat_arr[0]);
                    for (int i = 1; i < seat_cnt; i++) {
                        sprintf(return_buffer, "%s, %d", return_buffer, seat_arr[i]);
                        printf("%s", return_buffer);
                    }
                } else {
                    strcpy(return_buffer, "-1");
                }
            } else if (recv_query->action == CANCEL_RSV) { // CANCEL_RSV
                int user_id = recv_query->user;
                int seat_num = recv_query->data;
                success = 1;

                // if current logged in ID is not the requested id, fail
                if (is_logged_in != user_id) success = 0;

                // if the seat num is invalid, fail
                if (seat_num < 0 || seat_num > 255) success = 0;
                // if the user request before log in
                else if (is_user_login[user_id] != connected_fd) success = 0;
                // if the user of seat is not user_id
                else if (user_of_seat[seat_num] != user_id) success = 0;

                if (success) {
                    pthread_mutex_lock(&seat_lock[seat_num]);
                    user_of_seat[seat_num] = -1;
                    pthread_mutex_unlock(&seat_lock[seat_num]);
                    sprintf(return_buffer, "%d", seat_num);
                } else {
                    strcpy(return_buffer, "-1");
                }
            } else if (recv_query->action == LOGOUT) { // LOGOUT
                int user_id = recv_query->user;
                success = 1;
                if (is_logged_in != user_id) success = 0;
                if (is_user_login[user_id] != connected_fd) success = 0;

                if (success) {
                    strcpy(return_buffer, "1");
                    pthread_mutex_lock(&user_login_lock);
                    is_user_login[user_id] = -1;
                    pthread_mutex_unlock(&user_login_lock);
                } else {
                    strcpy(return_buffer, "-1");
                }
            }

            send(connected_fd, return_buffer, 2048, 0);
            if (recv_query->action == EXIT && recv_query->user == EXIT && recv_query->data == EXIT) break;
        }
    }

    pthread_mutex_lock(&thread_num_lock);
    for (int i = 0; i < 1024; i++) {
        if (thread_using_flag[i] == connected_fd) {
            thread_using_flag[i] = -1;
            break;
        }
    }
    pthread_mutex_unlock(&thread_num_lock);
    free(recv_query);
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

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[1]));
    serverAddr.sin_addr.s_addr = htons(INADDR_ANY);

    // bind
    if(bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        printf("Server: bind failed\n");
        exit(2);
    }

    // listen
    // upto 1024 clients
    if(listen(serverSocket, 1024) < 0) {
        printf("Server: listen failed\n");
        exit(2);
    }
    /*
     * Insert your PA3 server code
     *
     * You should generate thread when new client accept occurs
     * and process query of client until get termination query
     *
     */
    
    // There can be upto 1024 threads
    pthread_t threads[1024];
    int connected_fd = 0;
    int connected_address_length = 0;
    int thread_num = 0;

    // the sending value to new thread
    int *send_val;

    // initialize the global array 
    memset(is_user_login, -1, sizeof(is_user_login));
    memset(passwd_of_user, -1, sizeof(passwd_of_user));
    memset(user_of_seat, -1, sizeof(user_of_seat));
    memset(thread_using_flag, -1, sizeof(thread_using_flag));

    // infinite loop
    while(1) {
        connected_address_length = sizeof(clientAddr);
        if ((connected_fd = accept(serverSocket, (struct sockaddr *) &clientAddr, (socklen_t *) &connected_address_length)) < 0) {
            printf("accept() failed.\n");
            continue;
        }

        pthread_mutex_lock(&thread_num_lock);   
        send_val = &connected_fd;
        for (int i = 0; i < 1024; i++) {
            if (thread_using_flag[i] == -1) {
                thread_num = i;
                thread_using_flag[i] = *send_val;
                break;
            }
        }

        // create thread
        if (pthread_create(&threads[thread_num], NULL, client_thread, (void *) send_val)) {
            printf("pthread_create err\n");
            continue;
        }
        pthread_mutex_unlock(&thread_num_lock);
    }

    return 0;
}
