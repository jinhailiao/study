//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			hfatpath.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-19  create                  Kingsea
//---------------------------------------------------------------------------------
#include "hfstype.h"
#include "haiclib.h"
#include "hfatdent.h"
#include "hfatstr.h"


S_BYTE  CWD[HAI_PATH_MAX+4];
S_DWORD chdirStartClust = 0;

S_DWORD _hai_CwdOperate(int operate, S_BYTE *pname)
{
	int len;
	S_BYTE *pSlash;
	S_DWORD m = 0;
	switch (operate)
	{
	case CWDOPERATE_CLRPATH:
		hai_memset(CWD, 0x00, sizeof(CWD));
		break;

	case CWDOPERATE_INITPATH:
		hai_memset(CWD, 0x00, sizeof(CWD));
		*(CWD+0) = *pname & ~0x20;
		*(CWD+1) = ':';
		*(CWD+2) = '\0';
		break;

	case CWDOPERATE_ADDPATH:
		if (IS_FAT_CUR_DIR(pname))
		{
		}
		else if (IS_FAT_BACK_DIR(pname))
		{
			pSlash = hai_FindTailChar(CWD, '\\');//must successful
			*pSlash = '\0';
		}
		else
		{
			len = hai_strlen(CWD);
			CWD[len] = '\\';
			hai_strcpy(CWD+len+1, pname);
		}
		break;

	case CWDOPERATE_COPYTO:
		hai_strcpy(pname, CWD);
		break;

	case CWDOPERATE_COPYFROM:
		hai_strcpy(CWD, pname);
		break;

	case CWDOPERATE_SETCLUS:
		chdirStartClust = (S_DWORD)pname;
		break;

	case CWDOPERATE_GETCLUS:
		m = chdirStartClust;
		break;

	case CWDOPERATE_GETLENG:
		m = hai_strlen(CWD);
		break;

	default:
		break;
	}
	return m;
}

int hai_GetTailChar(const S_BYTE *pstr)
{
	int val = 0;
	while (*pstr)
	{
		if (*pstr&0x80)
		{
			val = (int)HAI_MAKEWORD(pstr);
			pstr += 2;
		}
		else
		{
			val = (int)*pstr++;
		}
	}
	return val;
}

S_BYTE *hai_FirstStriStr(const S_BYTE *pdststr, const S_BYTE *psrcstr)
{
	int len = hai_strlen(psrcstr);

	if (len && !hai_memicmp(pdststr, psrcstr, len))
		return (S_BYTE *)(pdststr+len);
	else
		return S_NULL;
}

const S_BYTE *hai_DelFrontSpace(const S_BYTE *pszString)
{
	while (*pszString && *pszString==' ' && *pszString=='\t')
		pszString++;
	return pszString;
}

const S_BYTE *hai_DelFrontChar(const S_BYTE *pszString, int c)
{
	while (*pszString)
	{
		if (*pszString&0x80)
		{
			if ((int)HAI_MAKEWORD(pszString)!=c)
				break;
			pszString += 2;
		}
		else
		{
			if ((int)*pszString != c)
				break;
			pszString++;
		}
	}
	return pszString;
}

S_BYTE *hai_DelTailSpace(S_BYTE *pszString)
{
	S_BYTE *ptmp = S_NULL;
	S_BYTE *pdst = pszString;
	
	while (*pdst)
	{
		if (*pdst&0x80)
		{
			pdst += 2;
			ptmp = S_NULL;
		}
		else
		{
			if (*pdst==' '||*pdst=='\t')
			{
				if (ptmp==S_NULL)
					ptmp = pdst;
			}
			else
				ptmp = S_NULL;
			pdst++;
		}
	}
	if (ptmp)
		*ptmp = '\0';
	
	return (ptmp?ptmp:pdst);
}

S_BYTE *hai_DelTailChar(S_BYTE *pszString, int c)
{
	S_BYTE *ptmp = S_NULL;
	S_BYTE *pdst = pszString;
	
	while (*pdst)
	{
		if (*pdst&0x80)
		{
			if ((int)HAI_MAKEWORD(pdst) == c)
			{
				if (ptmp==S_NULL)
					ptmp = pdst;
			}
			else
				ptmp = S_NULL;
			pdst += 2;
		}
		else
		{
			if ((int)*pdst == c)
			{
				if (ptmp==S_NULL)
					ptmp = pdst;
			}
			else
				ptmp = S_NULL;
			pdst++;
		}
	}
	if (ptmp)
		*ptmp = '\0';
	
	return (ptmp?ptmp:pdst);
}

S_BYTE *hai_FindTailChar(const S_BYTE *pszString, int c)
{
	S_BYTE *ptmp = S_NULL;
	
	while (*pszString)
	{
		if (*pszString&0x80)
		{
			if ((int)HAI_MAKEWORD(pszString) == c)
				ptmp = (S_BYTE *)pszString;
			pszString += 2;
		}
		else
		{
			if ((int)*pszString==c)
				ptmp = (S_BYTE *)pszString;
			pszString++;
		}
	}
	
	return ptmp;
}

S_BYTE *hai_GetOneDirName(const S_BYTE *dirpath, S_BYTE *OutName)
{
	int num = 0;
	S_BYTE *pOutName = OutName;

	if (*dirpath == '\\')
		dirpath++;

	while (*dirpath && *dirpath != '\\' && num < HAI_FILENAME_MAX-2)
	{
		if (*dirpath&0x80)
		{
			*pOutName++ = *dirpath++;
			num++;
		}
		*pOutName++ = *dirpath++;
		num++;
	}
	*pOutName = '\0';

	return (S_BYTE *)dirpath;
}

int hai_Make83Format(const S_BYTE *LongName, S_BYTE *pName83)
{
	int i;
	S_BYTE *p, *q;
	S_BYTE Name83[HAI_FILENAME_MAX+4];
	
	hai_strcpy(Name83, LongName);
	
	hai_strupr(Name83);
	
	for (p = Name83; *p;)//
	{
		if (*p&0x80)
		{
			p += 2;
		}
		else
		{
			if (LONGNAME_ILLEGAL_CHAR(*p))
				return 0;
			else if (LONGNAME_LEGAL_CHAR(*p))
				*p = '_';
			p++;
		}
	}
	
	for (p = q = Name83; *p; p++)
	{
		if (*p&0x80)
		{
			*q++ = *p++;
			*q++ = *p;
		}
		else
		{
			if (*p!=' ' && *p!='\t')
				*q++ = *p;
		}
	}
	*q = '\0';
	
	p = Name83;
	while (*p == '.')
		p++;

	for (q = Name83, i = 0; p[i]&&p[i]!='.'&&i<8; i++)
	{
		if (p[i]&0x80)
		{
			if (i+1 < 8)
				*q++ = p[i++], *q++ = p[i];
			else
				break;
		}
		else
		{
			*q++ = p[i];
		}
	}

	p = hai_FindTailChar(p+i, '.');
	if (p && *++p)
	{
		*q++ = '.';
		for (i = 0; p[i]&&i<3; i++)
		{
			if (p[i]&0x80)
			{
				if (i+1 < 3)
					*q++ = p[i++], *q++ = p[i];
				else
					break;
			}
			else
			{
				*q++ = p[i];
			}
		}
	}
	*q = '\0';
	
	hai_strcpy(pName83, Name83);
	
	return (int)(q-Name83);
}

int hai_GetFileNameFromDent(const S_BYTE *dent, S_BYTE *DstName)
{
	S_BYTE *pend;

	hai_memcpy(DstName, dent, 8);
	DstName[8] = '\0';
	pend = hai_DelTailSpace(DstName);
	*pend++ = '.';
	hai_memcpy(pend, dent+8, 3);
	*(pend+3) = '\0';
	hai_DelTailSpace(pend);
	hai_DelTailChar(pend-1, '.');

	if (*DstName == DIR_NAME_INSTEAD)
		*DstName = DIR_NAME_MAGIC;
	
	return 0;
}

S_BYTE *hai_WriteShortName2Dent(S_BYTE *dentbuf, S_BYTE *Name83)
{
	S_BYTE *p;

	hai_memset(dentbuf, 0x20, sDIR_Name);
	for (p = dentbuf; *Name83 && *Name83!='.'; Name83++, p++)
	{
		if (*Name83&0x80)
			*p++ = *Name83++;
		*p = *Name83;
	}
	if (*Name83 == '.')
		Name83++;
	for (p = dentbuf+8; *Name83; Name83++, p++)
	{
		if (*Name83&0x80)
			*p++ = *Name83++;
		*p = *Name83;
	}

	if (*dentbuf == DIR_NAME_MAGIC)
		*dentbuf = DIR_NAME_INSTEAD;

	return dentbuf;
}

int hai_GetFileNameFromLDent(const S_BYTE *dent, S_BYTE *DstName)
{
	int i, n, size;
	S_WORD v;
	S_BYTE *pdent, *pName = DstName;

	for (n = 0; n < 3; n++)
	{
		switch (n)
		{
		case 0:
			pdent = (S_BYTE *)pLDIR_Name1(dent);
			size = sLDIR_Name1;
			break;
		case 1:
			pdent = (S_BYTE *)pLDIR_Name2(dent);
			size = sLDIR_Name2;
			break;
		case 2:
			pdent = (S_BYTE *)pLDIR_Name3(dent);
			size = sLDIR_Name3;
			break;
		}
		for (i = 0; i < size; i+=2)
		{
			v = HAI_MAKEWORD(pdent);
			if (v == 0)
			{
				*pName = 0, *(pName+1) = 0;
				return (int)(pName-DstName);
			}
			else if (v == 0xFFFF)//may be error
			{
				*pName = 0, *(pName+1) = 0;
				return 0;			
			}
			*pName++ = *pdent++, *pName++ = *pdent++;
		}
	}
	
	return (int)(pName-DstName);
}

S_BYTE *hai_MakeLegalFatFileName(S_BYTE *filename)
{
	int c;
	S_BYTE Name83[16];

	while (*filename && (*filename==' '||*filename=='\t'||*filename=='.'))
		filename++;
	if (!*filename)
		return S_NULL;

	c = hai_GetTailChar(filename);
	while (c==' ' || c=='\t' || c=='.')
	{
		if (c=='.')
			hai_DelTailChar(filename, '.');
		else
			hai_DelTailSpace(filename);
		c = hai_GetTailChar(filename);
	}

	hai_memset(Name83, 0x00, sizeof(Name83));
	if (!hai_Make83Format(filename, Name83))
		return S_NULL;

	return filename;
}

S_BYTE *hai_MakeNumericTailName(S_BYTE *Name83)
{
	int n, m;
	S_DWORD num = 0;
	S_BYTE *p, *q, tmpbuf[16], numstr[12];

	hai_strcpy(tmpbuf, Name83);
	if (q = hai_strchr(tmpbuf, '.'))//del ext
		*q = '\0';
	
	if (p = hai_FindTailChar(tmpbuf, '~'))//split base name and number
	{
		for (q=p+1; *q; q++)// get number
		{
			if (!(*q&0x80) && *q>='0' && *q<='9')
				num = num*10 + (*q-'0');
			else
			{
				num = 0;
				break;
			}
		}
		if (num)//del number string
			*p = '\0';
	}

	num += 1;
	for (n=0; num; n++)
	{
		numstr[n] = (S_BYTE)(num%10+'0');
		num /= 10;
	}
	numstr[n++] = '~';

	for (p=q=tmpbuf,m=0; *q && *q!='.' && m < 8-n;)
	{
		if (*q&0x80)
		{
			if (m+1 < 8-n)
			{
				*p++ = *q++; *p++ = *q++;
				m += 2;
			}
			else
				break;
		}
		else
		{
			*p++ = *q++;
			m++;
		}
	}
	for (n--; n >= 0; n--)
		*p++ = numstr[n];

	q = hai_strchr(Name83, '.');
	if (q)
		hai_strcpy(p, q);
	else
		*p = '\0';

	hai_strcpy(Name83, tmpbuf);

	return Name83;
}

int hai_CountChar(const S_BYTE *pStr)
{
	int cnt = 0;

	while (*pStr)
	{
		if (*pStr&0x80)
			pStr += 2;
		else
			pStr += 1;
		cnt++;
	}
	return cnt;
}

S_WORD hai_GetOtherCodeFromUni(S_WORD code)
{
	extern unsigned short hai_Unicode2GB(unsigned short unicode);
	S_WORD tmp = hai_Unicode2GB(code);
	
	return tmp? tmp : ((code>>8)|(code<<8));
}

int hai_Unicode2OtherCode(S_BYTE *pBuffer)
{
	S_BYTE *psrc = pBuffer, *pdst = pBuffer;
	S_WORD code;

	while (code = HAI_MAKEWORD(psrc))
	{
		if (code < 0x80)//ascii
			*pdst++ = (S_BYTE)code;
		else
		{
			code = hai_GetOtherCodeFromUni(code);
			*pdst++ = (S_BYTE)(code>>8);
			*pdst++ = (S_BYTE)code;
		}
		psrc += 2;
	}
	*pdst = '\0';

	return (int)(pdst-pBuffer);
}

S_WORD hai_GetUnicodeFromOtherCode(S_WORD code)
{
	extern unsigned short hai_GB2Unicode(unsigned short unicode);
	S_WORD tmp = hai_GB2Unicode(code);
	
	return tmp? tmp : ((code>>8)|(code<<8));
}

int hai_OtherCode2Unicode(S_BYTE *UniStr, const S_BYTE *OtherStr)
{
	const S_BYTE *psrc = OtherStr;
	S_BYTE *pdst = UniStr;
	S_WORD code;

	while (*psrc)
	{
		if (*psrc&0x80)
		{
			code = hai_GetUnicodeFromOtherCode((S_WORD)((((S_WORD)*psrc)<<8)|*(psrc+1)));
			*pdst++ = (S_BYTE)code;
			*pdst++ = (S_BYTE)(code>>8);
			psrc += 2;
		}
		else
		{
			*pdst++ = *psrc++;
			*pdst++ = 0x00;
		}
	}
	*pdst = '\0', *(pdst+1) = '\0';

	return (int)(pdst-UniStr);
}

S_WORD hai_CountByteUni(const S_BYTE *UniStr)
{
	const S_BYTE *pUniStr = UniStr;

	while (HAI_MAKEWORD(pUniStr))
		pUniStr += 2;

	return (S_WORD)(pUniStr-UniStr);
}


