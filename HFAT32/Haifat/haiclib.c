//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			haiclib.c
// Description:		instead of clib if necessary
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-09-25  create                  Kingsea
//---------------------------------------------------------------------------------
#include "haiclib.h"

#ifdef HAVE_C_LIB_MEMSET
//do nothing
#else
void *hai_memset(void *dest, int c, size_t count)
{
}
#endif

#ifdef HAVE_C_LIB_MEMCMP
//do nothing
#else
int   hai_memcmp(const void *buf1, const void *buf2, size_t count)
{
}
#endif

#ifdef HAVE_C_LIB_MEMICMP
//do nothing
#else
int   hai_memicmp(const void *buf1, const void *buf2, unsigned int count)
{
	char ch1, ch2;
	unsigned int temp = 0;
	char *p1 = (char *)buf1, *p2 = (char *)buf2;
	while (temp < count)
	{
		if (*p1&0x80)
		{
			if (*p1 != *p2)
				break;
			p1++, p2++;
			temp++;
			if (temp == count)
				break;
			if (*p1 != *p2)
				break;
			p1++, p2++;
			temp++;
		}
		else
		{
			ch1 = (*p1>='A'&&*p1<='Z')?*p1+('a'-'A'):*p1;
			ch2 = (*p2>='A'&&*p2<='Z')?*p2+('a'-'A'):*p2;
			if (ch1 != ch2)
				break;
			p1++, p2++;
			temp++;
		}
	}

	if (temp == count)
		return 0;
	return *p1-*p2;
}
#endif

#ifdef HAVE_C_LIB_MEMCPY
//do nothing
#else
void *hai_memcpy(void *dest, const void *src, size_t count)
{
}
#endif

#ifdef HAVE_C_LIB_MEMMOVE
//do nothing
#else
void *hai_memmove(void *dest, const void *src, size_t count)
{
}
#endif

#ifdef HAVE_C_LIB_STRLEN
//do nothing
#else
size_t hai_strlen(const char *string)
{
}
#endif

#ifdef HAVE_C_LIB_STRCPY
//do nothing
#else
char *hai_strcpy(char *strDestination, const char *strSource)
{
}
#endif

#ifdef HAVE_C_LIB_STRNCPY
//do nothing
#else
char *hai_strncpy(char *strDest, const char *strSource, size_t count)
{
}
#endif

#ifdef HAVE_C_LIB_STRCAT
//do nothing
#else
char *hai_strcat(char *strDestination, const char *strSource)
{
}
#endif

#ifdef HAVE_C_LIB_STRCHR
//do nothing
#else
char *hai_strchr(const char *string, int c)
{
}
#endif

#ifdef HAVE_C_LIB_STRUPR
//do nothing
#else
char *hai_strupr(char *string)
{
	char *p = string;
	while (*p)
	{
		if (*p&0x80)
			p += 2;
		else
		{
			if (*p>='a' && *p<='z')
				*p = *p-('a'-'A');
			p++;
		}
	}
	return string;
}
#endif

#ifdef HAVE_C_LIB_STRCMP
//do nothing
#else
int   hai_strcmp(const char *string1, const char *string2)
{
}
#endif

#ifdef HAVE_C_LIB_STRICMP
//do nothing
#else
int   hai_stricmp(const char *string1, const char *string2)
{
	char ch1, ch2;
	while (*string1 && *string2)
	{
		if (*string1&0x80)
		{
			if (*string1 != *string2)
				break;
			string1++, string2++;
			if (*string1 != *string2)
				break;
			string1++, string2++;
		}
		else
		{
			ch1 = (*string1>='A'&&*string1<='Z')?*string1+('a'-'A'):*string1;
			ch2 = (*string2>='A'&&*string2<='Z')?*string2+('a'-'A'):*string2;
			if (ch1 != ch2)
				break;
			string1++, string2++;
		}
	}
	return *string1-*string2;
}
#endif

