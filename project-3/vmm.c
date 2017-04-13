#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* header files for parsing, other things coming */
#include "parsing.h"

#define PAGE_TABLE 8
#define PAGE_SIZE 16

/* masks to get lower 16 bits, page number, and offset */
const uint16_t LOW_16_BITS = 0xFFFF;
const uint16_t LOW_8_BITS = 0xFF;

typedef struct {
	int page;
	int frame;
} block;

block blockInit() {
	block newBlock = {0,0};
	return newBlock;
}

int main(int argc, char *argv[]) {
	bool checks;
	FILE *addresses;
	FILE *bs;
	
	/* data structures for vmm */
	int physicalMemory[PAGE_TABLE][256];
	block pageTable[PAGE_TABLE] = {blockInit()};

	/* variables for data display */
	int total = 0;
	int fault = 0;
	int hits = 0;

	/* variables for parsing the address */
	uint32_t address;
	uint16_t lower;
	uint8_t pagenum;
	uint8_t offset;

	/* variables for iterating through txt file */
	char *value = NULL;
	char *saveptr;
	size_t len = 0;
	ssize_t read;

	/* ***check arguments, make sure they have been entered properly*** */	
	/* check for correct number of arguments */
	if (argc != 2) {
		printf("ERROR: incorrect number of arguments given.\n");
		exit(0);
	}

	/* check if file exists */	
	checks = checkExistingFile(argv[1]);
	if (checks) {
		addresses = fopen(argv[1], "r");
		bs = fopen("BACKING_STORE.bin", "rb");
	} else {
		exit(0);
	}

	/* read in addresses, apply mask */
	while ((read = getline(&value, &len, addresses)) != -1) {
		address = strtol(value, &saveptr, 10);
		lower = address & LOW_16_BITS;
		pagenum =  lower >> 8;
		offset = lower & LOW_8_BITS;
				
	}	
	
	fclose(addresses);
}

