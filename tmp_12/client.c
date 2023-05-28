#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include<fcntl.h>
#include <sys/select.h>

#define MAXLINE 80
int main (int argc, char *argv[]) {
    int n, cfd;
    struct hostent *h;
    struct sockaddr_in saddr;
    char buf[MAXLINE];
    char recv_buf[MAXLINE];
    char username[MAXLINE];
    char content[MAXLINE];
    char quit[MAXLINE];
    char input[]="Insert your name : ";
    char *host = argv[1];
    int port = atoi(argv[2]);   
    int fd,n_size,w_size,r_size;
    write(STDOUT_FILENO,input,sizeof(input));
    n_size=read(STDIN_FILENO,username,MAXLINE);              
    if ((cfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket() failed.\n");
        exit(1);
    }
    if ((h = gethostbyname(host)) == NULL) {
        printf("invalid hostname %s\n", host);
        exit(2);
    }
    memset((char *)&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    memcpy((char *)&saddr.sin_addr.s_addr, (char *)h->h_addr_list[0], h->h_length);
    saddr.sin_port = htons(port);
    if (connect(cfd,(struct sockaddr *)&saddr,sizeof(saddr)) < 0) {
        printf("connect() failed.\n");
        exit(3);
    }
    write(cfd, username, n_size);
    int fdmax= cfd;
	fd_set master;
	fd_set read_fds;
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	FD_SET(0,&master);
	FD_SET(cfd,&master);  
    fdmax =cfd;       
    while(1){  
        read_fds = master;
		if(select(fdmax+1,&read_fds,NULL,NULL,NULL) == -1)
		{
			perror("select");
			exit(4);
		}
		for(int i = 0; i <= fdmax; i++){   
            memset(buf,0,MAXLINE);      
			if(FD_ISSET(i,&read_fds)){
                if(i == 0)
                {
                    r_size=read(STDIN_FILENO,buf,MAXLINE);   
                    if(strcmp(buf,"quit\n") == 0)
                    { 
                        sprintf(quit,"%s:%s","quit", username);
                        write(cfd, quit, strlen(quit));   
                        exit(0);
                    }
                    else{
                        sprintf(content,"%s:%s",username,buf);
                        write(cfd, content, strlen(content));   
                    }      
                }
                else
                {
                    w_size= read(cfd,recv_buf,MAXLINE);
                    write(STDOUT_FILENO, recv_buf, w_size);
                    fflush(stdout);
                }              
            }      
        }
    }   
    close(cfd);    
}    