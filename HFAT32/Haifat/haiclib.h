//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			haiclib.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-01  create                  Kingsea
// 1.0.1		2008-09-25	modify					Kingsea
//---------------------------------------------------------------------------------
#ifndef __HAICLIB_H__
#define __HAICLIB_H__

//config c lib
#define HAVE_C_LIB
#define HAVE_C_LIB_MEMSET
#define HAVE_C_LIB_MEMCMP
#define HAVE_C_LIB_MEMICMP
#define HAVE_C_LIB_MEMCPY
#define HAVE_C_LIB_MEMMOVE
#define HAVE_C_LIB_STRLEN
#define HAVE_C_LIB_STRCPY
#define HAVE_C_LIB_STRNCPY
#define HAVE_C_LIB_STRCAT
#define HAVE_C_LIB_STRCHR
#define HAVE_C_LIB_STRUPR
#define HAVE_C_LIB_STRCMP
#define HAVE_C_LIB_STRICMP

#ifdef HAVE_C_LIB
#include <string.h>
#else

#endif //HAVE_C_LIB

#ifndef size_t
typedef unsigned int	size_t;
#define _SIZE_T_DEFINED
#endif

#ifdef HAVE_C_LIB_MEMSET
#define hai_memset memset
#else
void *hai_memset(void *dest, int c, size_t count);
#endif

#ifdef HAVE_C_LIB_MEMCMP
#define hai_memcmp memcmp
#else
int   hai_memcmp(const void *buf1, const void *buf2, size_t count);
#endif

#ifdef HAVE_C_LIB_MEMICMP
#define hai_memicmp memicmp
#else
int   hai_memicmp(const void *buf1, const void *buf2, unsigned int count);
#endif

#ifdef HAVE_C_LIB_MEMCPY
#define hai_memcpy memcpy
#else
void *hai_memcpy(void *dest, const void *src, size_t count);
#endif

#ifdef HAVE_C_LIB_MEMMOVE
#define hai_memmove memmove
#else
void *hai_memmove(void *dest, const void *src, size_t count);
#endif

#ifdef HAVE_C_LIB_STRLEN
#define hai_strlen strlen
#else
size_t hai_strlen(const char *string);
#endif

#ifdef HAVE_C_LIB_STRCPY
#define hai_strcpy strcpy
#else
char *hai_strcpy(char *strDestination, const char *strSource);
#endif

#ifdef HAVE_C_LIB_STRNCPY
#define hai_strncpy strncpy
#else
char *hai_strncpy(char *strDest, const char *strSource, size_t count);
#endif

#ifdef HAVE_C_LIB_STRCAT
#define hai_strcat strcat
#else
char *hai_strcat(char *strDestination, const char *strSource);
#endif

#ifdef HAVE_C_LIB_STRCHR
#define hai_strchr strchr
#else
char *hai_strchr(const char *string, int c);
#endif

#ifdef HAVE_C_LIB_STRUPR
#define hai_strupr strupr
#else
char *hai_strupr(char *string);
#endif

#ifdef HAVE_C_LIB_STRCMP
#define hai_strcmp strcmp
#else
int   hai_strcmp(const char *string1, const char *string2);
#endif

#ifdef HAVE_C_LIB_STRICMP
#define hai_stricmp stricmp
#else
int   hai_stricmp(const char *string1, const char *string2);
#endif


#endif //__HAICLIB_H__

