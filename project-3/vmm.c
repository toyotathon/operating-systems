#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* header files for parsing, other things coming */
//#include "parsing.h"

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

tlbBlock tlbBlockInit() {
	tlbBlock tlb;
	tlb.page = -1;
	tlb.frame = -1;
	return tlb;
}

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
tlbBlock tlb[TLB_ENTRIES]; 
bool freeTLB[TLB_ENTRIES];
int tlbEvictIndex = 0;


/* searches free frame list for an open index, returns -1 if there is not a free frame */
int currentFreeFrame() {
	int i;
	for (i=0; i<FRAMES; i+=1) {
		if (freeFrames[i]) {
			freeFrames[i] = false;
			return i;
		}
	}
	return -1;
}

/* searches free TLB entry list for an open index, returns -1 if there is not a free frame*/
int currentFreeTLB() {
	int i; 
	for (i=0; i<TLB_ENTRIES; i+=1) {
		if (freeTLB[i])	{
			freeTLB[i] = false;
			return i;
		}
	}
	return -1;
}

int evictTLB() {
	int index; 
	
	index = tlbEvictIndex;
	if (index == TLB_ENTRIES) {
		tlbEvictIndex = 0;
		index = 0;
	}
	tlbEvictIndex += 1;
	return index;
}

/* Shift counter bits to move clock */
void shiftClock() {
	int i;
	for (i=0; i<PAGE_TABLE; i+=1) {
		pageTable[i].counter >>= 1;	
	}
}

int main(int argc, char *argv[]) {
	FILE *addresses;
	FILE *bs;

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

	addresses = fopen("addresses.txt", "r");
	bs = fopen("BACKING_STORE.bin", "rb");
	
	int i;
	/* initializing page table contents */
	for (i=0; i<PAGE_TABLE; i+=1) {
		pageTable[i] = pageBlockInit();
	}
	
	/* initializing TLB contents */	
	for (i=0; i<TLB_ENTRIES; i+=1) {
		tlb[i] = tlbBlockInit();
	}

	/* initalizing free TLB entries list */
	for (i=0; i<TLB_ENTRIES; i+=1) {
		freeTLB[i] = true;
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

		/* TLB Stage */
		int t = 0;
		while (tlb[t].page != pagenum && t < TLB_ENTRIES) {
			t += 1;
		}

		if (t == TLB_ENTRIES) {
			printf("Frame number for page %d is missing in the TLB.\n", pagenum);

			int freeTLB = currentFreeTLB();
			if (freeTLB == -1) {
				freeTLB = evictTLB();
			} 
			
			/* Page Table Stage */
			/* If page is not in memory: */
			if (!pageTable[pagenum].inmem) {
				fault += 1;

				printf("Virtual address %d contained in page %d causes a page fault.\n",address,pagenum);
				/* check free frame list */
				int free = currentFreeFrame();
				if (free == -1) { 
					/* use LRU algorithm to find a page to replace */
					int framereplace;
					unsigned min;
					min = pageTable[0].counter;
					int replace = 0;
					for (i=1; i<(PAGE_TABLE-1); i+=1) {
						if ((pageTable[i].counter < min) && pageTable[i].inmem){
							min = pageTable[i].counter;
							replace = i;
						} 
					}
					/* clear values from page being replaced, get new frame */
					framereplace = pageTable[replace].page;
					pageTable[replace] = pageBlockInit();
	
					pageTable[pagenum] = newPageBlock(framereplace);			
					pageTable[pagenum].counter |= COUNTER;
	
					printf("Page %d is loaded into frame %d.\n", pagenum, framereplace);
		
					tlb[freeTLB].page = pagenum;
					tlb[freeTLB].frame = framereplace;
				
					printf("Frame %d containing page %d is stored in entry %d of the TLB\n",
							tlb[freeTLB].frame, tlb[freeTLB].page, freeTLB);


					int j;
					int data;
					for (j=0; j< 265; j+=1) {
						fseek(bs, pagenum*256+j*4, SEEK_SET);
						fread(&data, sizeof(data), 1, bs);
						physicalMemory[framereplace][j] = data;
					}
				} 
				else {
					/* set the page value in page table */
					pageTable[pagenum].page = free;
					pageTable[pagenum].inmem = true;
					pageTable[pagenum].counter |= COUNTER;

					printf("Page %d is loaded into frame %d.\n", pagenum, free);
	
					tlb[freeTLB].page = pagenum;
					tlb[freeTLB].frame = free;

					printf("Frame %d containing page %d is stored in entry %d of the TLB\n",
							tlb[freeTLB].frame, tlb[freeTLB].page, freeTLB);

					/* set physical memory */
					int j;
					int data;
					for (j=0; j< 265; j+=1) {
						fseek(bs, pagenum*256+j*4, SEEK_SET);
						fread(&data, sizeof(data), 1, bs);
						physicalMemory[free][j] = data;
					}
				}	
			} 
		
			/* When page is in memory: */
			else {
				hits += 1;
				pageTable[pagenum].counter |= COUNTER;
				
				printf("Page %d is contained in frame %d.\n", pagenum, pageTable[pagenum].page);
			
				tlb[freeTLB].page = pagenum;
				tlb[freeTLB].frame = pageTable[pagenum].page;
				
				printf("Frame %d containing page %d is stored in entry %d of the TLB\n",
						tlb[freeTLB].frame, tlb[freeTLB].page, freeTLB);
			}
		} 
		
		/* TLB Hit */
		else {	
			printf("Page %d is stored in frame %d in entry %d of TLB.\n", 
					tlb[t].page, tlb[t].frame, t);	
		}
		shiftClock();	
	}	
	fclose(addresses);
}
