#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <math.h> 

#define BUFFSIZE 1024

/* check to see if the file is not empty, return true if not null */
bool checkNullFile(FILE *f) {
	long size;
	if (NULL != f) {
		fseek(f, 0, SEEK_END);
		size = ftell(f);
		if (0 == size) {
			printf("ERROR: file is empty\n");
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
			printf("ERROR: contains invalid character\n");
			return false;
		}
	}
	return true;
}

int main(int argc, char **argv) {
	FILE *f;
	char buff[BUFFSIZE];
	bool checks;
	int length;
	int numthreads;
	int rounds;

	char *saveptr;
	char *iter;
	char *parsed[BUFFSIZE];

	/* read from the file containing the integers */	
	f = fopen(argv[1], "r");

	/* file checks */
	checks = checkNullFile(f);	
	if (checks) {
		rewind(f);
		fgets(buff, BUFFSIZE-1, f);
		fclose(f);
	} else {
		exit(0);
	}

	/* buffer checks */
	checks = checkInvalidCharacters(buff);
	if (checks) {
		length = 0;	
		saveptr = buff;
		while ((iter = strtok_r(saveptr, " ", &saveptr))) {
			parsed[length] = iter;
			length++;
		}	
	} else {
		exit(0);
	}
	
	numthreads = length / 2;
	rounds = log2(length);
	
	/* close file before return */
	return 0;
}

