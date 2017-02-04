#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main (int argc, const char* argv[]) 
{
	char buffer[1024];
	char *string[256];

	char *add_operator;
	char *sub_operator;
	char *mul_operator;
	char *div_operator;

	char delim[] = "+-*/";
	char add_delim[] = "+";
	char sub_delim[] = "-";
	char mul_delim[] = "*";
	char div_delim[] = "/";

	int i = 0;

	unsigned int num;
	unsigned int num1;
	unsigned int result;
	
	if (fgets(buffer, sizeof(buffer), stdin)) {

		// check what operator was used
		add_operator = strstr(buffer, add_delim);
		sub_operator = strstr(buffer, sub_delim);
		mul_operator = strstr(buffer, mul_delim);
		div_operator = strstr(buffer, div_delim);
		
		// parse the integer values 		
		string[i] = strtok(buffer, delim);
		while (string[i] != NULL) {
			i++;
			string[i] = strtok(NULL, delim);
		}
		
		if (add_operator) {
			num = atoi(string[0]);
			num1 = atoi(string[1]);
			result = num + num1;
			printf("%d\n", result);
			exit(0);
		}

		if (sub_operator) {
			num = atoi(string[0]);
			num1 = atoi(string[1]);
			result = num - num1;
			printf("%d\n", result);
			exit(0);
		}

		if (mul_operator) {
			num = atoi(string[0]);
			num1 = atoi(string[1]);
			result = num * num1;
			printf("%d\n", result);
			exit(0);
		}

		if (div_operator) {
			num = atoi(string[0]);
			num1 = atoi(string[1]);
			result = num / num1;
			printf("%d\n", result);
			exit(0);
		}
	}
	return 0;
}

