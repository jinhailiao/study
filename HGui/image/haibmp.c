//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haibmp.h
// Description:		BMP decode; reference CxImage
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 2.0.0		2006-09-06  create                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------
#include "haitype.h"
#include "haihandle.h"
#include "haimem.h"
#include "_haigdi.h"
#include "haipic.h"

typedef struct tagBITMAPFILEHEADER {
	S_WORD    bfType;
	S_DWORD   bfSize;
	S_WORD    bfReserved1;
	S_WORD    bfReserved2;
	S_DWORD   bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPCOREHEADER {
	S_DWORD   bcSize;
	S_WORD    bcWidth;
	S_WORD    bcHeight;
	S_WORD    bcPlanes;
	S_WORD    bcBitCount;
} BITMAPCOREHEADER;



#define BMP_WIDTH_LIMIT		2000
#define BMP_HEIGHT_LIMIT	2000

#define RLE_COMMAND			0
#define RLE_ENDOFLINE		0
#define RLE_ENDOFBITMAP		1
#define RLE_DELTA			2


#define WIDTHBYTES(i)           ((S_DWORD)(((i)+31)&(~31))>>3)  /* ULONG aligned ! */
#define DibWidthBytesN(lpbi, n) (S_UINT)WIDTHBYTES((S_UINT)(lpbi)->biWidth * (S_UINT)(n))
#define DibWidthBytes(lpbi)     DibWidthBytesN(lpbi, (lpbi)->biBitCount)
#define DibSizeImage(lpbi)      ((lpbi)->biSizeImage == 0 \
                                    ? ((S_DWORD)(S_UINT)DibWidthBytes(lpbi) * (S_DWORD)(S_UINT)(lpbi)->biHeight) \
                                    : (lpbi)->biSizeImage)
#define DibNumColors(lpbi)      ((lpbi)->biClrUsed == 0 && (lpbi)->biBitCount <= 8 \
                                    ? (int)(1 << (int)(lpbi)->biBitCount)          \
                                    : (int)(lpbi)->biClrUsed)
#define FixBitmapInfo(lpbi)     if ((lpbi)->biSizeImage == 0)                 \
												(lpbi)->biSizeImage = DibSizeImage(lpbi); \
                                if ((lpbi)->biClrUsed == 0)                   \
                                    (lpbi)->biClrUsed = DibNumColors(lpbi);   \


S_DWORD _hai_ReadBitmapFileHeader(S_BYTE *pBmpData, BITMAPFILEHEADER *pbf);
S_DWORD _hai_ReadBitmapInfo(S_BYTE *pBmpData, BITMAPINFOHEADER *pbih);
S_WORD _hai_ExtractBits841BI_RGB(_SP_BITMAP pBmp, BITMAPINFOHEADER *pbih, S_BYTE *pBmpData, RGBQUAD *pPalette, S_BOOL bTopDownDib);
S_WORD _hai_ExtractBits841BI_RLE4(_SP_BITMAP pBmp, S_BYTE *pBmpData, RGBQUAD *pPalette, S_BOOL bTopDownDib);
S_WORD _hai_ExtractBits841BI_RLE8(_SP_BITMAP pBmp, S_BYTE *pBmpData, RGBQUAD *pPalette, S_BOOL bTopDownDib);

SH_BITMAP hai_BmpDecode(S_VOID *pBmpData)
{
	S_BOOL bIsOldBmp;
	S_BOOL bTopDownDib;
	S_WORD i, j, ColorNum;
	S_DWORD SkipBytes, WidthBytes;
	S_BYTE *pBmpDat = (S_BYTE *)pBmpData;
	S_BYTE *pDst;
	S_BYTE *pSrc;
	RGBQUAD *pPalette;
	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;
	_SP_BITMAP pBitmap;
	
	if (pBmpData == S_NULL)
		return S_NULL;

	SkipBytes = _hai_ReadBitmapFileHeader(pBmpDat, &bf);
	if (SkipBytes == 0)
		return S_NULL;
	pBmpDat += SkipBytes;
	SkipBytes = _hai_ReadBitmapInfo(pBmpDat, &bi);
	if (SkipBytes == 0)
		return S_NULL;
	if (bi.biWidth > BMP_WIDTH_LIMIT || bi.biHeight > BMP_HEIGHT_LIMIT)
		return S_NULL;

	pBmpDat += SkipBytes;

	bIsOldBmp = (bi.biSize == 0x0C);//sizeof(BITMAPCOREHEADER);
	bTopDownDib = (bi.biHeight < 0);
	if (bTopDownDib) 
		bi.biHeight = -bi.biHeight;

	if (ColorNum = DibNumColors(&bi))
	{
		pPalette = hai_MemAlloc(ColorNum*sizeof(RGBQUAD));
		if (pPalette == S_NULL)
			return S_NULL;
		if (bIsOldBmp){ // convert a old color table (3 byte entries) to a new color table (4 byte entries)
			for (i = 0; i < ColorNum; i++){
				pPalette[i].rgbRed      = *pBmpDat++;
				pPalette[i].rgbGreen    = *pBmpDat++;
				pPalette[i].rgbBlue     = *pBmpDat++;
				pPalette[i].rgbReserved = (S_BYTE)0;
			}
		} else {
			for (i = 0; i < ColorNum; i++){
				pPalette[i].rgbRed      = *pBmpDat++;
				pPalette[i].rgbGreen    = *pBmpDat++;
				pPalette[i].rgbBlue     = *pBmpDat++;
				pPalette[i].rgbReserved = *pBmpDat++;
			}
			//force rgbReserved=0, to avoid problems with some WinXp bitmaps
			for (i = 0; i < bi.biClrUsed; i++)
				pPalette[i].rgbReserved=0;
		}
	}
	else
		pPalette = S_NULL;

	WidthBytes = BITMAP_WIDTHBYTES(bi.biWidth, 24);
	pBitmap = hai_MemAlloc(sizeof(*pBitmap)+WidthBytes*bi.biHeight+4);
	if (pBitmap == S_NULL)
		goto BMPDECODEERROR;

	HAI_SETHDLTYPE(pBitmap, HT_BITMAP);
	pBitmap->width     = (S_WORD)bi.biWidth;
	pBitmap->height    = (S_WORD)bi.biHeight;
	pBitmap->BitsPixel = 24;
	pBitmap->WidthBytes = (S_WORD)WidthBytes;
	pBitmap->lpImage = ((S_BYTE *)pBitmap)+sizeof(*pBitmap);
	
	pBmpDat = (S_BYTE*)pBmpData + bf.bfOffBits;
	WidthBytes = BITMAP_WIDTHBYTES(bi.biWidth, bi.biBitCount);
	switch (bi.biBitCount) {
		case 32 :
			if (bi.biCompression == BI_BITFIELDS || bi.biCompression == BI_RGB){
				for (i = 0; i < bi.biHeight; i++)
				{
					pDst = ((S_BYTE*)pBitmap->lpImage)+i*pBitmap->WidthBytes;
					if (bTopDownDib)
						pSrc = pBmpDat+i*WidthBytes;
					else
						pSrc = pBmpDat+(bi.biHeight-i-1)*WidthBytes;
					j = (bi.biWidth+7)>>3;
					switch (bi.biWidth % 8)
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
			}
			else
				goto BMPDECODEERROR;
			break;
		case 24 :
			if (bi.biCompression == BI_RGB){
				if (bTopDownDib)
					memcpy(pBitmap->lpImage, pBmpDat, pBitmap->WidthBytes*bi.biHeight);
				else
				{
					pDst = pBitmap->lpImage;
					pSrc = pBmpDat+(bi.biHeight-1)*pBitmap->WidthBytes;
					for (i = 0; i < bi.biHeight; i++)
					{
						memcpy(pDst, pSrc, pBitmap->WidthBytes);
						pDst += pBitmap->WidthBytes;
						pSrc -= pBitmap->WidthBytes;
					}
				}
			}
			else
				goto BMPDECODEERROR;
			break;
		case 16 :
		{
			S_WORD rm, gm, bm;
			S_WORD c, rs=0, gs=0, bs=0;
			if (bi.biCompression == BI_BITFIELDS)
			{
				pDst = (S_BYTE*)pBmpData+14/*sizeof(BITMAPFILEHEADER)*/+SkipBytes;
				rm=(S_WORD)HAI_MAKEDWORD(pDst);
				gm=(S_WORD)HAI_MAKEDWORD(pDst+4);
				bm=(S_WORD)HAI_MAKEDWORD(pDst+8);
			} else {
				rm=0x1F; gm=0x3E0; bm=0x7C00; //RGB555
			}
			for (i=0;i<16;i++){
				if ((rm>>i)&0x01) rs++;
				if ((gm>>i)&0x01) gs++;
				if ((bm>>i)&0x01) bs++;
			}
			gs+=rs; bs+=gs; rs=8-rs; gs-=8; bs-=8;
			for (i = 0; i < bi.biHeight; i++)
			{
				pDst = ((S_BYTE*)pBitmap->lpImage)+i*pBitmap->WidthBytes;
				if (bTopDownDib)
					pSrc = pBmpDat+i*WidthBytes;
				else
					pSrc = pBmpDat+(bi.biHeight-i-1)*WidthBytes;
				j = (bi.biWidth+7)>>3;
				switch (bi.biWidth % 8)
				{
				case 0:do{c=HAI_MAKEWORD(pSrc);pSrc+=2;*pDst++=(S_BYTE)((c & rm)<<rs);*pDst++=(S_BYTE)((c & gm)>>gs);*pDst++=(S_BYTE)((c & bm)>>bs);
				case 7:     c=HAI_MAKEWORD(pSrc);pSrc+=2;*pDst++=(S_BYTE)((c & rm)<<rs);*pDst++=(S_BYTE)((c & gm)>>gs);*pDst++=(S_BYTE)((c & bm)>>bs);
				case 6:     c=HAI_MAKEWORD(pSrc);pSrc+=2;*pDst++=(S_BYTE)((c & rm)<<rs);*pDst++=(S_BYTE)((c & gm)>>gs);*pDst++=(S_BYTE)((c & bm)>>bs);
				case 5:     c=HAI_MAKEWORD(pSrc);pSrc+=2;*pDst++=(S_BYTE)((c & rm)<<rs);*pDst++=(S_BYTE)((c & gm)>>gs);*pDst++=(S_BYTE)((c & bm)>>bs);
				case 4:     c=HAI_MAKEWORD(pSrc);pSrc+=2;*pDst++=(S_BYTE)((c & rm)<<rs);*pDst++=(S_BYTE)((c & gm)>>gs);*pDst++=(S_BYTE)((c & bm)>>bs);
				case 3:     c=HAI_MAKEWORD(pSrc);pSrc+=2;*pDst++=(S_BYTE)((c & rm)<<rs);*pDst++=(S_BYTE)((c & gm)>>gs);*pDst++=(S_BYTE)((c & bm)>>bs);
				case 2:     c=HAI_MAKEWORD(pSrc);pSrc+=2;*pDst++=(S_BYTE)((c & rm)<<rs);*pDst++=(S_BYTE)((c & gm)>>gs);*pDst++=(S_BYTE)((c & bm)>>bs);
				case 1:     c=HAI_MAKEWORD(pSrc);pSrc+=2;*pDst++=(S_BYTE)((c & rm)<<rs);*pDst++=(S_BYTE)((c & gm)>>gs);*pDst++=(S_BYTE)((c & bm)>>bs);
						}while (--j);
				}
			}
			break;
		}
		case 8 :
		case 4 :
		case 1 :
		switch (bi.biCompression) 
		{
			case BI_RGB :
				_hai_ExtractBits841BI_RGB(pBitmap, &bi, pBmpDat, pPalette, bTopDownDib);
				break;
			case BI_RLE4 :
				_hai_ExtractBits841BI_RLE4(pBitmap, pBmpDat, pPalette, bTopDownDib);
				break;
			case BI_RLE8 :
				_hai_ExtractBits841BI_RLE8(pBitmap, pBmpDat, pPalette, bTopDownDib);
				break;
			default :								
				goto BMPDECODEERROR;
		}
	}

	if (pPalette)
		hai_MemFree(pPalette);
	return (SH_BITMAP)pBitmap;
BMPDECODEERROR:
	if (pPalette)
		hai_MemFree(pPalette);
	if (pBitmap)
		hai_MemFree(pBitmap);
	return S_NULL;
}

S_DWORD _hai_ReadBitmapFileHeader(S_BYTE *pBmpData, BITMAPFILEHEADER *pbf)
{
	if (pBmpData==S_NULL || pbf==S_NULL)
		return 0;
	
	pbf->bfType      = HAI_MAKEWORD(pBmpData+0); 
	pbf->bfSize      = HAI_MAKEDWORD(pBmpData+2); 
	pbf->bfReserved1 = HAI_MAKEWORD(pBmpData+6); 
	pbf->bfReserved2 = HAI_MAKEWORD(pBmpData+8); 
	pbf->bfOffBits   = HAI_MAKEDWORD(pBmpData+10); 

	if (pbf->bfType == 0x4D42) // "BM"
		return 14;
	return 0;	
}

S_DWORD _hai_ReadBitmapInfo(S_BYTE *pBmpData, BITMAPINFOHEADER *pbih)
{
	BITMAPCOREHEADER bc;
	
	if (pBmpData==S_NULL || pbih==S_NULL)
		return 0;
	
	pbih->biSize = HAI_MAKEDWORD(pBmpData);
	switch (pbih->biSize)
	{
	case 0x40: //sizeof(OS2_BMP_HEADER):
	case 0x28: //sizeof(BITMAPINFOHEADER):
		pbih->biSize          = HAI_MAKEDWORD(pBmpData+0);
		pbih->biWidth         = HAI_MAKEDWORD(pBmpData+4);
		pbih->biHeight        = HAI_MAKEDWORD(pBmpData+8);
		pbih->biPlanes        = HAI_MAKEWORD(pBmpData+12);
		pbih->biBitCount      = HAI_MAKEWORD(pBmpData+14);
		pbih->biCompression   = HAI_MAKEDWORD(pBmpData+16);
		pbih->biSizeImage     = HAI_MAKEDWORD(pBmpData+20);
		pbih->biXPelsPerMeter = HAI_MAKEDWORD(pBmpData+24);
		pbih->biYPelsPerMeter = HAI_MAKEDWORD(pBmpData+28);
		pbih->biClrUsed       = HAI_MAKEDWORD(pBmpData+32);
		pbih->biClrImportant  = HAI_MAKEDWORD(pBmpData+36);
		break;
		
	case 0x0C: //sizeof(BITMAPCOREHEADER):
		bc.bcSize             = HAI_MAKEDWORD(pBmpData+0);
		bc.bcWidth            = HAI_MAKEWORD(pBmpData+4);
		bc.bcHeight           = HAI_MAKEWORD(pBmpData+6);
		bc.bcPlanes           = HAI_MAKEWORD(pBmpData+8);
		bc.bcBitCount         = HAI_MAKEWORD(pBmpData+10);
		pbih->biSize          = bc.bcSize;
		pbih->biWidth         = (S_DWORD)bc.bcWidth;
		pbih->biHeight        = (S_DWORD)bc.bcHeight;
		pbih->biPlanes        = bc.bcPlanes;
		pbih->biBitCount      = bc.bcBitCount;
		pbih->biCompression   = BI_RGB;
		pbih->biSizeImage     = 0;
		pbih->biXPelsPerMeter = 0;
		pbih->biYPelsPerMeter = 0;
		pbih->biClrUsed       = 0;
		pbih->biClrImportant  = 0;
		break;
	default:
		pbih->biSize = 0;
		break;
	}

	FixBitmapInfo(pbih);
	return pbih->biSize;
}

S_WORD _hai_ExtractBits841BI_RGB(_SP_BITMAP pBmp, BITMAPINFOHEADER *pbih, S_BYTE *pBmpData, RGBQUAD *pPalette, S_BOOL bTopDownDib)
{
	S_BYTE c, *pDst, *pSrc;
	S_WORD i, j;
	S_WORD WidthBytes = BITMAP_WIDTHBYTES(pbih->biWidth, pbih->biBitCount);

	switch (pbih->biBitCount)
	{
	case 8:
		for (i = 0; i < pbih->biHeight; i++)
		{
			pDst = ((S_BYTE*)pBmp->lpImage)+i*pBmp->WidthBytes;
			if (bTopDownDib)
				pSrc = pBmpData+i*WidthBytes;
			else
				pSrc = pBmpData+(pbih->biHeight-i-1)*WidthBytes;
			j = (pbih->biWidth+7)>>3;
			switch (pbih->biWidth % 8)
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
		for (i = 0; i < pbih->biHeight; i++)
		{
			pDst = ((S_BYTE*)pBmp->lpImage)+i*pBmp->WidthBytes;
			if (bTopDownDib)
				pSrc = pBmpData+i*WidthBytes;
			else
				pSrc = pBmpData+(pbih->biHeight-i-1)*WidthBytes;
			j = (S_WORD)pbih->biWidth;
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
		for (i = 0; i < pbih->biHeight; i++)
		{
			pDst = ((S_BYTE*)pBmp->lpImage)+i*pBmp->WidthBytes;
			if (bTopDownDib)
				pSrc = pBmpData+i*WidthBytes;
			else
				pSrc = pBmpData+(pbih->biHeight-i-1)*WidthBytes;
			j = (S_WORD)pbih->biWidth;
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
		return S_FALSE;
		break;
	}
	return S_TRUE;
}

S_WORD _hai_ExtractBits841BI_RLE4(_SP_BITMAP pBmp, S_BYTE *pBmpData, RGBQUAD *pPalette, S_BOOL bTopDownDib)
{
	S_BOOL bContinue = S_TRUE;
	S_BYTE StatusByte = 0;
	S_BYTE SecondByte = 0;
	S_WORD i, bits = 0;
	S_DWORD ScanLine = 0;
	S_BYTE *pDst;

	while (bContinue == S_TRUE)
	{
		StatusByte = *pBmpData++;
		switch (StatusByte) {
			case RLE_COMMAND :
				StatusByte = *pBmpData++;
				switch (StatusByte) {
					case RLE_ENDOFLINE :
						bits = 0;
						ScanLine++;
						break;
					case RLE_ENDOFBITMAP :
						bContinue = S_FALSE;
						break;
					case RLE_DELTA :
						bits       += *pBmpData++;
						ScanLine+= *pBmpData++;
						break;
					default :
						if (bTopDownDib)
							pDst = (S_BYTE*)pBmp->lpImage + ScanLine*pBmp->WidthBytes + bits*3;
						else
							pDst = (S_BYTE*)pBmp->lpImage + (pBmp->height-ScanLine-1)*pBmp->WidthBytes + bits*3;
						bits += StatusByte;
						for (i = 0; i < StatusByte; i++, pBmpData++) {
							*pDst++ = pPalette[*pBmpData>>4].rgbRed;
							*pDst++ = pPalette[*pBmpData>>4].rgbGreen;
							*pDst++ = pPalette[*pBmpData>>4].rgbBlue;
							if (++i >= StatusByte)
							{
								pBmpData++;
								break;
							}
							*pDst++ = pPalette[*pBmpData&0xF].rgbRed;
							*pDst++ = pPalette[*pBmpData&0xF].rgbGreen;
							*pDst++ = pPalette[*pBmpData&0xF].rgbBlue;
						}
						// in absolute mode, each run must be aligned on a word boundary. 
						if (((StatusByte+1)>>1)&1)
							pBmpData++;												
						break;
				};
				break;
			default :
				if (bTopDownDib)
					pDst = (S_BYTE*)pBmp->lpImage + ScanLine*pBmp->WidthBytes + bits*3;
				else
					pDst = (S_BYTE*)pBmp->lpImage + (pBmp->height-ScanLine-1)*pBmp->WidthBytes + bits*3;
				SecondByte = *pBmpData++;
				bits += StatusByte;
				for (i = 0; i < StatusByte; i++) {
					*pDst++ = pPalette[SecondByte>>4].rgbRed;
					*pDst++ = pPalette[SecondByte>>4].rgbGreen;
					*pDst++ = pPalette[SecondByte>>4].rgbBlue;
					
					if (++i >= StatusByte)
					{
						pBmpData++;
						break;
					}
					*pDst++ = pPalette[SecondByte&0x0F].rgbRed;
					*pDst++ = pPalette[SecondByte&0x0F].rgbGreen;
					*pDst++ = pPalette[SecondByte&0x0F].rgbBlue;
				}
				break;
		};
	}
	return S_TRUE;
}

S_WORD _hai_ExtractBits841BI_RLE8(_SP_BITMAP pBmp, S_BYTE *pBmpData, RGBQUAD *pPalette, S_BOOL bTopDownDib)
{
	S_BOOL bContinue = S_TRUE;
	S_BYTE StatusByte = 0;
	S_BYTE SecondByte = 0;
	S_WORD i, bits = 0;
	S_DWORD ScanLine = 0;
	S_BYTE *pDst = (S_BYTE*)pBmp->lpImage;

	while (bContinue == S_TRUE)
	{
		StatusByte = *pBmpData++;
		switch (StatusByte) {
			case RLE_COMMAND :
				StatusByte = *pBmpData++;
				switch (StatusByte) {
					case RLE_ENDOFLINE :
						bits = 0;
						ScanLine++;
						break;
					case RLE_ENDOFBITMAP :
						bContinue=S_FALSE;
						break;
					case RLE_DELTA :
						bits       += *pBmpData++;
						ScanLine += *pBmpData++;
						break;
					default :
						if (bTopDownDib)
							pDst = (S_BYTE*)pBmp->lpImage + pBmp->WidthBytes*ScanLine + bits*3;
						else
							pDst = (S_BYTE*)pBmp->lpImage + pBmp->WidthBytes*(pBmp->height-ScanLine-1) + bits*3;
						for (i = 0; i < StatusByte; i++,pBmpData++)
						{
							*pDst++ = pPalette[*pBmpData].rgbRed;
							*pDst++ = pPalette[*pBmpData].rgbGreen;
							*pDst++ = pPalette[*pBmpData].rgbBlue;
						}
						// in absolute mode, each run must be aligned on a word boundary. 
						if ((StatusByte & 1) == 1)
							pBmpData++;												
						bits += StatusByte;													
						break;								
				};
				break;
			default :
				if (bTopDownDib)
					pDst = (S_BYTE*)pBmp->lpImage + pBmp->WidthBytes*ScanLine + bits*3;
				else
					pDst = (S_BYTE*)pBmp->lpImage + pBmp->WidthBytes*(pBmp->height-ScanLine-1) + bits*3;
				SecondByte = *pBmpData++;
				bits += StatusByte;
				for (i = 0; i < StatusByte; i++) {
					*pDst++ = pPalette[SecondByte].rgbRed;
					*pDst++ = pPalette[SecondByte].rgbGreen;
					*pDst++ = pPalette[SecondByte].rgbBlue;
				}
				break;
		};
	}
	return S_TRUE;
}


