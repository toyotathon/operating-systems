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

