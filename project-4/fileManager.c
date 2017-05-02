#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "disk.h"

#define DIR_LENGTH 8
#define FAT_LENGTH 16
#define DATA_LENGTH 32

/* Structs needed to implement filesystem */
typedef struct {
	char status[2];
	char bn[3];
	char fn[5];
	char len[4];
} directoryBlock;

typedef struct {
	char status1[2];
	char block1[5];
	char status2[2];
	char block2[5];
	char status3[2];
	char block3[5];
	char status4[2];
	char block4[5];
} fatBlock;

typedef struct {
	bool open;
	char *filename;
	int offset;
} oftBlock;

directoryBlock newDirectoryBlock(char s[1], char bn[2], char fn[4], char len[3]) {
	directoryBlock db;
	
	size_t ssize = sizeof(db.status);
	strncpy(db.status, s, ssize);
	db.status[ssize-1] = '\0';

	size_t bnsize = sizeof(db.bn);
	strncpy(db.bn, bn, bnsize);
	db.bn[bnsize-1] = '\0';

	size_t fnsize = sizeof(db.fn);
	strncpy(db.fn, fn, fnsize);
	db.fn[fnsize-1] = '\0';

	size_t lensize = sizeof(db.len);
	strncpy(db.len, len, lensize);
	db.len[lensize-1] = '\0';

	return db;
}

fatBlock newFATBlock(char s1[1],char b1[3],char s2[1],char b2[3],char s3[1],char b3[3],char s4[1],char b4[3]) {
	fatBlock f;

	size_t s1size = sizeof(f.status1);
	strncpy(f.status1, s1, s1size);
	f.status1[s1size-1] = '\0';
	
	size_t b1size = sizeof(f.block1);
	strncpy(f.block1, b1, b1size);
	f.block1[b1size-1] = '\0';

	size_t s2size = sizeof(f.status2);
	strncpy(f.status2, s2, s2size);
	f.status2[s2size-1] = '\0';
	
	size_t b2size = sizeof(f.block2);
	strncpy(f.block2, b2, b2size);
	f.block2[b2size-1] = '\0';

	size_t s3size = sizeof(f.status3);
	strncpy(f.status3, s3, s3size);
	f.status3[s3size-1] = '\0';
	
	size_t b3size = sizeof(f.block3);
	strncpy(f.block3, b3, b3size);
	f.block3[b3size-1] = '\0';

	size_t s4size = sizeof(f.status4);
	strncpy(f.status4, s4, s4size);
	f.status4[s4size-1] = '\0';
	
	size_t b4size = sizeof(f.block4);
	strncpy(f.block4, b4, b4size);
	f.block4[b4size-1] = '\0';

	return f;
}

oftBlock newOFTBlock() {
	oftBlock oft;
	
	oft.open = true;
	oft.filename = malloc(10);
	oft.offset = 0;
	
	return oft;
}

/* global filesystem arrays */
directoryBlock directory[DIR_LENGTH];
fatBlock fat[FAT_LENGTH];
oftBlock oft[4]; 
char disk[32][BLOCK_SIZE+1];

void fatBlockNumbers() {
	int i;
	int value = 0;	
	for(i=0; i<16; i++) {
		char temp[3];
		snprintf(temp, 3, "%d", value);
		strncpy(fat[i].block1, temp, 3);
		char temp1[3];
		snprintf(temp1, 3, "%d", value+1);
		strncpy(fat[i].block2, temp1, 3);
		char temp2[3];
		snprintf(temp2, 3, "%d", value+2);
		strncpy(fat[i].block3, temp2, 3);
		char temp3[3];
		snprintf(temp3, 3, "%d", value+3);
		strncpy(fat[i].block4, temp3, 3);
		value += 4;
	}
}

void printDisk() {
	int i;
	for (i=0; i<32; i++) {
		printf("contents of disk: %s\n", disk[i]);
	}

}

char CLEARDATA[] = {'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};

int make_fs(char *disk_name) {
	int new_disk = make_disk(disk_name);
	if (new_disk == -1)  {
		return -1;
	}
	int open = open_disk(disk_name);
	if (open == -1) {
		return -1;
	}
	
	/* initializing superblock */
	char super[BLOCK_SIZE] = {
			'1', ' ', '8', ' ', // directory block location
			'9', ' ', '1', '6', // FAT block location
			'3', '2', '6', '3'  // data block location
	};
	block_write(0, super);

	/* initializing directory */
	int i;
 	for (i=1; i<9; i++) {
		char directory[BLOCK_SIZE] = {'f', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};
		block_write(i, directory);
	}

	/* initializing FAT */
	for (i=9; i<17; i++) {
		char fat[BLOCK_SIZE] = 
			{'f', '\0', '\0', '\0',
			 'f', '\0', '\0', '\0', 
			 'f', '\0', '\0', '\0', 
			 'f', '\0', '\0', '\0'};	
		block_write(i, fat);
	}
	
	/* initializing data */
	for (i=32; i<64; i++) {
		char data[BLOCK_SIZE] =	
		{'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};
	
		block_write(i, data);
	}	

	close_disk();
	return 0;
}

int mount_fs(char *disk_name) {
	int open = open_disk(disk_name);
	if (open == -1) {
		return -1;
	}

	/* Read in directory data */
	int i;
	for (i=1; i<9; i++) {
		char temp[BLOCK_SIZE];

		/* read in directory block from memory */
		block_read(i, temp);

		/* read status of block */
		char s[] = {temp[0], 0};

		/* read block number of block */
		char bn[] = {temp[1], temp[2], 0};

		/* read file number of block */	
		char fn[] = {temp[3], temp[4], temp[5], temp[6], 0};

		/* read length of file */
		char len[] = {temp[7], temp[8], temp[9], 0};
		
		/* save data to directory struct */
		int save = i-1;	
		directory[save] = newDirectoryBlock(s, bn, fn, len);
	}

	/* read in FAT data */	
	int save = 0;
	for (i=9; i<17; i++) {
		char temp[BLOCK_SIZE];	
		block_read(i, temp);
	
		/* read in block 1 data */
		char s1[] = {temp[0], 0};
		char b1[] = {temp[1], temp[2], temp[3], 0};

		/* read in block 2 data */
		char s2[] = {temp[4], 0};
		char b2[] = {temp[5], temp[6], temp[7], 0};

		/* read in block 3 data */
		char s3[] = {temp[8], 0};
		char b3[] = {temp[9], temp[10], temp[11], 0};

		/* read in block 4 data */
		char s4[] = {temp[12], 0};
		char b4[] =	{temp[13], temp[14], temp[15], 0};

		/* save data to FAT struct */
		fat[save] = newFATBlock(s1,b1,s2,b2,s3,b3,s4,b4);

		save++;	
	}
	fatBlockNumbers();

	/* get data values from disk */
	save = 0;
	for (i=32; i<64; i++) {
		block_read(i, disk[save]);	
		save += 1;
	}
	

	/* initialize OFT values, set open flags */	
	for (i=0; i<4; i++) {
		oft[i] = newOFTBlock();
	}

	close_disk();
	return 0;
}

int dismount_fs(char *disk_name) {	
	open_disk(disk_name);

	/* load directory metadata back into disk */
	int i;
	for (i=0; i<DIR_LENGTH; i++) {
		directoryBlock dir = directory[i];
		char temp[BLOCK_SIZE];	
		
		temp[0] = dir.status[0];
		
		temp[1] = dir.bn[0];
		temp[2] = dir.bn[1];

		temp[3] = dir.fn[0];
		temp[4] = dir.fn[1];
		temp[5] = dir.fn[2];
		temp[6] = dir.fn[3];
		
		temp[7] = dir.len[0];
		temp[8] = dir.len[1];
		temp[9] = dir.len[2];

		temp[10] = '\0';
		temp[11] = '\0';
		temp[12] = '\0';
		temp[13] = '\0';
		temp[14] = '\0';
		temp[15] = '\0';
	
		int save = i+1;
		block_write(save, temp);
	}
	
	int save = 0;
	for (i=9; i<17; i++) {
		fatBlock f = fat[save];
		char temp[BLOCK_SIZE+1];
		
		temp[0] 	= f.status1[0];
		temp[1] 	= f.block1[0];
		temp[2] 	= f.block1[1];
		temp[3] 	= f.block1[2];

		temp[4] 	= f.status2[0];
		temp[5] 	= f.block2[0];
		temp[6] 	= f.block2[1];
		temp[7] 	= f.block2[2];

		temp[8] 	= f.status3[0];
		temp[9] 	= f.block3[0];
		temp[10] 	= f.block3[1];
		temp[11] 	= f.block3[2];
	
		temp[12] 	= f.status4[0];
		temp[13]  	= f.block4[0];
		temp[14]  	= f.block4[1];
		temp[15]  	= f.block4[2];

		temp[BLOCK_SIZE] = '\0';
		
		block_write(i, temp);
		save += 1;
	}
		
	save = 0;
	for (i=32; i<64; i++) {
		block_write(i, disk[save]);
		save += 1;
	}
	/* TODO ANY OTHER METADATA TO SAVE GETS WRITTEN HERE */

	close_disk();
	return 0;
}

int fs_create(char *name) {
	int i;
	int length = 0;	

	/* get file name length */
	for(i=0; name[i]!='\0'; i++) {
		name[i];
		length += 1;
	}
	
	if (length > 4) {
		return -1;
	}

	/* check that this file name isnt already in use */
	for (i=0; i<DIR_LENGTH; i++) {
		directoryBlock curr = directory[i];
		char *currfn = curr.fn;
		if (strcmp(currfn, name) == 0) {	
			return -1;
		}
	}

	/* both tests pass -> create a new file. check to see if it has been allocated first */
	char free[] = {'f', '\0'};
	char *temp = free;
	int count = 1;
	int dirindex;
	for (i=0; i<DIR_LENGTH; i++) {
		/* check to see if block has been allocated */
		if (strcmp(directory[i].status, temp) == 0) {
			directory[i].status[0] = 'a';
			directory[i].status[1] = '\0';
			dirindex = i;
			break;
		}
		count += 1;
	}
	
	/* if there isnt a free file */	
	if (count == DIR_LENGTH) {
		return -1;
	}

	for (i=0; i<length; i++) {
		directory[dirindex].fn[i] =	name[i];
	} 
			
	return 0;
}

int fs_open(char *name) {
	int i;
	bool notfound;
	bool nospace;
	int filedes;

	/* check if name exists in directory first */
	notfound = true;	
	for (i=0; i< DIR_LENGTH; i++) {
		directoryBlock curr = directory[i];
		if (strcmp(curr.fn, name) == 0) {
			notfound = false;
			break;	
		}
	}
	if (notfound) {
		return -1;
	}

	/* check if file is already open in OFT */
	for (i=0; i<4; i++) {
		oftBlock curr = oft[i];
		if (strcmp(curr.filename, name) == 0) {
			return -1;
		} 
	}

	/* check if there is room in the OFT */
	nospace = true;
	for (i=0; i<4; i++) {
		if (oft[i].open) {
			int size = sizeof(oft[i].filename);
			strncpy(oft[i].filename, name, size);
			filedes = i;
			oft[i].open = false;
			nospace = false;
			break;	
		}
	}
	if (nospace) {
		return -1;
	}

	return filedes;
}
	
int fs_close(int fildes) {
	char null[] = {'\0'};
	if (oft[fildes].open) {
		return -1;
	}	

	oft[fildes].open = true;
	int size = sizeof(oft[fildes].filename);
	strncpy(oft[fildes].filename, null, size);
	oft[fildes].offset = 0;
	
	return 0;
}

int fs_delete(char *name) {
	int i;
	int blocknumber;
	int length;

	/* check if the file is open first */
	for (i=0; i<4; i++) {
		if (strcmp(oft[i].filename, name) == 0) {
			return -1;
		}
	}

	/* find the directory to delete */
	int deleteindex;
	int count = 1;
	for (i=0; i<DIR_LENGTH; i++) {
		if (strcmp(directory[i].fn, name) == 0) {
			deleteindex = i;
			break;
		}
		count += 1;
	}

	/* file does not exist in directory */
	if (count == DIR_LENGTH) {
		return -1;
	}

	char *tempbn = directory[deleteindex].bn;
	char *templen = directory[deleteindex].len;
	blocknumber = (int) strtol(tempbn, (char **)NULL, 10);
	length = (int) strtol(templen, (char **)NULL, 10);
	
	int deleteblocks = (int) (length / 16) + 1;	
	int deletefats = (int) (deleteblocks / 4) + 2;
	
	for (i=0; i<deletefats; i++) {
		/* set FAT block to free */
		int j;
		fat[blocknumber].status1[0] = 'f';

		for (j=0; j<BLOCK_SIZE; j++) {
			disk[blocknumber][j] = '\0';
		}
		blocknumber += 1;
	}
	

	/* reset values in block */	
	directory[deleteindex].status[0] = 'f';
	
	directory[deleteindex].bn[0] = '\0'; 
	directory[deleteindex].bn[1] = '\0';
	
	directory[deleteindex].fn[0] = '\0';
	directory[deleteindex].fn[1] = '\0';
	directory[deleteindex].fn[2] = '\0';
	directory[deleteindex].fn[3] = '\0';

	directory[deleteindex].len[0] = '\0';
	directory[deleteindex].len[1] = '\0';
	directory[deleteindex].len[2] = '\0';
	
	return 0;
}

int fs_read(int fildes, void *buf, size_t nbyte) {
	int i;
	int blocknumber;
	int length;
	int offset;
	int blocksneeded;
	int fatsneeded;
	directoryBlock dir;
	
	/* check if file is open first */
	if (oft[fildes].open) {
		return -1;
	}

	/* get directory block information*/
	char *filename = oft[fildes].filename;
	
	for (i=0; i<DIR_LENGTH; i++) {
		if (strcmp(directory[i].fn, filename) == 0) {
			dir = directory[i];
		}
	}
	
	char *tempbn = dir.bn;
	char *templen = dir.len;
	blocknumber = (int) strtol(tempbn, (char **)NULL, 10);
	length = (int) strtol(templen, (char **)NULL, 10);
	blocksneeded = (int) (length / 16) + 1;
	fatsneeded = (int) (blocksneeded / 4) + 5;
	
	char *temphold = malloc(length);
	for (i=0; i<fatsneeded; i++) {
		char *tempiter = malloc(BLOCK_SIZE);
		strcpy(tempiter, disk[blocknumber]);
		strcat(temphold, tempiter);
		blocknumber += 1;
	}
	
	strcpy(buf, temphold);
	
	/* read beginning from offset */
	int returnlen = 0;
	offset = oft[fildes].offset;

	for (i=offset; i<nbyte; i++) {
		returnlen += 1;
	}
	
	return returnlen;
}

int fs_write(int fildes, void *buff, size_t nbyte) {
	int i, blocknumber, length, dirindex;
	directoryBlock dir;
	
	/* check if file is open first */
	if (oft[fildes].open) {
		return -1;
	}

	/* get directory block information*/
	char *filename = oft[fildes].filename;

	for (i=0; i<DIR_LENGTH; i++) {	
		if (strcmp(directory[i].fn, filename) == 0) {
			dir = directory[i];
			dirindex = i;
			break;
		}	
	}
	char null[] = {'\0', '\0', '\0'};
	char *tempbn = dir.bn;
	char *templen = dir.len;	
	int nolen = (strcmp(templen, null) == 0);
	
	char *newbn = malloc(3);
	/* if there hasnt been any data written to this file (e.g. was recently created)*/
	if (nolen) {
		// find an open FAT block to enter
		for (i=0; i<16; i++) {
			char free[] = {'f', '\0'};
			char alloc[] = {'a', '\0'};
			char *status1 = fat[i].status1;	
	
			if (strcmp(status1, free) == 0) {
				strcpy(fat[i].status1, alloc);
				strcpy(newbn, fat[i].block1);
				break;
			}
		}
		strcpy(directory[dirindex].bn, newbn);	
		blocknumber = strtol(newbn, (char **)NULL, 10);
		length = 0;

	} else {
		blocknumber = strtol(tempbn, (char **)NULL, 10);
		length = strtol(templen, (char **)NULL, 10);
	}

	/* now that we have first block, figure out how many more are needed */	
	int blocksneeded;
	int fatsneeded;
	int offset = 0;
	blocksneeded = (int) (nbyte / 16) + 1;
	fatsneeded = (int) (blocksneeded / 4) + 1;

	char *buffer = (char *)buff;
	
	for (i=0; i<fatsneeded; i++) {
		/* get data to write to disk */
		int j;
		char alloc[] = {'a', '\0'};

		/* block 1 write */
		if (blocksneeded != 0) {
			char *tempbuff = malloc(16);
			strcpy(fat[blocknumber].status1, alloc);
			for (j=0; j<BLOCK_SIZE; j++) {
				tempbuff[j] = buffer[offset];
				offset += 1;
				length += 1;
			}
			
			strcpy(disk[blocknumber], tempbuff);	
			blocksneeded -= 1;
			blocknumber += 1;
		} else break;
	
		/* block 2 write */
		if (blocksneeded != 0) {
			char *tempbuff = malloc(16);
			strcpy(fat[blocknumber].status2, alloc);
			for (j=0; j<BLOCK_SIZE; j++) {
				tempbuff[j] = buffer[offset];
				offset += 1;
				length += 1;	
			}
			
			strcpy(disk[blocknumber], tempbuff);
			blocksneeded -= 1;
			blocknumber += 1;
		} else break;

		/* block 3 write */
		if (blocksneeded != 0) {
			char *tempbuff = malloc(16);
			strcpy(fat[blocknumber].status3, alloc);
			for (j=0; j<BLOCK_SIZE; j++) {
				tempbuff[j] = buffer[offset];
				offset += 1;
				length += 1;
			}
			
			strcpy(disk[blocknumber], tempbuff);
	
			blocksneeded -= 1;
			blocknumber += 1;
		} else break; 

		/* block 4 write */
		if (blocksneeded != 0) {
			char *tempbuff = malloc(16);
			strcpy(fat[blocknumber].status4, alloc);
			for (j=0; j<BLOCK_SIZE; j++) {
				tempbuff[j] = buffer[offset];
				offset += 1;
				length += 1;
			}
			
			strcpy(disk[blocknumber], tempbuff);

			blocksneeded -= 1;
			blocknumber += 1;
		} else break;	
	}

	/* save new offset and length data to memory */
	oft[fildes].offset = offset;
	char *finlen = malloc(3);
	snprintf(finlen, 3, "%d", (int) nbyte);	
	strcpy(directory[dirindex].len, finlen);	
	
	return nbyte;
}


int fs_get_filesize(int fildes) {
	int i;
	int length;

	/* make sure file being requested is valid in OFT */
	if (oft[fildes].open) {
		return -1;
	}
	
	/* get filename */	
	char *filename = oft[fildes].filename;
	for (i=0; i<DIR_LENGTH; i++) {
		directoryBlock curr = directory[i];
		if (strcmp(curr.fn, filename) == 0) {
			char *temp = curr.len;
			length = (int) strtol(temp, (char **)NULL, 10);
			return length;
		}
	}


	/* if this is reached, another error occured */
	return -1;
}

int fs_lseek(int fildes, off_t offset) {
	int i;
	directoryBlock dir;
	int filelength;
	int curroffset;
	int newoffset;
	int temp;
	char *temp1;

	/* check file to see if it is in the OFT */
	if (oft[fildes].open) {
		return -1;
	}
	
	/* get corresponding directory */
	char *filename = oft[fildes].filename;
	for (i=0; i<DIR_LENGTH; i++) {
		if (strcmp(directory[i].fn, filename) == 0) {
			dir = directory[i];
		}
	}
	
	/* get length from the dir block; need to convert from string */
	temp1 = dir.len;
	filelength = (int) strtol(temp1, (char **)NULL, 10);

	/* get the new offset, current offset, file length */
	curroffset = oft[fildes].offset;
	temp = (int) curroffset	+ offset;

	/* do out of bounds checks */	
	if (temp < 0) {
		/* trying to point before beginning */
		return -1;
	}

	if (temp > filelength) {
		/* trying to point after the end of file */
		return -1;
	}

	newoffset = temp;
	oft[fildes].offset = newoffset;
		
	return 0;
}

int fs_truncate(int fildes, off_t length) {
	int i;
	int len;
	int blocknumber;
	int blocknumber1;
	int blocksneeded;
	int fatsneeded;
	directoryBlock dir;	

	if (oft[fildes].open) {
		return -1;
	}
	
	/* get corresponding directory */
	char *filename = oft[fildes].filename;
	for (i=0; i<DIR_LENGTH; i++) {
		if (strcmp(directory[i].fn, filename) == 0) {
			dir = directory[i];
		}
	}
	char *templen = dir.len;
	char *tempbn = dir.bn;
	blocknumber = (int) strtol(tempbn, (char **)NULL, 10);
	blocknumber1 = blocknumber;	
	len = (int) strtol(templen, (char **)NULL, 10);

	/* check if truncating length is greater than size of file */
	if (len < length) {
		return -1;
	}

	/* read through entire file, hold in temp buffer */
	blocksneeded = (int) (length / 16) + 1;
	fatsneeded = (int) (blocksneeded / 4) + 1;
	
	char *temphold = malloc(len);
	for (i=0; i<fatsneeded; i++) {
		char *tempiter = malloc(BLOCK_SIZE);
		strcpy(tempiter, disk[blocknumber]);
		strcat(temphold, tempiter);
		blocknumber += 1;
	}

	for (i=length; i< len; i++) {
		temphold[i] = '\0';
	}
	
	int j;
	int temp = 0;
	for (i=0; i<fatsneeded; i++) {
		for (j=0; j<BLOCK_SIZE; j++) {
			disk[blocknumber1][j] = temphold[temp];
			temp += 1;
			if (temphold[j] == '\0') break;
		}
		blocknumber1 += 1;
	}
	return 0;
}


