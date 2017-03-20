#include <stdio.h>

#define BUFFSIZE 1024

int main(int argc, char **argv) {
	FILE *f;
	char buff[BUFFSIZE];

	/* read from the file containing the integers */	
	f = fopen(argv[1], "r");
	
	fgets(buff, BUFFSIZE-1, f);
	printf("%s\n", buff);
	return 0;
}

