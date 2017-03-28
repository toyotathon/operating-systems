#define BUFFSIZE 4096

int final[BUFFSIZE];	// int array that will contain the return value
int numthreads;			// # of threads to be created
int rounds;				// # of rounds needed
sem_t mutex;
sem_t b1;
sem_t b2;

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
	int numthreads;
	int rounds;
	int finalVal; 
	int *number;
	int *final; 
	threadStruct *thread;
} argStruct;

/* function that will create a new argStruct and initialize all initial values */
argStruct *newArgStruct(sem_t m, sem_t b1, sem_t b2, int nt, int r, int n[], int f[], threadStruct *t) {
	argStruct *newargs = calloc(1, sizeof(argStruct));

	newargs->mutex = m;
	newargs->b1 = b1;
	newargs->b2 = b2;
	newargs->numthreads = nt;
	newargs->rounds = r;
	newargs->finalVal = 0;
	newargs->number = n;
	newargs->final= f;
	newargs->thread = t;
	
	return newargs;	
}


void *threadMax(void *a) {
	/* getting and setting arguments for routine */
	static argStruct *args;
	sem_t mutex;
	sem_t b1;
	sem_t b2;
	int rounds;
	threadStruct *thread;

	int arrayIndex;
	int finalIndex;

	int max;
	int mcount = 0;
	int scount = 0;

	args = a;
	mutex = args->mutex;
	b1 = args->b1;
	b2 = args->b2;
	rounds = args->rounds;
	thread = args->thread;

	arrayIndex = thread->arrayIndex;
	finalIndex = thread->finalIndex;	

	max = args->number[arrayIndex];

	if (args->number[arrayIndex+1] > max) {
		max = args->number[arrayIndex+1];
	}
	thread->returnVal = max;
	
	args->final[finalIndex] = thread->returnVal;

	sem_wait(&mutex);
	scount += 1;
	mcount = 1;

	if (scount == mcount) {
		sem_wait(&b2);
		sem_post(&b1);
	}
	sem_post(&mutex);
	sem_wait(&b1);
	sem_post(&b1);

	while (rounds != 0) {

		if (finalIndex % 2 == 0) {
			if (args->final[finalIndex] < args->final[finalIndex+1]) {
				if (finalIndex > 0) {
					if (args->final[finalIndex/2] < args->final[finalIndex+1]) {
							args->final[finalIndex/2] = args->final[finalIndex+1];
					}
				} else {
					args->final[finalIndex] = args->final[finalIndex+1];
				}	
			}
			if (args->final[finalIndex] > args->final[finalIndex+1]) {
				if (finalIndex > 0) {
					if (args->final[finalIndex/2] < args->final[finalIndex]) {
						args->final[finalIndex/2] = args->final[finalIndex];
					}
				}
			} 
		}
		
		if (mcount == 1) {
			sem_wait(&mutex);
			scount -= 1;
			rounds -= 1;
			mcount = 0;

			if (scount == 0) {
				sem_wait(&b1);
				sem_post(&b2);
			} 

			sem_post(&mutex);
			sem_wait(&b2);
			sem_post(&b2);
		} else {
			sem_wait(&mutex);
			scount += 1;
			rounds -= 1;
			mcount = 1;

			if (scount == mcount) {
				sem_wait(&b2);
				sem_post(&b1);
			}

			sem_post(&mutex);
			sem_wait(&b1);
			sem_post(&b1);
		}
	}	
	return 0;	
}
