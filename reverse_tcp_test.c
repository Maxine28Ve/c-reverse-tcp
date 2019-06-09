#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>


int main(int argc, char** argv){
	unsigned short int portno;
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
		perror("bind:");
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

	printf("\nClient connected\n");

	char* command_to_send = calloc(65536, sizeof(char*));
	char* response = calloc(128000, sizeof(char*));
	int status = 1;
	int error = 0;
	socklen_t len = sizeof (error);
	int retval = getsockopt (client_sock, SOL_SOCKET, SO_ERROR, &error, &len);
	while(status > 0){
		printf("\nCommand: ");
		fgets(command_to_send, 65536, stdin);
  	if(strcmp(command_to_send, "__quit__") == 0){
			return 1;
		}
		printf("__ sending command __\n");
		write(client_sock, command_to_send, strlen(command_to_send));
		printf("__ Command sent. Now reading __\n");
		while((status = read(client_sock, response, 900000)) > 0){
			printf("%s", response);
		}
		printf("\n");
 //		fflush(stdout);
 //		fflush(stderr);

 		bzero(response, 900000);
 		bzero(command_to_send, strlen(command_to_send));
 	}

	if(command_to_send){
		free(command_to_send);
	}
	if(response){
		free(response);
	}
	return 0;
}
