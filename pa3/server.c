#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct _query {
    int user;
    int action;
    int data;
} query;

int main(int argc, char* argv[])
{
    int serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;

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

    return 0;
}
