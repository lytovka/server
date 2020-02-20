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
Helper function needed for dynamic memory allocation
Input: 123 (int - a number)
Output: 3 (int - number of characters in number)
*/
int content_chars_length(int n){
	int count;
 	while (n != 0) {
       		 n /= 10;
        	 ++count;
    	}
	return count;
}

/*
Headers to be displayed to client when they receive a response from server
*/
void write_headers(int client_sock, int message_length){
        struct tm strtime;
        time_t timeoftheday;
        struct tm *loc_time;
        timeoftheday=time(NULL);
        loc_time=localtime(&timeoftheday);
	
        write (client_sock,"\nHost-Name: 10.17.175.206",25);
        write(client_sock,"\nContent-Length: 80",19);
        write(client_sock,"\n",2);
        write(client_sock,asctime(loc_time),strlen(asctime(loc_time)));
        write(client_sock,"Content-type: txt/html\n",23);
        write(client_sock,"\n",2);

}

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

/*
GET request implementation
to-do: HEAD and POST
*/
void process_request(int client_sock, char* type, char* file){
	FILE* f;
	char* temp_filename = get_filename(file);	

	if((f = fopen(temp_filename,"r")) == NULL){
		fprintf(stderr, "Can't resolve the file\n");
		exit(2);
	}
	char S[1000];

	while(fgets(S, 1000, f) != NULL){

		//printf("%s\n", S);
		fflush(stdout);
	}
	write_headers(client_sock, strlen(S));
	write(client_sock, S, strlen(S));
	
	fclose(f);
	free(temp_filename);
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
		if(strcmp(client_message, "\0") == 0){
			printf("Empty message\n");
		}
		else{
			puts(client_message);
		}
		fflush(stdout);

	//divide incoming string into tokens

		char* token = strtok(client_message, " ");
		int i = 1;
		while (token != NULL) { 
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
    		}
			printf("struct: %s %s %s \n", client_request.type, client_request.path, client_request.protocol);
			fflush(stdout);
			process_request(client_sock, client_request.type, client_request.path);
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
