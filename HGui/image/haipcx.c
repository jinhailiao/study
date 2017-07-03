//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haibmp.h
// Description:		
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

typedef struct tagPCXHEADER
{
  S_CHAR Manufacturer;		// always 0X0A
  S_CHAR Version;			// version number
  S_CHAR Encoding;			// always 1
  S_CHAR BitsPerPixel;		// color bits
  S_WORD Xmin, Ymin;		// image origin
  S_WORD Xmax, Ymax;		// image dimensions
  S_WORD Hres, Vres;		// resolution values
  S_BYTE ColorMap[16][3];	// color palette
  S_CHAR Reserved;
  S_CHAR ColorPlanes;		// color planes
  S_WORD BytesPerLine;		// line buffer size
  S_WORD PaletteType;		// grey or color palette
  S_CHAR Filter[58];
} PCXHEADER;

#define PCX_MAGIC 0X0A			// PCX magic number
#define PCX_256_COLORS 0X0C		// magic number for 256 colors
#define PCX_HDR_SIZE 128		// size of PCX header
#define PCX_MAXCOLORS 256
#define PCX_MAXPLANES 4
#define PCX_MAXVAL 255

S_WORD _hai_PcxReadHeader(S_BYTE *pPcxData, PCXHEADER *pph);
S_BOOL _hai_PcxDeal3x8(S_BYTE *pPixels, S_BYTE *pBitplanes, S_WORD BytesPerLine, S_WORD Width);
S_BOOL _hai_PcxPlanesToPixels(S_BYTE * pixels, S_BYTE * bitplanes, S_WORD Width, S_WORD bytesperline, S_WORD planes, S_WORD bitsperpixel, S_BYTE pPalette[][3]);
S_BOOL _hsi_PcxUnpackPixels(S_BYTE * pixels, S_BYTE * bitplanes, S_WORD Width, S_WORD planes, S_WORD bitsperpixel, S_BYTE pPalette[][3]);


SH_BITMAP hai_PcxDecode(S_VOID *pPcxData)
{
	S_BYTE c, ColorMap[PCX_MAXCOLORS][3];
	S_INT count;
	S_WORD x, y;
	S_DWORD nbytes, Height, Width;
	S_BYTE *pPcx;
	S_BYTE *pcximage = S_NULL;
	S_BYTE *pPcxPixels, *pBitplanes;
 	PCXHEADER pcxHeader;
	_SP_BITMAP pBitmap = S_NULL;

	if (pPcxData == S_NULL)
		return S_NULL;
	
	_hai_PcxReadHeader(pPcxData, &pcxHeader);
	if (pcxHeader.Manufacturer != PCX_MAGIC) // pcx flag
		return S_NULL;
    if (pcxHeader.Encoding != 1)// 1 is RLE encode
		return S_NULL;
    if (pcxHeader.ColorPlanes > 4)
		return S_NULL;

    Width  = (pcxHeader.Xmax - pcxHeader.Xmin) + 1;
    Height = (pcxHeader.Ymax - pcxHeader.Ymin) + 1;

    nbytes     = pcxHeader.BytesPerLine * pcxHeader.ColorPlanes * Height;
    pPcxPixels = pcximage = (S_BYTE*)hai_MemAlloc(nbytes);
	pPcx = (S_BYTE*)pPcxData + PCX_HDR_SIZE;
    while (nbytes > 0){
		c = *pPcx++;
		if ((c & 0XC0) != 0XC0){ // Repeated group
			*pPcxPixels++ = c;
			--nbytes;
		}
		else {
			count = c & 0X3F; // extract count
			c = *pPcx++;
			nbytes -= count;
			while (--count >=0) 
				*pPcxPixels++ = c;
		}
	}

    for (x = 0; x < 16; x++){
		ColorMap[x][0] = pcxHeader.ColorMap[x][2];
		ColorMap[x][1] = pcxHeader.ColorMap[x][1];
		ColorMap[x][2] = pcxHeader.ColorMap[x][0];
	}
    if (pcxHeader.BitsPerPixel == 8 && pcxHeader.ColorPlanes == 1){
		c = *pPcx++;
		if (c != PCX_256_COLORS)
			goto PCXDECODEERROR;
		for (x = 0; x < PCX_MAXCOLORS; x++){
			ColorMap[x][2] = *pPcx++;
			ColorMap[x][1] = *pPcx++;
			ColorMap[x][0] = *pPcx++;
		}
	}
    if (pcxHeader.BitsPerPixel == 1 && pcxHeader.ColorPlanes == 1){
		ColorMap[0][0] = ColorMap[0][1] = ColorMap[0][2] = 0;
		ColorMap[1][0] = ColorMap[1][1] = ColorMap[1][2] = 255;
	}

	nbytes = BITMAP_WIDTHBYTES(Width, 24);
	pBitmap = hai_MemAlloc(sizeof(*pBitmap)+nbytes*Height+4);
	if (pBitmap == S_NULL)
		goto PCXDECODEERROR;

	HAI_SETHDLTYPE(pBitmap, HT_BITMAP);
	pBitmap->width     = (S_WORD)Width;
	pBitmap->height    = (S_WORD)Height;
	pBitmap->BitsPixel = 24;
	pBitmap->WidthBytes = (S_WORD)nbytes;
	pBitmap->lpImage = ((S_BYTE *)pBitmap)+sizeof(*pBitmap);

    for (y = 0; y < Height; y++){
		pPcxPixels = (S_BYTE*)pBitmap->lpImage+y*pBitmap->WidthBytes;
		pBitplanes = pcximage + (y * pcxHeader.BytesPerLine * pcxHeader.ColorPlanes);

		if (pcxHeader.ColorPlanes == 3 && pcxHeader.BitsPerPixel == 8)
			_hai_PcxDeal3x8(pPcxPixels, pBitplanes, pcxHeader.BytesPerLine, (S_WORD)Width);
		else if (pcxHeader.ColorPlanes == 1)
			_hsi_PcxUnpackPixels(pPcxPixels, pBitplanes, (S_WORD)Width, pcxHeader.ColorPlanes, pcxHeader.BitsPerPixel, ColorMap);
		else
			_hai_PcxPlanesToPixels(pPcxPixels, pBitplanes, (S_WORD)Width, pcxHeader.BytesPerLine, pcxHeader.ColorPlanes, pcxHeader.BitsPerPixel, ColorMap);
	}

	if (pcximage)
		hai_MemFree(pcximage);
	return (SH_BITMAP)pBitmap;

PCXDECODEERROR:
	if (pcximage)
		hai_MemFree(pcximage);
	if (pBitmap)
		hai_MemFree(pBitmap);
	return S_NULL;
}

S_BOOL _hai_PcxDeal3x8(S_BYTE *pPcxPixels, S_BYTE *pBitplanes, S_WORD BytesPerLine, S_WORD Width)
{
	S_WORD i = (Width+7)>>3;
	S_BYTE *pB = pBitplanes;
	S_BYTE *pG = pBitplanes+BytesPerLine;
	S_BYTE *pR = pBitplanes+BytesPerLine+BytesPerLine;

	switch (Width%8)
	{
	case 0:do{*pPcxPixels++ = *pR++; *pPcxPixels++ = *pG++; *pPcxPixels++ = *pB++;
	case 7:     *pPcxPixels++ = *pR++; *pPcxPixels++ = *pG++; *pPcxPixels++ = *pB++;
	case 6:     *pPcxPixels++ = *pR++; *pPcxPixels++ = *pG++; *pPcxPixels++ = *pB++;
	case 5:     *pPcxPixels++ = *pR++; *pPcxPixels++ = *pG++; *pPcxPixels++ = *pB++;
	case 4:     *pPcxPixels++ = *pR++; *pPcxPixels++ = *pG++; *pPcxPixels++ = *pB++;
	case 3:     *pPcxPixels++ = *pR++; *pPcxPixels++ = *pG++; *pPcxPixels++ = *pB++;
	case 2:     *pPcxPixels++ = *pR++; *pPcxPixels++ = *pG++; *pPcxPixels++ = *pB++;
	case 1:     *pPcxPixels++ = *pR++; *pPcxPixels++ = *pG++; *pPcxPixels++ = *pB++;
			}while (--i);
	}
	return S_TRUE;
}

S_BOOL _hai_PcxPlanesToPixels(S_BYTE *pPixels, S_BYTE *pBitPlanes, S_WORD Width, S_WORD BytesPerLine, S_WORD Planes, S_WORD BitsPerPixel, S_BYTE pPalette[][3])
{
	S_INT i, j;
	S_BYTE *p = pPixels;
	S_INT pixbit, bits, mask;
	
	if (Planes > 4 || BitsPerPixel != 1)
		return S_FALSE;

	memset(p, 0x00, (BytesPerLine<<3)/BitsPerPixel);
	
	for (i = 0; i < Planes; i++){
		pixbit = (1 << i);
		p = pPixels;
		for (j = 0; j < BytesPerLine; j++){
			bits = *pBitPlanes++;
			for (mask = 0X80; mask != 0; mask >>= 1, p++)
				if (bits & mask) *p |= pixbit;
		}
	}

	p = pPixels + Width*3 - 1;
	pPixels += Width - 1;
	i = (Width+7)>>3;
	switch (Width%8)
	{
	case 0:do{*p-- = pPalette[*pPixels][2]; *p-- = pPalette[*pPixels][1]; *p-- = pPalette[*pPixels][0]; pPixels--;
	case 7:   *p-- = pPalette[*pPixels][2]; *p-- = pPalette[*pPixels][1]; *p-- = pPalette[*pPixels][0]; pPixels--;
	case 6:   *p-- = pPalette[*pPixels][2]; *p-- = pPalette[*pPixels][1]; *p-- = pPalette[*pPixels][0]; pPixels--;
	case 5:   *p-- = pPalette[*pPixels][2]; *p-- = pPalette[*pPixels][1]; *p-- = pPalette[*pPixels][0]; pPixels--;
	case 4:   *p-- = pPalette[*pPixels][2]; *p-- = pPalette[*pPixels][1]; *p-- = pPalette[*pPixels][0]; pPixels--;
	case 3:   *p-- = pPalette[*pPixels][2]; *p-- = pPalette[*pPixels][1]; *p-- = pPalette[*pPixels][0]; pPixels--;
	case 2:   *p-- = pPalette[*pPixels][2]; *p-- = pPalette[*pPixels][1]; *p-- = pPalette[*pPixels][0]; pPixels--;
	case 1:   *p-- = pPalette[*pPixels][2]; *p-- = pPalette[*pPixels][1]; *p-- = pPalette[*pPixels][0]; pPixels--;
			}while (--i);
	}

	return S_TRUE;
}

S_BOOL _hsi_PcxUnpackPixels(S_BYTE *pPixels, S_BYTE *pBitPlanes, S_WORD Width, S_WORD Planes, S_WORD BitsPerPixel, S_BYTE pPalette[][3])
{
	S_REGISTER S_INT bits;
	S_WORD cnt;
	
	if (Planes != 1)
		return S_FALSE;
	switch (BitsPerPixel)
	{
	case 8:
		cnt = (Width+7)>>3;
		switch (Width%8)
		{
		case 0:do{bits = (S_INT)*pBitPlanes++;*pPixels++ = pPalette[bits][0];*pPixels++ = pPalette[bits][1];*pPixels++ = pPalette[bits][2];
		case 7:   bits = (S_INT)*pBitPlanes++;*pPixels++ = pPalette[bits][0];*pPixels++ = pPalette[bits][1];*pPixels++ = pPalette[bits][2];
		case 6:   bits = (S_INT)*pBitPlanes++;*pPixels++ = pPalette[bits][0];*pPixels++ = pPalette[bits][1];*pPixels++ = pPalette[bits][2];
		case 5:   bits = (S_INT)*pBitPlanes++;*pPixels++ = pPalette[bits][0];*pPixels++ = pPalette[bits][1];*pPixels++ = pPalette[bits][2];
		case 4:   bits = (S_INT)*pBitPlanes++;*pPixels++ = pPalette[bits][0];*pPixels++ = pPalette[bits][1];*pPixels++ = pPalette[bits][2];
		case 3:   bits = (S_INT)*pBitPlanes++;*pPixels++ = pPalette[bits][0];*pPixels++ = pPalette[bits][1];*pPixels++ = pPalette[bits][2];
		case 2:   bits = (S_INT)*pBitPlanes++;*pPixels++ = pPalette[bits][0];*pPixels++ = pPalette[bits][1];*pPixels++ = pPalette[bits][2];
		case 1:   bits = (S_INT)*pBitPlanes++;*pPixels++ = pPalette[bits][0];*pPixels++ = pPalette[bits][1];*pPixels++ = pPalette[bits][2];
				}while (--cnt);
		}
		break;
	case 4:
		while (Width >= 2){
			Width -= 2;
			bits = *pBitPlanes++;
			*pPixels++ = pPalette[(bits>>4)&0X0F][0];
			*pPixels++ = pPalette[(bits>>4)&0X0F][1];
			*pPixels++ = pPalette[(bits>>4)&0X0F][2];
			*pPixels++ = pPalette[bits&0X0F][0];
			*pPixels++ = pPalette[bits&0X0F][1];
			*pPixels++ = pPalette[bits&0X0F][2];
		}
		if (Width == 1){
			bits = *pBitPlanes++;
			*pPixels++ = pPalette[(bits>>4)&0X0F][0];
			*pPixels++ = pPalette[(bits>>4)&0X0F][1];
			*pPixels++ = pPalette[(bits>>4)&0X0F][2];
		}
		break;
	case 2:
		while (Width >= 4){
			Width -= 4;
			bits = *pBitPlanes++;
			*pPixels++ = pPalette[(bits>>6)&0X03][0];*pPixels++ = pPalette[(bits>>6)&0X03][1];*pPixels++ = pPalette[(bits>>6)&0X03][2];
			*pPixels++ = pPalette[(bits>>4)&0X03][0];*pPixels++ = pPalette[(bits>>4)&0X03][1];*pPixels++ = pPalette[(bits>>4)&0X03][2];
			*pPixels++ = pPalette[(bits>>2)&0X03][0];*pPixels++ = pPalette[(bits>>2)&0X03][1];*pPixels++ = pPalette[(bits>>2)&0X03][2];
			*pPixels++ = pPalette[(bits>>0)&0X03][0];*pPixels++ = pPalette[(bits>>0)&0X03][1];*pPixels++ = pPalette[(bits>>0)&0X03][2];
		}
		if (Width)
		{
			S_SHORT w = (S_SHORT)Width;
			bits = *pBitPlanes++;
			*pPixels++ = pPalette[(bits>>6)&0X03][0];*pPixels++ = pPalette[(bits>>6)&0X03][1];*pPixels++ = pPalette[(bits>>6)&0X03][2];
			if (--w > 0)
				{*pPixels++ = pPalette[(bits>>4)&0X03][0];*pPixels++ = pPalette[(bits>>4)&0X03][1];*pPixels++ = pPalette[(bits>>4)&0X03][2];}
			if (--w > 0)
				{*pPixels++ = pPalette[(bits>>2)&0X03][0];*pPixels++ = pPalette[(bits>>2)&0X03][1];*pPixels++ = pPalette[(bits>>2)&0X03][2];}
		}
		break;
	case 1:
		while (Width >= 8){
			Width -= 8;
			bits = *pBitPlanes++;
			*pPixels++ = pPalette[(bits>>7)&0X01][0];*pPixels++ = pPalette[(bits>>7)&0X01][1];*pPixels++ = pPalette[(bits>>7)&0X01][2];
			*pPixels++ = pPalette[(bits>>6)&0X01][0];*pPixels++ = pPalette[(bits>>6)&0X01][1];*pPixels++ = pPalette[(bits>>6)&0X01][2];
			*pPixels++ = pPalette[(bits>>5)&0X01][0];*pPixels++ = pPalette[(bits>>5)&0X01][1];*pPixels++ = pPalette[(bits>>5)&0X01][2];
			*pPixels++ = pPalette[(bits>>4)&0X01][0];*pPixels++ = pPalette[(bits>>4)&0X01][1];*pPixels++ = pPalette[(bits>>4)&0X01][2];
			*pPixels++ = pPalette[(bits>>3)&0X01][0];*pPixels++ = pPalette[(bits>>3)&0X01][1];*pPixels++ = pPalette[(bits>>3)&0X01][2];
			*pPixels++ = pPalette[(bits>>2)&0X01][0];*pPixels++ = pPalette[(bits>>2)&0X01][1];*pPixels++ = pPalette[(bits>>2)&0X01][2];
			*pPixels++ = pPalette[(bits>>1)&0X01][0];*pPixels++ = pPalette[(bits>>1)&0X01][1];*pPixels++ = pPalette[(bits>>1)&0X01][2];
			*pPixels++ = pPalette[(bits>>0)&0X01][0];*pPixels++ = pPalette[(bits>>0)&0X01][1];*pPixels++ = pPalette[(bits>>0)&0X01][2];
		}
		if (Width)
		{
			S_SHORT w = (S_SHORT)Width;
			bits = *pBitPlanes++;
			{*pPixels++ = pPalette[(bits>>7)&0X01][0];*pPixels++ = pPalette[(bits>>7)&0X01][1];*pPixels++ = pPalette[(bits>>7)&0X01][2];}
			if (--w > 0)
				{*pPixels++ = pPalette[(bits>>6)&0X01][0];*pPixels++ = pPalette[(bits>>6)&0X01][1];*pPixels++ = pPalette[(bits>>6)&0X01][2];}
			if (--w > 0)
				{*pPixels++ = pPalette[(bits>>5)&0X01][0];*pPixels++ = pPalette[(bits>>5)&0X01][1];*pPixels++ = pPalette[(bits>>5)&0X01][2];}
			if (--w > 0)
				{*pPixels++ = pPalette[(bits>>4)&0X01][0];*pPixels++ = pPalette[(bits>>4)&0X01][1];*pPixels++ = pPalette[(bits>>4)&0X01][2];}
			if (--w > 0)
				{*pPixels++ = pPalette[(bits>>3)&0X01][0];*pPixels++ = pPalette[(bits>>3)&0X01][1];*pPixels++ = pPalette[(bits>>3)&0X01][2];}
			if (--w > 0)
				{*pPixels++ = pPalette[(bits>>2)&0X01][0];*pPixels++ = pPalette[(bits>>2)&0X01][1];*pPixels++ = pPalette[(bits>>2)&0X01][2];}
			if (--w > 0)
				{*pPixels++ = pPalette[(bits>>1)&0X01][0];*pPixels++ = pPalette[(bits>>1)&0X01][1];*pPixels++ = pPalette[(bits>>1)&0X01][2];}
			if (--w > 0)
				{*pPixels++ = pPalette[(bits>>0)&0X01][0];*pPixels++ = pPalette[(bits>>0)&0X01][1];*pPixels++ = pPalette[(bits>>0)&0X01][2];}
		}
		break;
	default:
		return S_FALSE;
		break;
	}
	return S_TRUE;
}

S_WORD _hai_PcxReadHeader(S_BYTE *pPcxData, PCXHEADER *pph)
{
	if (pPcxData == S_NULL || pph == S_NULL)
		return 0;
	
	pph->Manufacturer = *pPcxData;
	pph->Version      = *(pPcxData+1);
	pph->Encoding     = *(pPcxData+2);
	pph->BitsPerPixel = *(pPcxData+3);
	pph->Xmin = HAI_MAKEWORD(pPcxData+4); 
	pph->Ymin = HAI_MAKEWORD(pPcxData+6);
	pph->Xmax = HAI_MAKEWORD(pPcxData+8);
	pph->Ymax = HAI_MAKEWORD(pPcxData+10);
	pph->Hres = HAI_MAKEWORD(pPcxData+12);
	pph->Vres = HAI_MAKEWORD(pPcxData+14);
	memcpy(pph->ColorMap, pPcxData+16, 16*3);
	pph->Reserved     = *(pPcxData+64);
	pph->ColorPlanes  = *(pPcxData+65);
	pph->BytesPerLine = HAI_MAKEWORD(pPcxData+66);
	pph->PaletteType  = HAI_MAKEWORD(pPcxData+68);
	memcpy(pph->Filter, pPcxData+70, 58);

	return PCX_HDR_SIZE;
}

