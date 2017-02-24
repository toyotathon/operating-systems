#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

/* array that includes all legal characters in the shell */
char LEGAL_CHARS[74] = {
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
	'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
	'0','1','2','3','4','5','6','7','8','9','0',
	'-','.','_', 
	'|','<','>'
};

/* legal tokens for the shell */
const char SPACE[]				= " ";
const char PIPE[] 				= "|";
const char INPUT_REDIR[] 		= "<";
const char OUTPUT_REDIR[] 		= ">";

/*TODO change this as needed
* - int: length of command 
	- get this when parsing the command
* - int: number of command (i.e. order it is executed in)
	- get this from commandnum
	- if the output redir is used before the last command, throw error
* - int: number of input redirections
	- more than one input redir, throw error
* - int: number of output redirections
	- more than one output redir, throw error
* - char *: command (space parsed)
*/
typedef struct commandstruct {
	int length;
	int number;
	int inputs;
	int outputs;
	char **command;
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
			// validates on the first character
			// might be problematic later in development, keep an eye on this?
			if (buff[i] == LEGAL_CHARS[j]) {	
				valid = true;
			}
		}
		if (!valid) {
			printf("invalid input 1\n");
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
//bool checkCommandErrors(char *buff) {}

int main() {
	char input[1024];
	bool errors;
	char *parsed[1024];
	int commandnum;
	char *saveptr;
	char *iter;
	bool pipe;
		
	// get command from user
	fgets(input, 1024, stdin);
	
	// remove the trailing newline
	input[strcspn(input, "\n")] = 0;
	
	// check the input for any errors
	errors = checkInputErrors(input);
	if (!errors) {
		exit(0);
	}

	/* check if there are pipes in the input */
	if (strchr(input, '|') != NULL) {
		pipe = true;
	} 
	else {
		pipe = false;
	}
	
	
	/* parse the piped portions of the string */
	// TODO move this to parseLine function??
	commandnum = 0;
	saveptr = input;
	
	while ( (iter = strtok_r(saveptr, PIPE, &saveptr)) ) {
		parsed[commandnum] = iter;
		commandnum++;
	}
	/* free the pointers used to parse the string */
	//free(iter);
	//free(saveptr);
	
	/* check to see if pipe is spaced correctly */
	if (pipe) {
	 	errors = checkPipes(parsed, commandnum);
	}	
	if (!errors) {
		printf("invalid input 2\n");
		exit(0);
	}	

	/* CURRENT IDEA FOR PARSING PARSED:
	* - get the command string
	* - get the length of the string
	* - parse the string by space
	* - create a new command struct
	* - add this to an array 
	*/
	
	/* CURRENT COMMAND STRUCT:
	* - int: length of command 
		- get this when parsing the command
	* - int: number of command (i.e. order it is executed in)
		- get this from commandnum
		- if the output redir is used before the last command, throw error
	* - int: number of input redirections
		- more than one input redir, throw error
	* - int: number of output redirections
		- more than one output redir, throw error
	* - char *: command 
		- space parsed
	*/

	command totalcommands[1024];	
	int i;
	for (i=0; i<commandnum; i++) {
		char *currstring;
		char *iter1;
		char *saveptr1;
		char *commands[1024];
		int commandindex = 0;
		
		currstring = parsed[i];
		saveptr1 = currstring;
		
		while ( (iter1 = strtok_r(saveptr1, SPACE, &saveptr1)) ) {
			commands[commandindex] = iter1;
			commandindex++;	
		}
		
		int j;
		for (j=0; j<commandindex; j++) {
			printf("[%d] => '%s'\n", j, commands[j]);
		}
		printf("\n");
		
		totalcommands[i].length = commandindex;
		totalcommands[i].number = i;
		totalcommands[i].inputs = 0;
		totalcommands[i].outputs = 0;
		totalcommands[i].command = commands;
	}
	
	command curr = totalcommands[0];
	int length = curr.length;
	printf("%d\n", length);
	int j;
	for (j=0; j<length; j++) {
		printf("%s\n", curr.command[j]);
	}
	
	return 0;

	/* TODO check commands for legality */
}
