#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
/* TODO might not use this?? */
char **parseLine(char *buff, const char *delim) {}

/* check command input for errors, or exit command */
bool checkErrors(char *buff) {
	int length = strlen(buff);
	if (strcmp(buff, "exit") == 0) {	
		return false;
	}
	/* iterate through input, see if any chars are invalid */
	int i;
	int j;
	bool valid = false;
	for (i=0; i<length; i++) {
		for (j=0; j < 71; j++) {
			if (buff[i] == LEGAL_CHARS[j]) {	
				valid = true;
			}
		}
		if (!valid) {
			printf("invalid input\n");
			return false;
		}
	}	
	return true;
}

int main() {
	char command[1024];
	char *parsed[1024];
	
	// get command from user
	char *input = getLine(command, stdin);
	
	// remove the trailing newline
	input[strcspn(input, "\n")] = 0;
	
	// check the input for any errors
	bool errors = checkErrors(input);
		
	// parse the piped portions of the string	
	return 0;	
}



