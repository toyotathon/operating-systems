
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* array that includes all legal characters in the shell */
char LEGAL_CHARS[71] = {
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
	'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
	'0','1','2','3','4','5','6','7','8','9','0',
	'-','.','_'
};

char PIPE[] = "|";
char INPUT_REDIR[] = "<";
char OUTPUT_REDIR[] = ">";

// gets the line from file (just stdin for now)
char *getLine(char *buff, FILE* input) {
	int length;
	
	length = sizeof(buff);
	fgets(buff, length, input);
	return buff;
}

// parses the delimiter from the given string
char **parseLine(char *buff, char *delim, char *returnbuff[]) {
	int i = 0;
	char *tempbuff[sizeof(buff)];
	
	tempbuff[i] = strtok(buff, delim);
	while (tempbuff[i] != NULL) {
		i++;
		tempbuff[i] = strtok(NULL, delim);
	}
	returnbuff = tempbuff;
	
	return returnbuff;
}


int main() {
	char command[1024];
	char *pipedcommand[1024];
	int index = 0;
	
	// get command from user
	char *input = getLine(command, stdin);
	
	// remove the trailing newline
	input[strcspn(input, "\n")] = 0;
	
	// parse the piped portions of the string	
	// TODO move this to a separate function??
	pipedcommand[index] = strtok(input, PIPE);
	while (pipedcommand[index] != NULL) {
		index++;
		pipedcommand[index] = strtok(NULL, PIPE);
	}
	printf("%s\n", pipedcommand[0]);
/*
	while (pipedcommand[test] != NULL) {
		printf("%s\n", pipedcommand[test]);
		test++;
	}	
*/
	//
	exit(0);
}



