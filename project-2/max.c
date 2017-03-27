#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

/* TODO create header files for thread and parsing stuff*/
#include "parsing.h"

#define BUFFSIZE 4096

/* Thread Struct */
typedef struct {
	pthread_t id;
	int arrayIndex;
	int finalIndex;
	int returnVal;
} threadStruct;

/* Argument Struct */
typedef struct {
	sem_t mutex;
	sem_t b1;
	sem_t b2;
	int rounds;
	int finalVal; 
	int *number;
	int *final; 
	threadStruct *thread;
} argStruct;

/* function that will create a new argStruct and initialize all initial values */
argStruct *newArgStruct(sem_t m, sem_t b1, sem_t b2, int r, int n[], int f[], threadStruct *t) {
	argStruct *newargs = calloc(1, sizeof(argStruct));

	newargs->mutex = m;
	newargs->b1 = b1;
	newargs->b2 = b2;
	newargs->rounds = r;
	newargs->finalVal = 0;
	newargs->number = n;
	newargs->final= f;
	newargs->thread = t;
	
	return newargs;	
}


void *threadMax(void *args) {	
	return 0;	
}

int main(int argc, char **argv) {
	FILE *f;				// file pointer
	char buff[BUFFSIZE];	// buffer that will hold string input
	int	numbers[BUFFSIZE];	// int array that will hold converted strings
	int final[BUFFSIZE];	// int array that will contain the return value	
	bool checks;			// boolean used to check the input given by user
	int length;				// # of numbers in the file
	int numthreads;			// # of threads to be created
	int rounds;				// # of rounds needed
	sem_t mutex;
	sem_t b1;
	sem_t b2;


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

	/* initialize structs and data needed to create threads */
	threadStruct threads[numthreads];
	sem_init(&mutex, 0, 1);
	sem_init(&b1, 0, 0);
	sem_init(&b2, 0, 1);
	
	// initial values 
	int assigned = 0;
	int finarray = 0;
	int threadcond = -1;
	for (i=0; i<numthreads; i+=1) {
		threads[i].arrayIndex = assigned;
		threads[i].finalIndex = finarray;
		
		finarray += 1;
		assigned += 2;

		argStruct *args = newArgStruct(mutex, b1, b2, rounds, numbers, final, &threads[i]);
		
		threadcond = pthread_create(&(threads[i].id), NULL, &threadMax, args);

		if (threadcond != 0) {
			printf("ERROR: unsuccessfully created thread.");
			exit(0);
		}
	}

	for (i=0; i<numthreads; i+=1) {
		pthread_join(threads[i].id, NULL);
	}
	
	return 0;
}

