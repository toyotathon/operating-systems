#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* header files for parsing, other things coming */
#include "parsing.h"

#define PAGE_TABLE 16
#define FRAMES 8
#define TLB_ENTRIES 4

/* masks to get page number and offset */
const uint16_t PAGE_NUMBER_BITS = 0xF00;
const unsigned COUNTER = 0x8000;
//const uint16_t OFFSET_BITS = 0xFF;

/* 
2 structs to use:
- TLB block
	- page number
	- frame number
- Page block
	- page 
	- counter for LRU replacement
	- bool to indicate that it is in memory
*/

typedef struct {
	int page;
	int frame;
} tlbBlock;

tlbBlock newTLBBlock(int page, int frame) {
	tlbBlock tlb;
	tlb.page = page;
	tlb.frame = frame;
	return tlb;
}

typedef struct {
	int page;
	bool inmem;
	unsigned counter;	
} pageBlock;

pageBlock pageBlockInit() {
	pageBlock p;
	p.page = 0;
	p.inmem = false;
	p.counter = 0;
	return p;
}

pageBlock newPageBlock(int page) {
	pageBlock p;
	p.page = page;
	p.inmem = true;
	p.counter = 0;
	return p;
}

/* data structures for vmm */
int physicalMemory[FRAMES][256];
bool freeFrames[FRAMES]; 
pageBlock pageTable[PAGE_TABLE];
tlbBlock tlb[TLB_ENTRIES]; // TODO implement TLB

int currentFreeFrame() {
	int i;
	for (i=0; i<FRAMES; i+=1) {
		if (freeFrames[i] == true) {
			freeFrames[i] = false;
			return i;
		}
	}
	return -1;
}

int main(int argc, char *argv[]) {
	bool checks;
	FILE *addresses;
	FILE *bs;
	
	/* variables for pointing, iterating through data structure */

	/* variables for data display */
	int total = 0;
	int fault = 0;
	int hits = 0;

	/* variables for parsing the address */
	int address;	
	int pagenum;
	//int offset;

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
	
	int i;
	/* initializing page table contents */
	for (i=0; i<PAGE_TABLE ;i+=1) {
		pageTable[i] = pageBlockInit();
	}

	/* initializing free frame list */
	for (i=0; i<FRAMES; i+=1) {
		freeFrames[i] = true;
	}

	/* read in addresses, apply mask */
	while ((read = getline(&value, &len, addresses)) != -1) {
		total += 1;
		address = strtol(value, &saveptr, 10);
		pagenum = (address & PAGE_NUMBER_BITS) >> 8;
		
		printf("Address: %d\n", address);
		printf("Page number: %d\n", pagenum);
		
		// page is NOT in memory
		if (!pageTable[pagenum].inmem) {
			fault += 1;
			// TODO error message here
			printf("Virtual address %d contained in page %d causes a page fault.\n",address,pagenum);
			// check free frame list
			int free = currentFreeFrame();
			if (free == -1) { 
				// do something to free up physical mem
				int framereplace;
				unsigned min;
				min = pageTable[0].counter;
				int replace = 0;
				for (i=1; i<PAGE_TABLE; i+=1) {
					if (pageTable[i].counter < min){
						min = pageTable[i].counter;
						replace = i;
						printf("getting reached");
					} 
				}
				printf("Page %d is being replaced.\n", replace);
				pageTable[replace].inmem = false;
				framereplace = pageTable[replace].page;
				
				pageTable[pagenum].page = framereplace;
				pageTable[pagenum].inmem = true;
				pageTable[pagenum].counter |= COUNTER;	

				int j;
				int data;
				for (j=0; j< 265; j+=1) {
					fseek(bs, pagenum*256+j*4, SEEK_SET);
					fread(&data, sizeof(data), 1, bs);
					physicalMemory[framereplace][j] = data;
				}
			} 
			else {
				// set the page value in page table
				pageTable[pagenum].page = free;
				pageTable[pagenum].inmem = true;
				pageTable[pagenum].counter |= COUNTER;

				// set physical memory
				int j;
				int data;
				for (j=0; j< 265; j+=1) {
					fseek(bs, pagenum*256+j*4, SEEK_SET);
					fread(&data, sizeof(data), 1, bs);
					physicalMemory[free][j] = data;
				}
			}	
		} 
	
		// page is in memory
		else {
			hits += 1;
			pageTable[pagenum].counter |= COUNTER;
			// TODO message here 	
		}
		// shift bits to move clock
		for (i=0; i<PAGE_TABLE; i+=1) {
			pageTable[i].counter >>= 1;	
		}
		//for (i=0; i<PAGE_TABLE; i+=1) {
		//	printf("Page %d count: %d\n", i, pageTable[i].counter);
		//}
	}	
	fclose(addresses);
}
