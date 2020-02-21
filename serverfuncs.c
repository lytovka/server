#include "server.h"

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

int send_status(char operation, int succeed){
	if(operation == 'r' && succeed){
		return OK;		
	}
	return 0;
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
	
	char message [21];
	sprintf(message, "\nContent-Length: %d", message_length);

        write (client_sock,"\nHost-Name: 10.17.175.206",25);
        write(client_sock,message,22);
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

void send_get(int client_sock, char* file){
	FILE* f;
	char* temp_filename = get_filename(file);	
	int succeed = 1;

	if((f = fopen(temp_filename,"r")) == NULL){
		fprintf(stderr, "Can't resolve the file\n");
		succeed = 0;
	}
	char S[1000];

	while(fgets(S, 1000, f) != NULL){
		fflush(stdout);
	}
	int code = send_status('r', succeed);
	char status_code_message[20];
	sprintf(status_code_message, "\nStatus Code: %d", code);

	write(client_sock, status_code_message, strlen(status_code_message));
	write_headers(client_sock, strlen(S));
	write(client_sock, S, strlen(S));
	
	fclose(f);
	free(temp_filename);
}

void send_head(int client_sock, char* file){
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

	fclose(f);
	free(temp_filename);
}

/*
GET request implementation
to-do: HEAD and POST
*/
void process_request(int client_sock, char* type, char* file){
	if(strcmp(type, "GET") == 0){
		send_get(client_sock, file);
	}
	if(strcmp(type, "HEAD") == 0){
		send_head(client_sock, file);
	}
}

void clear_buffer(){
	int c;
	while ((c = getchar()) != '\n' && c != EOF) { }
}
