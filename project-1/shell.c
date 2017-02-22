#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

/* array that includes all legal characters in the shell */
char LEGAL_CHARS[71] = {
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
	'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
	'0','1','2','3','4','5','6','7','8','9','0',
	'-','.','_'
};

const char PIPE[] 				= "|";
const char INPUT_REDIR[] 		= "<";
const char OUTPUT_REDIR[] 		= ">";

/* gets the line from file (just stdin for now) */
char *getLine(char *buff, FILE* input) {
	int length;
	
	length = sizeof(buff);
	fgets(buff, length, input);
	return buff;
}

/* parses the delimiter from the given string */
char **parseLine(char *str, const char *delim, char **final) {
	char **result 	= 0;
	int str_len 	= strlen(str);
	int count 		= 0;
	int i 			= 0;
	
	if (strcmp(str, delim) == 0) {
		count++;
	}
	result = malloc(sizeof(char*) * count);
	
	result[i] = strtok(str, delim);
	while (result[i] != NULL) {
		i++;
		result[i] = strtok(NULL, delim);
	}
	return result;
}


int main() {
	char command[1024];
	char **piped;
	
	// get command from user
	char *input = getLine(command, stdin);
	
	// remove the trailing newline
	input[strcspn(input, "\n")] = 0;
	
	// parse the piped portions of the string
	piped = parseLine(input, PIPE);
	int temp = sizeof(piped);
	printf("%d\n", temp);
	printf("%s\n", piped[0]);
	printf("%s\n", piped[1]);

	return 0;	
}



