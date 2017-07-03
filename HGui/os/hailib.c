//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			hailib.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 2.0.0		2006-09-06  create                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------
#include "haitype.h"


S_BOOL hai_RleDecode(S_BYTE *poutbuf, S_BYTE *pinbuf, S_WORD WidthBytes)
{
	S_BYTE ch;
	S_WORD cnt = 1;
	S_WORD skip = (WidthBytes+3 & ~0x3) - WidthBytes;// 4 bytes align

	while(ch = *pinbuf++)
	{
		if (ch&0x80)
		{
			ch &= 0x7F;
			while (ch--)
			{
				*poutbuf++ = *pinbuf;
				if (cnt++ == WidthBytes) // 4 bytes align 
				{
					cnt = 1;
					poutbuf += skip;
				}
			}
			pinbuf++;
		}
		else
		{
			while (ch--)
			{
				*poutbuf++ = *pinbuf++;
				if (cnt++ == WidthBytes) // 4 bytes align 
				{
					cnt = 1;
					poutbuf += skip;
				}
			}
		}
	}
	return S_TRUE;
}

S_BYTE *hai_strchr(const S_BYTE *pszString, S_BYTE c)
{
	const S_BYTE *p = pszString;
	
	if (pszString == S_NULL)// || c == 0
		return S_NULL;
	
	while (1)
	{
		if (*p & 0x80)
		{
			p += 2;
		}
		else
		{
			if (*p == c)
				return (S_BYTE *)p;
			if (*p == 0)
				return S_NULL;
			++p;
		}
	}
}

S_WORD  hai_strlen(const S_BYTE *pszString)
{
	S_WORD  num = 0;
	
	if (pszString == S_NULL)
		return 0;
	
	while (*pszString)
	{
		if (*pszString & 0x80)
		{
			pszString += 2;
			num += 2;
		}
		else
		{
			pszString++;
			num++;
		}
	}
	
	return num;
}

S_BYTE  *hai_strcpy(S_BYTE *pszDst, const S_BYTE *pszSrc)
{
	S_BYTE *dst = pszDst;
	
	if (pszDst == S_NULL || pszSrc == S_NULL)
		return S_NULL;
	
	while (*pszSrc)
	{
		if (*pszSrc & 0x80)
		{
			*dst++ = *pszSrc++;
			*dst++ = *pszSrc++;
		}
		else
		{
			*dst++ = *pszSrc++;
		}
	}
	*dst = '\0';
	return pszDst;
}

S_VOID hai_CutString(S_BYTE *pszText, S_SHORT AscWidth, S_SHORT DisWidth)
{
	S_WORD w = 0;

	if (!pszText || AscWidth <= 0 || DisWidth <= 0)
		return;

	while (*pszText++)
	{
		if (*pszText & 0x80)
		{
			if (w + AscWidth*2 > DisWidth)
			{
				*(pszText-1) = 0;
				break;
			}
			w += AscWidth * 2;
			pszText++;
		}
		else
		{
			if (w + AscWidth > DisWidth)
			{
				*(pszText-1) = 0;
				break;
			}
			w += AscWidth;
		}
	}
}

S_VOID *HG_memcpy(S_VOID *dest, const S_VOID *src, S_UINT count)
{
	S_BYTE *pDst = (S_BYTE *)dest;
	S_BYTE *pSrc = (S_BYTE *)src;

	if (pDst < pSrc)
	{
		while(count--)
			*pDst++ = *pSrc++;
	}
	else if (pDst > pSrc)
	{
		while(count--)
			*(pDst+count) = *(pSrc+count);
	}

	return dest;
}

S_INT HG_memcmp(const S_VOID *buf1, const S_VOID *buf2, S_UINT count)
{
	S_BYTE *p1 = (S_BYTE *)buf1;
	S_BYTE *p2 = (S_BYTE *)buf2;

	while (count--)
	{
		if (*p1++ != *p2++)
			break;
	}

	return (*(--p1)-*(--p2));
}

S_VOID *HG_memset(S_VOID *dest, S_INT c, S_UINT count)
{
	S_BYTE ch = (S_BYTE)c;
	S_BYTE *pDst = (S_BYTE *)dest;

	while(count--)
		*pDst++ = ch;

	return dest;
}
