#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>


/* array that includes all legal characters in the shell */
char LEGAL_CHARS[70] = {
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
	'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
	'0','1','2','3','4','5','6','7','8','9',
	'-','.','_','/', '|','<','>', ' '
};

char SYMBOLS[8] = {'-','.','_','/', '|','<','>', ' '};

/* legal tokens for the shell */
const char SPACE[]				= " ";
const char PIPE[] 				= "|";
const char INPUT_REDIR[] 		= "<";
const char OUTPUT_REDIR[] 		= ">";

/* TODO change this as needed
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
	int in_index;
	int out_index;
	char **command;
} command;

/* initialize a new command struct */
command *newCommand(int size, int number) {
	size_t newsize; 

	newsize = (size_t) size;
	command *newcommand = calloc(1, sizeof(command));

	newcommand->length 		= size;
	newcommand->number 		= number;
	newcommand->inputs 		= 0;
	newcommand->outputs 	= 0;
	newcommand->in_index 	= 0; 
	newcommand->out_index 	= 0;
	newcommand->command 	= malloc(newsize * sizeof(*(newcommand->command)));

	return newcommand;	
}

/* get indices of redirection tokens */
void getRedirIndices(command *totalcommands[], int commandnum) {
	int i;
	int j;
	int inputindex;
	int outputindex;
	int length;
	command *current;
	char* commandtoken;

	for (i=0; i<commandnum; i++) {			
		current = totalcommands[i];
		length = current->length;

		inputindex = 0;
		outputindex = 0;

		for (j=0; j<length; j++) {
			commandtoken = current->command[j];
	
			if (strcmp(commandtoken, INPUT_REDIR) != 0) {
				inputindex++;
			} 
			else {
				current->in_index = inputindex;
				break;
			}
		}	
	
		// get index of output redir
		for (j=0; j<length; j++) {
			commandtoken = current->command[j];	
			if (strcmp(commandtoken, OUTPUT_REDIR) != 0) {
				outputindex++;
			} 
			else {
				current->out_index = outputindex;
				break;
			}
		}
	}
}

/* check command input for errors, returns false if invalid*/
bool checkInputErrors(char *buff) {
	int length;
	int i;
	int j;
	bool valid;

	length = strlen(buff);

	/* if exit is the only thing entered by user, exit the program */
	if (strcmp(buff, "exit") == 0) {	
		return false;
	}

	/* make sure 1st input char is not a symbol/whitespace */
	for (i=0; i<8; i++) {
		if (buff[0] == SYMBOLS[i]) {
			printf("invalid input\n");
			return false;
		}
	}
	
	/* iterate through input, see if any chars are invalid */
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
		
		// check if the command is not just whitespace
		if (strcmp(currstring, " ") == 0) {
			return false;
		}

		// check spacing of first command
		if (i == 0) {	
			length = strlen(currstring);
			lastchar = currstring[length-1];
			if (lastchar != ' ') {
				return false;
			}
		} 
		
		// check spacing of next command
		else {
			firstchar = currstring[0];
			if (firstchar != ' ') {
				return false;
			}	
		}
	}
	return true;
} 

/* check errors in the command (after the input is validated and parsed) */
bool checkCommandErrors(command *totalcommands[], int commandnum) {
	int i;
	int j;
	command *current;
	int length;
	bool legal;
	char* commandtoken;

	/* get the redirect indices needed for this */
	getRedirIndices(totalcommands, commandnum);
	
	legal = true;

	for (i=0; i<commandnum; i++) {			
		current = totalcommands[i];
		length = current->length;
		
		for (j=0; j<length; j++) {
			commandtoken = current->command[j];
			
			/* counting the number of file redirects */
			if (strcmp(commandtoken, INPUT_REDIR) == 0) {
				current->inputs += 1;
			}

			if (strcmp(commandtoken, OUTPUT_REDIR) == 0) {
				current->outputs += 1;
			}
			
			/* checking for errors in the user input */
			if (strstr(commandtoken, "<<") != NULL) {
				//syntax error
				legal = false;
			}

			if (strstr(commandtoken, ">>") != NULL) {
				legal = false;
			}		
		}

		// check if redirs are placed in the correct order
		if ((current->inputs == 1) && (current->outputs ==1)) {			
			if ((current->in_index) > (current->out_index)) {
				legal = false;
			}	
		}

		if (current->inputs > 1) {
			legal = false;
		}
			
		if (current->outputs > 1) {
			legal = false;
		} 

		// output redir cannot be followed by a pipe operator
		if (current->outputs == 1) {
			int currnumber = commandnum - 1;
			if (current->number !=  currnumber) {
				legal = false;
			}
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
	bool pipepresent;
	command *totalcommands[1024];
	bool commanderrors;

	/* get command from user */
	fgets(input, 1024, stdin);
	
	/* remove the trailing newline */
	input[strcspn(input, "\n")] = 0;
	
	/* check the input for any errors */
	errors = checkInputErrors(input);
	if (!errors) {
		exit(0);
	}

	/* check if there are pipes in the input */
	if (strchr(input, '|') != NULL) {
		pipepresent = true;
	} 
	else {
		pipepresent = false;
	}
		

	commandnum = 0;
	saveptr = input;

	/* parse the piped portions of the string */
	while ( (iter = strtok_r(saveptr, PIPE, &saveptr)) ) {
		parsed[commandnum] = iter;
		commandnum++;
	}
	
	if (pipepresent) {
		/* check to see if pipe is spaced correctly */
	 	errors = checkPipes(parsed, commandnum);
	}
	
	/* if piping errors are found */
	if (!errors) {
		printf("invalid input 2\n");
		exit(0);
	}
	
	/* parse the given command, separate into separate structs */
	getCommands(totalcommands, parsed, commandnum);

	/* check commands for legality */
	commanderrors = checkCommandErrors(totalcommands, commandnum);
	
	if (!commanderrors) {
		printf("invalid input 3\n");
		exit(0);	
	}

	/* interpret the commands from the user */
	// # of forks = commandnum
	// # of pipes = commandnum - 1
	int pipenumber;
	command *current;
	int out_file;
	char *out_name;
	int in_file;
	char *in_name;
	bool read;
	bool write;
	bool rw;
	int r_index = -2;
	int w_index = -1;
	int status;
	int i;
	int j;

	pipenumber = commandnum - 1;

	read = false;
	write = false;
	rw = false;
	
	int fd[2*pipenumber];

	/* set up the pipes necessary */	
	for (i=0; i<pipenumber; i++) {
		pipe(fd + 2*i);
	}
	
	for (j=0; j<commandnum; j++) {
		/* get the current command to be executed */
		current = totalcommands[j];

		if (current->inputs == 1) {
			int in_index = current->in_index;
			in_name = current->command[in_index + 1];
			in_file = open(in_name, O_CREAT | O_RDONLY, S_IRWXU);
			current->command[in_index] = NULL;
			current->command[in_index + 1] = NULL;
		}
		
		if (current->outputs == 1) {
			int out_index = current->out_index;
			out_name = current->command[out_index + 1];
			out_file = open(out_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
			current->command[out_index] = NULL;
			current->command[out_index + 1] = NULL;

		}

		if (current->number == pipenumber) {
			read = true;
			r_index += 2;
		}

		if ((pipenumber > 0) && current->number == 0) {
			write = true;
			w_index += 2;
		}

		if (current->number > 1) {
			rw = true;
			r_index += 2;
			w_index += 2;
		}
		
		int pid = fork();

		if (pid == 0) { /* child process */

			if (current->inputs == 1) {
				dup2(in_file, 0);
			}

			if (current->outputs == 1) {
				dup2(out_file, 1);
			}


			if (pipenumber > 0) {
			
				if (read) {	
					if (dup2(fd[r_index], 0) == -1) {
						perror("ERROR: error occured reading");
						// need to terminate process
						_exit(1);
					} 
				}	

				else if (write) {
					if (dup2(fd[w_index], 1) == -1) {
						perror("ERROR: error occured when writing");
					}
				}
			
				else if (rw) {
					if (dup2(fd[r_index], 0) == -1) {
						perror("ERROR: error between reading");
					}
					if (dup2(fd[w_index], 1) == -1) {
						perror("ERROR: error between writing");
					}
				}
					
				int k;
				int closingpipes = 2*pipenumber;
				for (k=0; k<closingpipes; k++) {
					close(fd[k]);
				}
			}
			execvp(current->command[0], current->command);		
			printf("invalid input in child process\n");
			exit(0);
		}
	
	}
	// wait for remaining processes
	int l;
	for (l=0; l < (pipenumber+1); l++) {
		wait(&status);
		fprintf(stderr, "%d\n", WEXITSTATUS(status));
	}
	
	// close all pipes	
	for (l=0; l<2*pipenumber; l++) {
		close(fd[l]);
	}
	
	// close all used files
	close(out_file);
	close(in_file);

	return 0;
}
