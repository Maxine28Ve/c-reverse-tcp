#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

short int port;
int main(int argc, char** argv){

	unsigned short int startport = 3306;
	int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(startport);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    while(bind(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
    	if(startport <= 65535){
		    server_addr.sin_port = htons(++startport);
    	}
    }
    printf("Found a port: %hu\n", startport);
    port = startport;

	int pid = fork();
	if(pid == 0){
		pid = fork();
		if(pid == 0){
			pid = fork();
		}
	}
	int i = 0;
	sleep(2);
	if(pid != 0){
		printf("Process pid: %d\n", getpid());
	    server_addr.sin_port = htons(port);
		while(i < 10){
			printf("Process %d tries to acquire the port %d\n", getpid(), port);
		    while(bind(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
				printf("Process %d failed to connect\n", getpid());
				sleep(rand()%3);
		    }
		    return 0;
		}
		printf("End %d\n", getpid());
	}


	return 0;
}
