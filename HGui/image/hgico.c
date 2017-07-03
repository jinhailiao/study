//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			hgico.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 3.0.0		2008-03-20	cteate					Kingsea
//---------------------------------------------------------------------------------
#include "haitype.h"
#include "haihandle.h"
#include "haimem.h"
#include "_haigdi.h"
#include "haipic.h"


typedef struct tagIconDirectoryEntry
{
	S_BYTE  bWidth;
	S_BYTE  bHeight;
	S_BYTE  bColorCount;
	S_BYTE  bReserved;
	S_WORD  wPlanes;
	S_WORD  wBitCount;
	S_DWORD dwBytesInRes;
	S_DWORD dwImageOffset;
}
S_ICONDIRENTRY, *SP_ICONDIRENTRY;

typedef struct tagIconHead
{
	S_WORD idReserved;
	S_WORD idType;
	S_WORD idCount;
}
S_ICONHEADER, *SP_ICONHEADER;


S_DWORD _hai_ReadBitmapInfo(S_BYTE *pBmpData, BITMAPINFOHEADER *pbih);


S_BYTE hai_GetIcoFrameTotal(S_VOID *pIcoData)
{
	S_BYTE *picoData = pIcoData;
	S_ICONHEADER IconHeader;
	
	if (pIcoData == S_NULL)
		return 0;

	IconHeader.idReserved = HAI_MAKEWORD(picoData+0);
	IconHeader.idType  = HAI_MAKEWORD(picoData+2);
	IconHeader.idCount = HAI_MAKEWORD(picoData+4);

	// check an icon or a cursor
	if (IconHeader.idReserved==0 && (IconHeader.idType==1||IconHeader.idType==2))
		return (S_BYTE)IconHeader.idCount;
	else
		return 0;
}

SH_ICON hai_IcoDecode(S_VOID *pIcoData, S_BYTE frame)
{
	S_WORD i, j, c;
	S_DWORD WidthBytes;
	S_BYTE *picoData = pIcoData;
	S_BYTE *pDst, *pSrc;
	RGBQUAD *pPalette = S_NULL;
	_SP_ICON pIcon = S_NULL;
	S_ICONHEADER IconHeader;
	S_ICONDIRENTRY IconDir;
	BITMAPINFOHEADER bih;
	
	if (pIcoData == S_NULL)
		return S_NULL;

	IconHeader.idReserved = HAI_MAKEWORD(picoData+0);
	IconHeader.idType  = HAI_MAKEWORD(picoData+2);
	IconHeader.idCount = HAI_MAKEWORD(picoData+4);
	picoData += 6;

	// check an icon or a cursor
	if (!(IconHeader.idReserved==0 && (IconHeader.idType==1||IconHeader.idType==2)))
		return S_NULL;
	if (frame > IconHeader.idCount)
		return S_NULL;
	frame -= 1;
	picoData += frame*sizeof(S_ICONDIRENTRY);
	memcpy(&IconDir, picoData, sizeof(S_ICONDIRENTRY));

	picoData = (S_BYTE *)pIcoData + IconDir.dwImageOffset;
	//read bih;
	picoData += _hai_ReadBitmapInfo(picoData, &bih);
	//
	if (bih.biPlanes != 1 || bih.biCompression != BI_RGB)
		return S_NULL;

	// read the palette
	if (bih.biBitCount <= 8)
	{
		pPalette = hai_MemAlloc((1<<bih.biBitCount)*sizeof(RGBQUAD));
		if (pPalette == S_NULL)
			return S_NULL;
		memcpy(pPalette, picoData, (1<<bih.biBitCount)*sizeof(RGBQUAD));
	}
	
	WidthBytes = BITMAP_WIDTHBYTES(bih.biWidth, 24);
	pIcon = hai_MemAlloc(sizeof(*pIcon)+WidthBytes*bih.biWidth+MASKWIDTH(bih.biWidth)*bih.biWidth+4);
	if (pIcon == S_NULL)
		goto ICONDECODEERROR;

	HAI_SETHDLTYPE(pIcon, HT_ICON);
	pIcon->width     = (S_WORD)bih.biWidth;
	pIcon->height    = (S_WORD)bih.biWidth;
	pIcon->BitsPixel = 24;
	pIcon->WidthBytes = (S_WORD)WidthBytes;
	pIcon->lpImage = ((S_BYTE *)pIcon)+sizeof(*pIcon);
	pIcon->lpMask = ((S_BYTE *)pIcon)+sizeof(*pIcon)+WidthBytes*bih.biWidth;
	
	//read the icon
	picoData += (1<<bih.biBitCount)*sizeof(RGBQUAD);
	WidthBytes = BITMAP_WIDTHBYTES(bih.biWidth, bih.biBitCount);
	switch (bih.biBitCount)	{
		case 32 :
			for (i = 0; i < pIcon->height; i++)
			{
				pDst = ((S_BYTE*)pIcon->lpImage)+i*pIcon->WidthBytes;
				pSrc = picoData+(pIcon->height-i-1)*WidthBytes;
				j = (bih.biWidth+7)>>3;
				switch (bih.biWidth % 8)
				{
				case 0:do{*pDst++ = *pSrc++;*pDst++ = *pSrc++;*pDst++ = *pSrc++;pSrc++;
				case 7:     *pDst++ = *pSrc++;*pDst++ = *pSrc++;*pDst++ = *pSrc++;pSrc++;
				case 6:     *pDst++ = *pSrc++;*pDst++ = *pSrc++;*pDst++ = *pSrc++;pSrc++;
				case 5:     *pDst++ = *pSrc++;*pDst++ = *pSrc++;*pDst++ = *pSrc++;pSrc++;
				case 4:     *pDst++ = *pSrc++;*pDst++ = *pSrc++;*pDst++ = *pSrc++;pSrc++;
				case 3:     *pDst++ = *pSrc++;*pDst++ = *pSrc++;*pDst++ = *pSrc++;pSrc++;
				case 2:     *pDst++ = *pSrc++;*pDst++ = *pSrc++;*pDst++ = *pSrc++;pSrc++;
				case 1:     *pDst++ = *pSrc++;*pDst++ = *pSrc++;*pDst++ = *pSrc++;pSrc++;
						}while (--j);
				}
			}
			break;
		case 24 :
			memcpy(pIcon->lpImage, picoData, pIcon->WidthBytes*pIcon->height);
			break;
		case 16 :
			for (i = 0; i < pIcon->height; i++)
			{
				pDst = ((S_BYTE*)pIcon->lpImage)+i*pIcon->WidthBytes;
				pSrc = picoData+(pIcon->height-i-1)*WidthBytes;
				j = (bih.biWidth+7)>>3;
				switch (bih.biWidth % 8)
				{
				case 0:do{c=HAI_MAKEWORD(pSrc);pSrc+=2;*pDst++=(S_BYTE)((c&0x1F)<<3);*pDst++=(S_BYTE)((c&0x3E0)>>2);*pDst++=(S_BYTE)((c&0x7C00)>>7);
				case 7:     c=HAI_MAKEWORD(pSrc);pSrc+=2;*pDst++=(S_BYTE)((c&0x1F)<<3);*pDst++=(S_BYTE)((c&0x3E0)>>2);*pDst++=(S_BYTE)((c&0x7C00)>>7);
				case 6:     c=HAI_MAKEWORD(pSrc);pSrc+=2;*pDst++=(S_BYTE)((c&0x1F)<<3);*pDst++=(S_BYTE)((c&0x3E0)>>2);*pDst++=(S_BYTE)((c&0x7C00)>>7);
				case 5:     c=HAI_MAKEWORD(pSrc);pSrc+=2;*pDst++=(S_BYTE)((c&0x1F)<<3);*pDst++=(S_BYTE)((c&0x3E0)>>2);*pDst++=(S_BYTE)((c&0x7C00)>>7);
				case 4:     c=HAI_MAKEWORD(pSrc);pSrc+=2;*pDst++=(S_BYTE)((c&0x1F)<<3);*pDst++=(S_BYTE)((c&0x3E0)>>2);*pDst++=(S_BYTE)((c&0x7C00)>>7);
				case 3:     c=HAI_MAKEWORD(pSrc);pSrc+=2;*pDst++=(S_BYTE)((c&0x1F)<<3);*pDst++=(S_BYTE)((c&0x3E0)>>2);*pDst++=(S_BYTE)((c&0x7C00)>>7);
				case 2:     c=HAI_MAKEWORD(pSrc);pSrc+=2;*pDst++=(S_BYTE)((c&0x1F)<<3);*pDst++=(S_BYTE)((c&0x3E0)>>2);*pDst++=(S_BYTE)((c&0x7C00)>>7);
				case 1:     c=HAI_MAKEWORD(pSrc);pSrc+=2;*pDst++=(S_BYTE)((c&0x1F)<<3);*pDst++=(S_BYTE)((c&0x3E0)>>2);*pDst++=(S_BYTE)((c&0x7C00)>>7);
						}while (--j);
				}
			}
			break;
		case 8:
			for (i = 0; i < pIcon->height; i++)
			{
				pDst = ((S_BYTE*)pIcon->lpImage)+i*pIcon->WidthBytes;
				pSrc = picoData+(pIcon->height-i-1)*WidthBytes;
				j = (bih.biWidth+7)>>3;
				switch (bih.biWidth % 8)
				{
				case 0:do{c=*pSrc++;*pDst++=pPalette[c].rgbRed;*pDst++=pPalette[c].rgbGreen;*pDst++=pPalette[c].rgbBlue;
				case 7:     c=*pSrc++;*pDst++=pPalette[c].rgbRed;*pDst++=pPalette[c].rgbGreen;*pDst++=pPalette[c].rgbBlue;
				case 6:     c=*pSrc++;*pDst++=pPalette[c].rgbRed;*pDst++=pPalette[c].rgbGreen;*pDst++=pPalette[c].rgbBlue;
				case 5:     c=*pSrc++;*pDst++=pPalette[c].rgbRed;*pDst++=pPalette[c].rgbGreen;*pDst++=pPalette[c].rgbBlue;
				case 4:     c=*pSrc++;*pDst++=pPalette[c].rgbRed;*pDst++=pPalette[c].rgbGreen;*pDst++=pPalette[c].rgbBlue;
				case 3:     c=*pSrc++;*pDst++=pPalette[c].rgbRed;*pDst++=pPalette[c].rgbGreen;*pDst++=pPalette[c].rgbBlue;
				case 2:     c=*pSrc++;*pDst++=pPalette[c].rgbRed;*pDst++=pPalette[c].rgbGreen;*pDst++=pPalette[c].rgbBlue;
				case 1:     c=*pSrc++;*pDst++=pPalette[c].rgbRed;*pDst++=pPalette[c].rgbGreen;*pDst++=pPalette[c].rgbBlue;
						}while (--j);
				}
			}
			break;
		case 4:
			for (i = 0; i < pIcon->height; i++)
			{
				pDst = ((S_BYTE*)pIcon->lpImage)+i*pIcon->WidthBytes;
				pSrc = picoData+(pIcon->height-i-1)*WidthBytes;
				j = (S_WORD)bih.biWidth;
				while (j >= 2){
					j -= 2;
					c = *pSrc++;
					*pDst++ = pPalette[(c>>4)&0X0F].rgbRed;
					*pDst++ = pPalette[(c>>4)&0X0F].rgbGreen;
					*pDst++ = pPalette[(c>>4)&0X0F].rgbBlue;
					*pDst++ = pPalette[c&0X0F].rgbRed;
					*pDst++ = pPalette[c&0X0F].rgbGreen;
					*pDst++ = pPalette[c&0X0F].rgbBlue;
				}
				if (j == 1){
					c = *pSrc++;
					*pDst++ = pPalette[(c>>4)&0X0F].rgbRed;
					*pDst++ = pPalette[(c>>4)&0X0F].rgbGreen;
					*pDst++ = pPalette[(c>>4)&0X0F].rgbBlue;
				}
			}
			break;
		case 1:
			for (i = 0; i < pIcon->height; i++)
			{
				pDst = ((S_BYTE*)pIcon->lpImage)+i*pIcon->WidthBytes;
				pSrc = picoData+(pIcon->height-i-1)*WidthBytes;
				j = (S_WORD)bih.biWidth;
				while (j >= 8){
					j -= 8;
					c = *pSrc++;
					*pDst++ = pPalette[(c>>7)&0X01].rgbRed;*pDst++ = pPalette[(c>>7)&0X01].rgbGreen;*pDst++ = pPalette[(c>>7)&0X01].rgbBlue;
					*pDst++ = pPalette[(c>>6)&0X01].rgbRed;*pDst++ = pPalette[(c>>6)&0X01].rgbGreen;*pDst++ = pPalette[(c>>6)&0X01].rgbBlue;
					*pDst++ = pPalette[(c>>5)&0X01].rgbRed;*pDst++ = pPalette[(c>>5)&0X01].rgbGreen;*pDst++ = pPalette[(c>>5)&0X01].rgbBlue;
					*pDst++ = pPalette[(c>>4)&0X01].rgbRed;*pDst++ = pPalette[(c>>4)&0X01].rgbGreen;*pDst++ = pPalette[(c>>4)&0X01].rgbBlue;
					*pDst++ = pPalette[(c>>3)&0X01].rgbRed;*pDst++ = pPalette[(c>>3)&0X01].rgbGreen;*pDst++ = pPalette[(c>>3)&0X01].rgbBlue;
					*pDst++ = pPalette[(c>>2)&0X01].rgbRed;*pDst++ = pPalette[(c>>2)&0X01].rgbGreen;*pDst++ = pPalette[(c>>2)&0X01].rgbBlue;
					*pDst++ = pPalette[(c>>1)&0X01].rgbRed;*pDst++ = pPalette[(c>>1)&0X01].rgbGreen;*pDst++ = pPalette[(c>>1)&0X01].rgbBlue;
					*pDst++ = pPalette[(c>>0)&0X01].rgbRed;*pDst++ = pPalette[(c>>0)&0X01].rgbGreen;*pDst++ = pPalette[(c>>0)&0X01].rgbBlue;
				}
				if (j)
				{
					S_SHORT w = (S_SHORT)j;
					c = *pSrc++;
					{*pDst++ = pPalette[(c>>7)&0X01].rgbRed;*pDst++ = pPalette[(c>>7)&0X01].rgbGreen;*pDst++ = pPalette[(c>>7)&0X01].rgbBlue;}
					if (--w > 0)
						{*pDst++ = pPalette[(c>>6)&0X01].rgbRed;*pDst++ = pPalette[(c>>6)&0X01].rgbGreen;*pDst++ = pPalette[(c>>6)&0X01].rgbBlue;}
					if (--w > 0)
						{*pDst++ = pPalette[(c>>5)&0X01].rgbRed;*pDst++ = pPalette[(c>>5)&0X01].rgbGreen;*pDst++ = pPalette[(c>>5)&0X01].rgbBlue;}
					if (--w > 0)
						{*pDst++ = pPalette[(c>>4)&0X01].rgbRed;*pDst++ = pPalette[(c>>4)&0X01].rgbGreen;*pDst++ = pPalette[(c>>4)&0X01].rgbBlue;}
					if (--w > 0)
						{*pDst++ = pPalette[(c>>3)&0X01].rgbRed;*pDst++ = pPalette[(c>>3)&0X01].rgbGreen;*pDst++ = pPalette[(c>>3)&0X01].rgbBlue;}
					if (--w > 0)
						{*pDst++ = pPalette[(c>>2)&0X01].rgbRed;*pDst++ = pPalette[(c>>2)&0X01].rgbGreen;*pDst++ = pPalette[(c>>2)&0X01].rgbBlue;}
					if (--w > 0)
						{*pDst++ = pPalette[(c>>1)&0X01].rgbRed;*pDst++ = pPalette[(c>>1)&0X01].rgbGreen;*pDst++ = pPalette[(c>>1)&0X01].rgbBlue;}
					if (--w > 0)
						{*pDst++ = pPalette[(c>>0)&0X01].rgbRed;*pDst++ = pPalette[(c>>0)&0X01].rgbGreen;*pDst++ = pPalette[(c>>0)&0X01].rgbBlue;}
				}
			}
			break;
		default:
			goto ICONDECODEERROR;
	}

	// read mask
	picoData += WidthBytes*pIcon->height;
	WidthBytes = MASKWIDTH(pIcon->width);
	for (i = 0; i < pIcon->height; i++)
	{
		pDst = ((S_BYTE*)pIcon->lpMask)+i*WidthBytes;
		pSrc = picoData+(pIcon->height-i-1)*WidthBytes;
		memcpy(pDst, pSrc, WidthBytes);
	}

	if (pPalette)
		hai_MemFree(pPalette);
	return (SH_ICON)pIcon;
ICONDECODEERROR:
	if (pPalette)
		hai_MemFree(pPalette);
	if (pIcon)
		hai_MemFree(pIcon);
	return S_NULL;
}



