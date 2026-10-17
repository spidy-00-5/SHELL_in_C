#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>
#include<unistd.h>

#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

void lsh_loop();
char* lsh_read_line();
char** lsh_split_line(char* line);
int lsh_launch(char** argc);
int lsh_execute(char** args);

int main(int argc , char** argv){
	lsh_loop();
	return EXIT_SUCCESS;
}

void lsh_loop(){
	char* line;
	char** args;
	int status;

	do{
		printf("> ");
		line = lsh_read_line();
		args = lsh_split_line(line);
		status = lsh_execute(args);

		free(line);
		free(args);
	}while(status);
}
char* lsh_read_line(){
	
	int bufsize = LSH_RL_BUFSIZE;
	int position = 0;
	char* buffer = malloc(sizeof(*buffer)*bufsize);
	int c;

	while(1){
		c = getchar();

		if(c == EOF || c == '\n'){
			buffer[position] = '\0';
			return buffer;
		}else{
			buffer[position] = c;
		}
		position++;

		if(position >= bufsize){
			bufsize += LSH_RL_BUFSIZE;
			buffer = realloc(buffer,bufsize);

			if(!buffer){
				fprintf(stderr , "lsh:alloction error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

char** lsh_split_line(char* line){

	int bufsize = LSH_TOK_BUFSIZE , position = 0;
	char** tokens = malloc(sizeof(**tokens) * bufsize);
	char* token;

	if(!tokens){
		fprintf(stderr , "lsh:allocation failure\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line , LSH_TOK_DELIM);
	while(token != NULL){

		tokens[position++] = token;

		if(position >= bufsize){
			bufsize += LSH_TOK_BUFSIZE;
			tokens = realloc(tokens , bufsize);

			if(!tokens){
				fprintf(stderr , "lsh:allocation failure\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL , LSH_TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}

int lsh_launch(char** args){
	pid_t pid , wpid;
	int status;

	pid = fork();

	if(pid == 0){
		//child process;
		
		if(execvp(args[0],args) == -1){
			perror("lsh");
		}
	}else if(pid < 0){
		perror("lsh");
	}else{
		do{
			wpid = waitpid(pid,&status,WUNTRACED);
		}while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	
	return 1;
}

int lsh_execute(char **args){

	if(args[0] == NULL){
		return 1;
	}else if(strcmp("exit",args[0]) == 0){
		return 0;
	}
	return lsh_launch(args);
}
