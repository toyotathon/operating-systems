#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int make_fs(char *disk_name);
int mount_fs(char *disk_name);
int dismount_fs(char *disk_name);
int fs_open(char *name);
int fs_close(int fildes);
int fs_create(char *name);
int fs_delete(char *name);
int fs_read(int fildes, void *buf, size_t nbyte);
int fs_write(int fildes, void *buf, size_t nbyte);
int fs_get_filesize(int fildes);
int fs_lseek(int fildes, off_t offset);
int fs_truncate(int fildes, off_t length);

size_t nbyte;
off_t offset;
off_t length;
int temp;

#define BUFFER_SIZE 80

int i;

int status;

int totalNum;

char tempBuf[BUFFER_SIZE];

int fda, fdb, fdc, fdd, fde; // 5 file descriptors

int main() {

  status = make_fs("mydk");
  printf("status for make_fs = %d\n", status); // "status for make_fs = 0" should be printed

  status = mount_fs("mydk");
  printf("status for mount_fs = %d\n", status); // "status for mount_fs = 0" should be printed

  status = fs_create("fa");
  printf("status for fs_create(""fa"") = %d\n", status); // "status for fs_create("fa") = 0" should be printed

  fda = fs_open("fa");
  printf("fda for fs_open(""fa"") = %d\n", fda); // "fda for fs_open("fa") = a nonnegative int (0..3)" should be printed

  temp = fs_open("fa"); 
  printf("temp for fs_open(""fa"") = %d\n", temp); // "temp for fs_open("fa") = -1" should be printed

  char bufa[] = "Hi, there";
  nbyte = 9;
  totalNum = fs_write(fda,bufa,nbyte);
  printf("total number of bytes written into file fa is %d\n", totalNum); // "total number of bytes written into file fa is 9" should be printed

  status = fs_create("fb");
  printf("status for fs_create(""fb"") = %d\n", status); // "status for fs_create("fb") = 0" should be printed

  fdb = fs_open("fb");
  printf("fdb for fs_open(""fb"") = %d\n", fdb); // "fdb for fs_open("fb") = a nonnegative int (0..3)" should be printed

  char bufb[] = "The goal of this project";
  nbyte = 24;
  totalNum = fs_write(fdb,bufb,nbyte);
  printf("total number of bytes written into file fb is %d\n", totalNum); // "total number of bytes written into file fb is 24" should be printed

  status = fs_create("fc");
  printf("status for fs_create(""fc"") = %d\n", status); // "status for fs_create("fc") = 0" should be printed

  fdc = fs_open("fc");
  printf("fdc for fs_open(""fc"") = %d\n", fdc); // "fdc for fs_open("fc") = a nonnegative int (0..3)" should be printed

  char bufc[] = "CS4414 is fun";
  nbyte = 13;
  totalNum = fs_write(fdc,bufc,nbyte);
  printf("total number of bytes written into file fc is %d\n", totalNum); // "total number of bytes written into file fc is 13" should be printed

  status = fs_create("fd");
  printf("status for fs_create(""fd"") = %d\n", status); // "status for fs_create("fd") = 0" should be printed

  fdd = fs_open("fd");
  printf("fdd for fs_open(""fd"") = %d\n", fdd); // "fdd for fs_open("fd") = a nonnegative int (0..3)" should be printed

  char bufd[] = "I love computer programming because I can ask computers to do something for me";
  nbyte = 78;
  totalNum = fs_write(fdd,bufd,nbyte);
  printf("total number of bytes written into file fd is %d\n", totalNum); // "total number of bytes written into file fd is 78" should be printed

  status = fs_create("fe");
  printf("status for fs_create(""fe"") = %d\n", status); // "status for fs_create("fe") = 0" should be printed

  length = fs_get_filesize(fdb);
  printf("file size of file fb = %d\n", (int) length); // "file size of file fb = 24" should be printed

  offset = -80;
  status = fs_lseek(fdd,offset);
  printf("status for fs_lseek(fdd,offset) = %d\n", status); // "status for fs_lseek(fdd,offset) = -1" should be printed

  offset = -22;
  status = fs_lseek(fdd,offset);
  printf("status for fs_lseek(fdd,offset) = %d\n", status); // "status for fs_lseek(fdd,offset) = 0" should be printed

  nbyte = 15;
  totalNum = fs_read(fdd,tempBuf,nbyte);
  printf("total number of bytes read from file fd = %d\n", totalNum); // "total number of bytes read from file fd = 15" should be printed

  for (i = 0; i < nbyte; i++)
	  printf("%c",tempBuf[i]); // "to do something" should be printed
  printf("\n");

  length = 9;
  status = fs_truncate(fdc,length);
  printf("status for fs_truncate(fdc,length) = %d\n", status); // "status for fs_truncate(fdc,length) = 0" should be printed 

  nbyte = 6;
  totalNum = fs_read(fdc,tempBuf,nbyte);
  printf("total number of bytes read from file fc = %d\n", totalNum); // "total number of bytes read from file fc = 6" should be printed

  for (i = 0; i < nbyte; i++)
	  printf("%c",tempBuf[i]); // "CS4414" should be printed
  printf("\n");
  
  status = fs_close(fdb);
  printf("status for fs_close(fdb) = %d\n", status); // "status for fs_close(fdb) = 0" should be printed

  totalNum = fs_read(fdb,tempBuf,nbyte);
  printf("totalNum for fs_read(fdb,tempBuf,nbyte) = %d\n", totalNum); // "totalNum for fs_read(fdb,tempBuf,nbyte) = -1" should be printed 

  fde = fs_open("fe");
  printf("fde for fs_open(""fe"") = %d\n", fde); // "fde for fs_open("fe") = a nonnegative int (0..3)" should be printed

  char bufe[] = "I am glad this is the last project for the semester\n";
  nbyte = 51;
  totalNum = fs_write(fde,bufe,nbyte);
  printf("total number of bytes written into file fe = %d\n", totalNum); // "total number of bytes written into file fe = 51" should be printed

  status = fs_delete("fd");
  printf("status for fs_delete(""fd"") = %d\n", status); // "status for fs_delete("fd") = -1" should be printed

  status = fs_delete("fb");
  printf("status for fs_delete(""fb"") = %d\n", status); // "status for fs_delete("fb") = 0" should be printed

  status = dismount_fs("mydk");
  printf("status for dismount_fs(""mydk"") = %d\n", status); // "status for dismount_fs("mydk") = 0" should be printed
  
  status = mount_fs("mydk");
  printf("status for mount_fs(""mydk"") = %d\n", status); // "status for mount_fs("mydk") = 0" should be printed

  fda = fs_open("fa");
  printf("fda for fs_open(""fa"") = %d\n", fda); // "fda for fs_open("fa") = a nonnegative int (0..3)" should be printed

  fdb = fs_open("fb");
  printf("fdb for fs_open(""fb"") = %d\n", fdb); // "fdb for fs_open("fb") = -1" should be printed

  fdc = fs_open("fc");
  printf("fdc for fs_open(""fc"") = %d\n", fdc); // "fdc for fs_open("fc") = a nonnegative int (0..3)" should be printed

  fdd = fs_open("fd");
  printf("fdd for fs_open(""fd"") = %d\n", fdd); // "fdd for fs_open("fd") = a nonnegative int (0..3)" should be printed

  fde = fs_open("fe");
  printf("fde for fs_open(""fe"") = %d\n", fde); // "fde for fs_open("fe") = a nonnegative int (0..3)" should be printed

  nbyte = 9;
  totalNum = fs_read(fda,tempBuf,nbyte);
  printf("total number of bytes read from file fa = %d\n", totalNum); // "total number of bytes read from file fa = 9" should be printed
  for (i = 0; i < nbyte; i++)
	  printf("%c",tempBuf[i]); // "Hi, there" should be printed
  printf("\n");

  nbyte = 9;
  totalNum = fs_read(fdc,tempBuf,nbyte);
  printf("total number of bytes read from file fc = %d\n", totalNum); // "total number of bytes read from file fc = 9" should be printed
  for (i = 0; i < nbyte; i++)
	  printf("%c",tempBuf[i]); // "CS4414 is" should be printed
  printf("\n");

  nbyte = 78;
  totalNum = fs_read(fdd,tempBuf,nbyte);
  printf("total number of bytes read from file fd = %d\n", totalNum); // "total number of bytes read from file fd = 78" should be printed
  for (i = 0; i < nbyte; i++)
	  printf("%c",tempBuf[i]); // "I love computer programming because I can ask computers to do something for me" should be printed 
  printf("\n");

  nbyte = 51;
  totalNum = fs_read(fde,tempBuf,nbyte);
  printf("total number of bytes read from file fe = %d\n", totalNum); // "total number of bytes read from file fe = 51" should be printed
  for (i = 0; i < nbyte; i++)
	  printf("%c",tempBuf[i]); // "I am glad this is the last project for the semester" should be printed
  printf("\n");
 
  dismount_fs("mydk");
  printf("status for dismount_fs(""mydk"") = %d\n", status); // "status for dismount_fs("mydk") = 0" should be printed
 
  return 0;
}

 
