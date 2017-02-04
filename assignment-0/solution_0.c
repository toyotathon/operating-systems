#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main (int argc, const char* argv[]) 
{
	char buf[1024];
	int compare;
	while (fgets (buf, sizeof(buf), stdin) ) {	
		compare = strcmp(buf, "exit\n");
		if (compare == 0) {
			exit(0);
		} else {
			printf("%zu\n", strlen(buf)-1);
		}
	}
	return 0;
}




