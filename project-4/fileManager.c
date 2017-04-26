#include <stddef.h>
#include <stdio.h>

#include "disk.h"

/* Structs needed to implement filesystem */

typedef struct {

} superBlock;

typedef struct {

} directoryBlock;

typedef struct {

} fatBlock;



int make_fs(char *disk_name) {
	int new_disk = make_disk(disk_name);
	int open = open_disk(disk_name);
	block_write(0, "hi");;
	return 1;
}

int mount_fs(char *disk_name) {return 1;}

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


