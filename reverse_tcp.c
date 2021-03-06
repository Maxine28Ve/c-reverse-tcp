#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

sem_t reading;
sem_t writing;
int go = 0;

void sendFileToClient( int client_sock){
	char* local_filename = calloc(256, sizeof(*local_filename));
	printf("\t--- Local Filename: ");
	scanf("%s", local_filename);
	char* remote_filename = calloc(256, sizeof(*remote_filename));
	printf("\t--- Remote Filename: ");
	scanf("%s", remote_filename);

	char* buffer;
	size_t bufsize = 32;
	FILE* fp = fopen(local_filename, "r");
	if(fp == NULL){
		write(2, "couldn't open local file\n", strlen("couldn't open local file\n"));
		return;
	}

	char* command = calloc(strlen(remote_filename) + 11, sizeof(*command));
	write(client_sock, strcat(strcat(command, "echo '' > "), remote_filename), strlen(remote_filename) + 11);
	command = realloc(NULL, strlen(remote_filename) + 11 + bufsize + 20);
	free(local_filename);
	while(getline(&buffer, &bufsize, fp) > 0){
		printf("Buffer: %s", buffer);
		strcpy(command, "echo '");
		strcat(command, buffer);
		strcat(command, "' >> ");
		strcat(command, remote_filename);
		printf("Command: %s\n", command);
		write(client_sock, command, strlen(command));
		bzero(command, strlen(command));
		bzero(buffer, strlen(buffer));
	}

	fclose(fp);

}

void* th_read(void* args){
	int client_sock = (int)((int*)args)[0];
	int status = 0;
	char* response = realloc(NULL, sizeof(*response)*900000);
	while(1){
		status = read((int)client_sock, response, 900000);
		if(status <= 0){
			perror("read failed: ");
			return NULL;
		}
		else{
			printf("%s", response);
			printf("\n");
			fflush(stdout);
			fflush(stderr);
			bzero(response, 900000);
			go = 0;
		}
	}
}

void* th_send(void* args){
	int client_sock = (int)((int*)args)[0];
	char* command_to_send = realloc(NULL, sizeof(*command_to_send)*65536);
	while(1){
		printf("\nCommand: ");
		fgets(command_to_send, 65536, stdin);
		printf("Sending %s\n", command_to_send);
		if(strcmp(command_to_send, "quit\n") == 0){
			exit(0);
		}
		if(strcmp(command_to_send, "culo\n") == 0){
			printf("nice");
			sendFileToClient(client_sock);
		}
		else{
			write((int)client_sock, command_to_send, strlen(command_to_send));
		}
		bzero(command_to_send, strlen(command_to_send));
	}
	if(command_to_send){
		free(command_to_send);
	}
}

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

	pthread_t thread_read, thread_send;
	int iret1, iret2;

	sem_init(&reading, 1, 1);
	sem_init(&writing, 1, 1);
	int args[1];
	args[0] = client_sock;
	iret2 = pthread_create(&thread_send, NULL, th_send, (void*)args);
	iret1 = pthread_create(&thread_read, NULL, th_read, (void*)args);
	if(iret1 || iret2){
		perror("Failed to start threads\n");
		exit(EXIT_FAILURE);
	}
	wait(NULL);
	pthread_join( thread_send, NULL);
	pthread_join( thread_read, NULL);

	if(command_to_send){
		free(command_to_send);
	}
}
