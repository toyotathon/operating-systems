#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

/* header files for thread and parsing functions */
#include "parsing.h"
#include "threading.h"

#define BUFFSIZE 4096

int main(int argc, char **argv) {
	FILE *f;				// file pointer
	char buff[BUFFSIZE];	// buffer that will hold string input
	int	numbers[BUFFSIZE];	// int array that will hold converted strings
	bool checks;			// boolean used to check the input given by user
	int length;				// # of numbers in the file
	int max;

	/* initial values for thread creation */
	int assigned = 0;
	int finarray = 0;
	argStruct *args;
	int threadcond = -1;

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
	rounds = numthreads/2;

	/* initialize structs and data needed to create threads */
	threadStruct threads[numthreads];
	sem_init(&mutex, 0, 1);
	sem_init(&b1, 0, 0);
	sem_init(&b2, 0, 1);
		
	for (i=0; i<numthreads; i+=1) {
		threads[i].arrayIndex = assigned;
		threads[i].finalIndex = finarray;
		
		finarray += 1;
		assigned += 2;

		args = newArgStruct(mutex, b1, b2, numthreads, rounds, numbers, final, &threads[i]);
		
		threadcond = pthread_create(&(threads[i].id), NULL, &threadMax, args);

		if (threadcond != 0) {
			printf("ERROR: unsuccessfully created thread.");
			exit(0);
		}	
	}

	max = finalMax(numthreads, threads, args);

	printf("%d\n", max);
	
	return 0;
}

