#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

/* array that includes all legal characters in the shell */
char LEGAL_CHARS[70] = {
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
	'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
	'0','1','2','3','4','5','6','7','8','9',
	'-','.','_','/', '|','<','>', ' '
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

/* initialize a new command struct */
command *newCommand(int size, int number) {
	size_t newsize = (size_t) size;
	command *newcommand = calloc(1, sizeof(command));

	newcommand->length 	= size;
	newcommand->number 	= number;
	newcommand->inputs 	= 0;
	newcommand->outputs = 0;
	newcommand->command = malloc(newsize * sizeof(*(newcommand->command)));

	return newcommand;	
}


/* TODO parses the given string by the spaces*/
void parseLine(char *buff, char **returnbuff) {}

/* check command input for errors, returns false if invalid*/
bool checkInputErrors(char *buff) {
	int length;
	
	length = strlen(buff);

	/* if exit is the only thing entered by user, exit the program */
	if (strcmp(buff, "exit") == 0) {	
		return false;
	}

	/* iterate through input, see if any chars are invalid */
	int i;
	int j;
	bool valid;
	for (i=0; i<length; i++) {
		valid = false;
		for (j=0; j < 70; j++) {
			if (buff[i] == LEGAL_CHARS[j]) {	
				valid = true;
				break;
			}
		}
		if (valid == false) {
			printf("invalid input 1\n");
			break;
		}
	}
	return valid;
}

/* check if input has any incorrect pipe stuff */
bool checkPipes(char *parsed[], int commandnum) {
	int i;
	char *currstring;
	int length;
	char lastchar;
	char firstchar;

	for (i=0; i<commandnum; i++) {
		currstring = parsed[i];
		if (i == 0) {	
			length = strlen(currstring);
			lastchar = currstring[length-1];
			if (lastchar != ' ') {
				return false;
			}
		} 
		else {
			firstchar = currstring[0];
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
bool checkCommandErrors(command *totalcommands[], int commandnum) {
	int i;
	int j;
	command *current;
	int length;
	bool legal;
	char* commandtoken;

	for (i=0; i<commandnum; i++) {			
		current = totalcommands[i];
		length = current->length;
		
		for (j=0; j<length; j++) {
			commandtoken = current->command[j];
			
			/* counting the number of file redirects */
			if (strstr(commandtoken, INPUT_REDIR) != NULL) {
				current->inputs += 1;
			}

			if (strstr(commandtoken, OUTPUT_REDIR) != NULL) {
				current->outputs += 1;
			}
			
			/* checking for errors in the user input */
			if (strstr(commandtoken, "<<") != NULL) {
				//syntax error
				printf("invalid input 3\n");
				legal = false;
			}

			if (strstr(commandtoken, ">>") != NULL) {
				printf("invalid input 3\n");	
				legal = false;
			}		
		}

		if (current->inputs > 1) {
			printf("invalid input 3\n");
			legal = false;
		}
		
		else if (current->outputs > 1) {
			printf("invalid input 3\n");
			legal = false;
		} else {
			legal = true;
		}
	}
	return legal;
}

/* takes in the parsed commands, separates them into structs */
void getCommands(command *totalcommands[], char *parsed[], int commandnum) {
	int i;
	int j;
	char *currstring;
	char *iter1;
	char *saveptr1;
	char *commands[1024];
	int commandindex;

	for (i=0; i<commandnum; i++) {
		commandindex = 0;
		
		currstring = parsed[i];
		saveptr1 = currstring;
		
		while ( (iter1 = strtok_r(saveptr1, SPACE, &saveptr1)) ) {
			commands[commandindex] = iter1;
			commandindex++;	
		}
		
		command *newcommand = newCommand(commandindex, i);

		for (j=0; j<commandindex; j++) {
			newcommand->command[j] = strdup(commands[j]);
		}	
		totalcommands[i] = newcommand;
	}

}

int main() {
	char input[1024];
	bool errors;
	char *parsed[1024];
	int commandnum;
	char *saveptr;
	char *iter;
	bool pipe;
	command *totalcommands[1024];
	bool commanderrors;

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
	
	/* check to see if pipe is spaced correctly */
	if (pipe) {
	 	errors = checkPipes(parsed, commandnum);
	}	
	if (!errors) {
		printf("invalid input 2\n");
		exit(0);
	}
	
	/* parse the given command, separate into separate structs */
	getCommands(totalcommands, parsed, commandnum);

	/* check commands for legality */
	commanderrors = checkCommandErrors(totalcommands, commandnum);
	
	if (!commanderrors) {
		exit(0);	
	}

	return 0;
}
