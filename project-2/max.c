#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h> 

#define BUFFSIZE 1024

/* check if file actually exists, returns true if it does */
bool checkExistingFile(const char * filename) { 
	int exists;

	exists = access(filename, F_OK); 
	if (exists != 0) {
		printf("ERROR: %s does not exist.\n", filename);
		return false;
	}
	return true;
}

/* check to see if the file is not empty, return true if not null */
bool checkNullFile(FILE *f) {
	long size;

	if (NULL != f) {
		fseek(f, 0, SEEK_END);
		size = ftell(f);
		if (0 == size) {
			printf("ERROR: file is empty.\n");
			return false;
		}
	}
	return true;
}

/* check for alphabetic characters, return true if none found */
bool checkInvalidCharacters(char* buff) {
	int i;
	int length;

	length = strlen(buff);
	for (i = 0; i<length; i+=1) {
		char current = buff[i];
		if (!isdigit(current) && !isspace(current)) {
			printf("ERROR: contains invalid character.\n");
			return false;
		}
	}
	return true;
}

int main(int argc, char **argv) {
	FILE *f;				// file pointer
	char buff[BUFFSIZE];	// buffer that will hold string input
	int	numbers[BUFFSIZE];	// int array that will hold converted strings
	bool checks;			// boolean used to check the input given by user
	int length;				// # of numbers in the file
	int numthreads;			// # of threads to be created
	int rounds;				// # of rounds needed 

	/* variables used to iterate and parse through input */
	int i;	
	char *saveptr;		
	char *iter;
	char *parsed[BUFFSIZE];
	
	/* check for correct number of arguments */	
	if (argc != 2) {
		printf("ERROR: incorrect number of arguments given.\n");
		exit(0);
	}
	
	/* check if file exists */
	checks = checkExistingFile(argv[1]);
	if (checks) {
		/* read from the file containing the integers */
		f = fopen(argv[1], "r");
	} else {
		exit(0);
	}	

	/* file checks */
	checks = checkNullFile(f);	
	if (checks) {
		/* go back to beginning of file, grab first line */
		rewind(f);
		fgets(buff, BUFFSIZE-1, f);
		/* close file before return */
		fclose(f);
	} else {
		exit(0);
	}

	/* buffer checks */
	checks = checkInvalidCharacters(buff);
	if (checks) {
		/* parse the buffer by spaces */
		length = 0;	
		saveptr = buff;
		while ((iter = strtok_r(saveptr, " ", &saveptr))) {
			parsed[length] = iter;
			length++;
		}	
	} else {
		exit(0);
	}

	/* convert strings in array to int */
	for (i=0; i<length; i+=1) { 
		numbers[i] = strtol(parsed[i], &saveptr, 10);	
	}
	
	numthreads = length / 2;
	rounds = log2(length);
		
	return 0;
}

