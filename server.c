#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h> 
#include <regex.h>

struct Request{
	char* type;
	char* path;
	char* protocol;
}client_request;

/*
INPUT: char* file contains name of a file that's being read as a part of client's request.
char* file always starts with '/' character.
OUTPUT: char* get_filename returns the name of file without '/' character. 
*/
char* get_filename(char* file){
	char* new_file = (char*)malloc(strlen(file)-1);
	strncpy(new_file, file + 1, strlen(file) - 1);

	return new_file;
}

//GET request implementation
void process_request(char* type, char* file){
	FILE* f;

	if((f = fopen(get_filename(file), "r")) == NULL){
		fprintf(stderr, "Can't resolve the file\n");
		exit(2);
	}
	char ch = getc(f);
	while(ch != EOF){
		putchar(ch);
		ch = getc(f);
	}
}

void clear_buffer(){
	int c;
	while ((c = getchar()) != '\n' && c != EOF) { }
}

int main(int argc, char *argv[]){

	//Create a socket

	int socket_desc;
	int client_sock, c, read_size;
	char client_message[1000];
	struct sockaddr_in server, client;
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);

	if(socket_desc == -1){
		printf("Could not create a socket/n");
		return 1;
	}

	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_family = AF_INET;
	server.sin_port = htons(8888);

	//bind socket
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
       	       perror("bind failed");
               return 1;
 }
    	puts ("Bind successful");
	
	//listen to socket

	listen(socket_desc, 3);

        //Accept incoming
   	puts("Waiting for incoming connections ... .. . ..");
	
	c=sizeof(struct sockaddr_in);

        //connect
   	client_sock=accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c);
   	 if (client_sock < 0){
       		 perror("accept failed");
        	 return 1;
 }
   	 puts("Connection accepted");

	//main loop to handle client requests
	while((read_size = recv(client_sock, client_message, sizeof(client_message)-1,0)) > 0){
		fflush(stdin);
		if(strcmp(client_message, "\0") == 0){
			printf("Empty message\n");
		}
		else{
			puts(client_message);
		}
		fflush(stdin);

	//divide incoming string into tokens

		char* token = strtok(client_message, " ");
		int i = 1;
		while (token != NULL) { 
        	//	printf("%s, len - %d\n", token, strlen(token));
			if(strcmp(token, "GET") == 0 || strcmp(token, "POST") == 0 || strcmp(token, "HEAD") == 0){
				client_request.type = token;
		}	
			else if(i == 2){
                                client_request.path = token;
                }

			else if(i == 3){
				client_request.protocol = token;
		}
        		token = strtok(NULL, " ");
			++i;
			//clear_buffer(); 
    		}
			printf("struct: %s %s %s \n", client_request.type, client_request.path, client_request.protocol);
			fflush(stdin);
			process_request(client_request.type, client_request.path);
			//char* new_str = (get_filename(client_request.path));
			//printf("new name is: %s\n", new_str);		 
}
	if(read_size==0){
       		 puts("Client disconnected");	
    }
    	else if(read_size==-1){
        	perror("receive failed");
   }
close(socket_desc);
return 0;
}
