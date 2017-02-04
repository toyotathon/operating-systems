#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main (int argc, const char* argv[]) 
{
	char buffer[1024];
	char buffer1[1024];
	char buffer2[1024];
	
	char delim[] = " ";
	
	int i = 0;
	

	// get the length of the string to be inserted
	if (fgets(buffer, sizeof(buffer), stdin)) {
		int length = atoi(buffer);
		char *inserted[length];
		// get the string to be parsed
		if (fgets(buffer1, sizeof(buffer1), stdin)) {
			//remove trailing newline from fgets buffer2
			buffer1[strcspn(buffer1, "\n")] = 0;
			inserted[i] = strtok(buffer1, delim);
			while (inserted[i] != NULL) {
				i++;
				inserted[i] = strtok(NULL, delim);
			}
			// get the index, grab the string
			if (fgets(buffer2, sizeof(buffer2), stdin)) {
				int index = atoi(buffer2);
				printf("%s\n",inserted[index]);
			}
		}
	}
	return 0;
}

