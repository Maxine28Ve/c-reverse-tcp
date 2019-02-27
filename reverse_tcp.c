#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char** argv){
	short int portno;
	if(argc < 2){
		portno = 65000;
	}
	else{
		portno = atoi(argv[1]);
	}

	int server_sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(portno);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
		perror("bind() error");
	}

	listen(server_sock, 10);
	int client_sock;
	struct sockaddr_in client_addr;
	int c = sizeof(struct sockaddr_in);
	client_sock = accept(server_sock, (struct sockaddr *)&client_addr, (socklen_t *)&c);
    if(client_sock < 0){
    	perror("Accept: ");
        exit(EXIT_FAILURE);
    }
	printf("Client connected\n");

	char* command_to_send = malloc(sizeof(char*)*65536);
	 bzero(command_to_send, 65536);
	char* response = malloc(sizeof(char*)*128000);
	 bzero(response, 128000);
	while(1){
		printf("Command: ");
		fgets(command_to_send, 65536, stdin);
		if(strcmp(command_to_send, "quit") == 0)
			break;
		write(client_sock, command_to_send, strlen(command_to_send));
		if(read(client_sock, response, 128000) < 0){
			perror("read failed: ");
			break;
		}
		else{
			printf("Result:\n");
			printf("%s", response);
		}
		fflush(stdout);
		fflush(stdin);
		bzero(command_to_send, 65536);
		bzero(response, strlen(response));
	}
	close(client_sock);
	if(command_to_send){
		free(command_to_send);
	}
}
