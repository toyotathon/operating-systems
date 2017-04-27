#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "disk.h"

#define DIR_LENGTH 8
#define FAT_LENGTH 16
#define DATA_LENGTH 32

/* Structs needed to implement filesystem */
typedef struct {
	char *status;
	char *bn;
	char *fn;
	char *len;
} directoryBlock;

typedef struct {
	char *status1;
	char *block1;
	char *status2;
	char *block2;
	char *status3;
	char *block3;
	char *status4;
	char *block4;
} fatBlock;

// TODO
typedef struct {

} oftBlock;

directoryBlock newDirectoryBlock(char s[1], char bn[2], char fn[4], char len[3]) {
	directoryBlock db;

	db.status = s;
	db.bn = bn;
	db.fn = fn;
	db.len = len;

	return db;
}

fatBlock newFATBlock(char s1[1],char b1[3],char s2[1],char b2[3],char s3[1],char b3[3],char s4[1],char b4[3]) {
	fatBlock f;

	f.status1 	= s1;
	f.block1 	= b1;

	f.status2 	= s2;
	f.block2 	= b2;

	f.status3 	= s3;
	f.block3 	= b3;

	f.status4 	= s4;
	f.block4 	= b4;

	return f;
}

/* global memory arrays */
directoryBlock directory[DIR_LENGTH];
fatBlock fat[FAT_LENGTH];

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
		char directory[BLOCK_SIZE] = 
			{'f', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};	
		block_write(i, directory);
	}

	/* initializing FAT */
	for (i=9; i<17; i++) {
		char fat[BLOCK_SIZE] = 
			{'f', ' ', ' ', ' ', 'f', ' ', ' ', ' ', 'f', ' ', ' ', ' ', 'f', ' ', ' ', ' '};	
		block_write(i, fat);
	}
	
	/* initializing data */
	for (i=32; i<64; i++) {
		char data[BLOCK_SIZE] = 
			{'f', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};	
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
		char s[1];
		char bn[2];
		char fn[4];
		char len[3];

		/* read in directory block from memory */
		block_read(i, temp);	

		/* read status of block */
		s[0] = temp[0];

		/* read block number of block */	
		bn[0] = temp[1];
		bn[1] = temp[2];

		/* read file number of block */	
		fn[0] = temp[3];
		fn[1] = temp[4];
		fn[2] = temp[5];
		fn[3] = temp[6];

		/* read length of file */
		len[0] = temp[7];	
		len[1] = temp[8];
		len[2] = temp[9];	
		
		int save = i-1;	
		directory[save] = newDirectoryBlock(s, bn, fn, len);
	}
	
	int save = 0;
	for (i=9; i<17; i++) {
		char temp[BLOCK_SIZE];
		char s1[1];
		char b1[3];

		char s2[1];
		char b2[3];

		char s3[1];
		char b3[3];

		char s4[1];
		char b4[3];	
		
		block_read(i, temp);
		printf("block %d data: %s\n", i, temp);	
	
		s1[0] = temp[0];
		b1[0] = temp[1];
		b1[1] = temp[2];
		b1[2] = temp[3];
		printf("block 1 status: %s\n", s1);
		printf("block 1 content: %s\n", b1);

		s2[0] = temp[4];
		b2[0] = temp[5];
		b2[1] = temp[6];
		b2[2] = temp[7];
		printf("block 2 status: %s\n", s2);
		printf("block 2 content: %s\n", b2);

		s3[0] = temp[8];
		b3[0] = temp[9];
		b3[1] = temp[10];
		b3[2] = temp[11];
		printf("block 3 status: %s\n", s3);
		printf("block 3 content: %s\n", b3);

		s4[0] = temp[12];
		b4[0] = temp[13];
		b4[1] = temp[14];
		b4[2] = temp[15];
		printf("block 4 status: %s\n", s4);
		printf("block 4 content: %s\n", b4);

		fat[save] = newFATBlock(s1,b1,s2,b2,s3,b3,s4,b4);
		printf("index: %d\n", save);
		save++;	
	}
	return 1;
}

int dismount_fs(char *disk_name) {return 1;}

int fs_create(char *name) {return 1;}

int fs_open(char *name) {return 1;}

int fs_close(int fildes) {return 1;}

int fs_delete(char *name) {return 1;}

int fs_read(int fildes, void *buf, size_t nbyte) {return 1;}

int fs_write(int filedes, void *buff, size_t nbyte) {return 1;}

int fs_get_filesize(int fildes) {return 1;}

int fs_lseek(int fildes, off_t offset) {return 1;}

int fs_truncate(int fildes, off_t length) {return 1;}


