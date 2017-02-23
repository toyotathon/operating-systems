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

const char SPACE[]				= " ";
const char PIPE[] 				= "|";
const char INPUT_REDIR[] 		= "<";
const char OUTPUT_REDIR[] 		= ">";

/*TODO create a struct that holds all the necessary things??
* - char **: commands
* - int: number of commands
* - int: number of pipes
* - int: number of redirections
*/
typedef struct command{
	int numcommands;
	int pipes;
	int redirections;
	char *command[];
} command;

/* parses the given string by the spaces*/
void parseLine(char *buff, char **returnbuff) {}

/* check command input for errors, or exit command */
bool checkInputErrors(char *buff) {
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

/* check if input has any incorrect pipe stuff */
bool checkPipes(char *parsed[], int commandnum) {
	int i;
	for (i=0; i<commandnum; i++) {
		char *currstring = parsed[i];
		if (i == 0) {	
			int length = strlen(currstring);
			char lastchar = currstring[length-1];
			if (lastchar != ' ') {
				return false;
			}
		} 
		else {
			char firstchar = currstring[0];
			if (firstchar != ' ') {
				return false;
			}	
		}
	}
	return true;
} 

/* check errors in the command (after the input is validated and parsed) 
* things to check for:
* 	- redirection tokens used in wrong place
* 	- only one of each redirect token per command
* 	- (add more)
*/
/* TODO */
bool checkCommandErrors(char *buff) {}

int main() {
	char input[1024];
	bool errors;
	char *parsed[1024];
	int commandnum;
	char *saveptr;
	char *iter;
		
	// get command from user
	fgets(input, 1024, stdin);
	
	// remove the trailing newline
	input[strcspn(input, "\n")] = 0;
	
	// check the input for any errors
	errors = checkInputErrors(input);
	if (!errors) {
		exit(0);
	}
	
	/* parse the piped portions of the string */
	// TODO move this to parseLine function??
	commandnum = 0;
	saveptr = input;
	
	while (iter = strtok_r(saveptr, PIPE, &saveptr)) {
		parsed[commandnum] = iter;
		commandnum++;
	}
	
	/* check to see if pipe is spaced correctly */
	errors = checkPipes(parsed, commandnum);	
	if (!errors) {
		printf("invalid input\n");
		exit(0);
	}
	
	/* TODO check commands for legality */
	
	return 0;
}



/*
if (i == 0 && (strcmp(strArray[i], ">") == 0 || strcmp(strArray[i], "<") == 0 || strcmp(strArray[i], "|") == 0)) {
					execute = false;
					printf("invalid input\n");
					
				}


*/

