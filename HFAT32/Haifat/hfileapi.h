//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			hfileapi.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-01  create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __H_FILEAPI_H__
#define __H_FILEAPI_H__


typedef unsigned long* HFILE;
#ifndef size_t
typedef unsigned int	size_t;
#define _SIZE_T_DEFINED
#endif
struct finddata
{
	unsigned attrib;
	long CreatTime;
	long AccesTime;
	long WriteTime;
	unsigned long size;
	char name[260];
};


#define SEEK_CUR    1
#define SEEK_END    2
#define SEEK_SET    0


//file system function prototypes
//
int   hai_InitFileSys(void);

//int   hai_format(char *disk, char *fat);
//return val:
//        0  successful;
//       -1 unknown error;
//       -2 disk in use
//       -3 invalid disk
//parameter:
//       disk  as "c:","C:", "d:"....
//       fat   "FAT32" or "FAT16"
int   hai_format(char *disk, char *fat);


//directory function prototypes
//
int   hai_chdir(const char *dirname);
char *hai_getcwd(char *buffer, int maxlen);
int   hai_mkdir(const char *dirname);
int   hai_rmdir(const char *dirname);


//file function prototypes
//
HFILE hai_fopen(const char *filename, const char *mode);
int   hai_fclose(HFILE hfile);
int   hai_fcloseall(void);

int hai_fgetc(HFILE hfile);
int hai_fputc(int c, HFILE hfile);
size_t hai_fwrite(const void *buffer, size_t size, size_t count, HFILE hfile);
size_t hai_fread(void *buffer, size_t size, size_t count, HFILE hfile);

int  hai_remove(const char *filename);
int  hai_rename(const char *oldname, const char *newname);
int  hai_fseek(HFILE hfile, long offset, int origin);
int  hai_ferror(HFILE hfile);
int  hai_feof(HFILE hfile);
long hai_ftell(HFILE hfile);
void hai_clearerr(HFILE hfile);


//find function prototypes
//

//return val: if successful, return a find handle. Otherwise return 0;
long hai_findfirst(char *filespec, struct finddata *fileinfo);

//return val: if successful, return 0. Otherwise return !0;
int  hai_findnext(long handle, struct finddata *fileinfo);

//return val: if successful, return 0. Otherwise return !0;
int  hai_findclose(long handle);


//flush function prototypes
//
int hai_fflush(HFILE hfile);
int hai_flushall(void);



#endif //__H_FILEAPI_H__

