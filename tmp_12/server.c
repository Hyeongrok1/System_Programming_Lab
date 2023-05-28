#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/select.h>
#define MAXLINE 80

int main (int argc, char *argv[]) {
    int n, listenfd, connfd, caddrlen;
    struct sockaddr_in saddr, caddr;
    char buf[MAXLINE];
    char username[MAXLINE];
    char data[MAXLINE];
    char start[MAXLINE];
    char end[MAXLINE];
    char **copyData;
    if (argc < 2) return 0;
    copyData = (char **)malloc(sizeof(char*) *2);
    for (int z=0; z<2; z++){
        copyData[z]= (char*)malloc(sizeof(char) * 80);
    }
    int port = atoi(argv[1]);
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket() failed.\n");
        exit(1);
    }
    memset((char *)&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);
    if (bind(listenfd, (struct sockaddr *)&saddr,sizeof(saddr)) < 0) {
        printf("bind() failed.\n");
        exit(2);
    }
    if (listen(listenfd, 5) < 0) {
        printf("listen() failed.\n");
        exit(3);
    }
    fd_set readset, copyset;
    FD_ZERO(&readset);
    FD_ZERO(&copyset);
    FD_SET(listenfd, &readset);
    int fdmax = listenfd, fdnum;
    int user_size;
    int total=0;
    while (1) {
        copyset = readset;
        if ((fdnum = select(fdmax + 1, &copyset, NULL, NULL, NULL)) < 0) {
            printf("select() failed\n");
            exit(0);
        }
        if (fdnum == 0) {
            continue;
        }        
        for (int i = 0; i < fdmax + 1; i++) {                                    
            if (FD_ISSET(i, &copyset)) {
                if (i == listenfd) {
                    if ((connfd = accept(listenfd, (struct sockaddr *)&caddr,(socklen_t *)&caddrlen)) < 0) {
                        printf ("accept() failed.\n");
                        continue;
                    }
                    total++; 
                    if ((user_size = read(connfd,username, MAXLINE)) > 0) {
                        char *name = strtok(username,"\n");
                        for(int j=0; j<= fdmax; j++){
                            if(FD_ISSET(j,&readset)) 
                            {
                                if(j != listenfd && j != i)
                                {
                                    sprintf(start,"%s joined %d current users \n", name, total);
                                    write(j,start,strlen(start));
                                }
                            }                          
                        }
                    }                                        
                    FD_SET(connfd, &readset);    
                    if (fdmax < connfd) {
                        fdmax = connfd;
                    }
                }
                else {                                  
                    if ((n = read(i, buf, MAXLINE)) > 0) {
                        char *ptr = strtok(buf,":");
                        strcpy(copyData[0],ptr);
                        ptr= strtok(NULL,"\n");
                        strcpy(copyData[1],ptr);
                        if(strcmp(copyData[0],"quit") == 0)
                        { 
                            total --;                           
                            for(int a=0; a<=fdmax; a++){
                                if(FD_ISSET(a,&readset)) 
                                {
                                    if(a != listenfd && a != i)
                                    {
                                        sprintf(end,"%s leaved %d current users\n",copyData[1], total);
                                        write(a,end,strlen(end));
                                    }
                                }                          
                            }                           
                            FD_CLR(i, &readset);                                               
                            close(i);                               
                        }
                        else{
                            int textsize=strlen(copyData[1]);                            
                            printf ("got %d bytes from user %s",textsize,copyData[0]);
                            for(int j=0; j<=fdmax; j++){
                                if(FD_ISSET(j,&readset)) 
                                {
                                    if(j != listenfd && j != i)
                                    { 
                                        char *name =strtok(copyData[0],"\n");  
                                        sprintf(data,"%s:%s\n",copyData[0], copyData[1]);
                                        write(j,data,strlen(data));                          
                                    }
                                }                          
                            }
                        }                  
                    }
                }
            }
        }
    }
    for(int b=0; b<90; b++){
        free(copyData[b]);
    }
    free(copyData);
}