#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* header files for parsing, other things coming */
#include "parsing.h"

const int PAGE_SIZE = 256;
const int VM_SIZE = 256;
const int PM_SIZE = 128;

/* masks to get lower 16 bits, page number, and offset */
const uint16_t LOW_16_BITS = 0xFFFF;
const uint16_t LOW_8_BITS = 0xFF;
//const uint16_t HIGH_8_BITS = 0xFF00;

int main(int argc, char *argv[]) {
	bool checks;
	FILE *addresses;
	FILE *bs;
	int framenum;

	int physical_memory[PM_SIZE];
	/*
	virtual_memory initialization
	[0] -> physical address
	[1] -> age 
	*/
	int virtual_memory[VM_SIZE];
	/* TODO implement TLB */

	uint32_t address;
	uint16_t lower;
	uint8_t pagenum;
	uint8_t offset;

	/* variables for iterating through txt file */
	ssize_t read;
	char *saveptr;
	size_t len = 0;
	char *value = NULL;

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

	/* initialize tables, etc */
	int i;
	for (i=0; i<PM_SIZE; i++) {
		physical_memory[i] = -1;
	}
	
	for (i=0; i<VM_SIZE; i++) {
		framenum = i*PAGE_SIZE;
		virtual_memory[i] = framenum;
	}

	/* read in addresses, apply mask */
	while ((read = getline(&value, &len, addresses)) != -1) {
		address = strtol(value, &saveptr, 10);
		lower = address & LOW_16_BITS;
		pagenum =  lower >> 8;
		
		//TODO page hit implement
		
		//TODO page fault implement

		//TODO tlb implement
				
	}	
	
	fclose(addresses);
}

