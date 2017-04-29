#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
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

/* global memory arrays */
directoryBlock directory[DIR_LENGTH];
fatBlock fat[FAT_LENGTH];
char oft[4][4];
bool freeDirectory[DIR_LENGTH];

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
			{'.', '\0', '\0', '\0',
			 '.', '\0', '\0', '\0', 
			 '.', '\0', '\0', '\0', 
			 '.', '\0', '\0', '\0'};	
		block_write(i, fat);
	}
	
	/* initializing data */
	for (i=32; i<64; i++) {
		char data[BLOCK_SIZE] = 		
		{'.', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};	
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

	/* initialize OFT values, set open flags */
	for (i=0; i<4; i++) {
		oft[i][0] = '.';
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

	/* TODO ANY OTHER METADATA TO SAVE GETS WRITTEN HERE */

	close_disk();
	return 1;
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
			printf("filename in use\n");
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
	return 1;
}

int fs_close(int fildes) {return 1;}

int fs_delete(char *name) {return 1;}

int fs_read(int fildes, void *buf, size_t nbyte) {return 1;}

int fs_write(int filedes, void *buff, size_t nbyte) {return 1;}

int fs_get_filesize(int fildes) {return 1;}

int fs_lseek(int fildes, off_t offset) {return 1;}

int fs_truncate(int fildes, off_t length) {return 1;}


