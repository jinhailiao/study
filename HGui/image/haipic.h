//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haipic.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 2.0.0		2006-09-06  create                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------
#ifndef _HAIPIC_H_
#define _HAIPIC_H_

#include "haitype.h"
#include "haihandle.h"

#define BI_RGB			0L
#define BI_RLE8			1L
#define BI_RLE4			2L
#define BI_BITFIELDS		3L
#define MASKWIDTH(w)	((((w)+31)>>5)<<2)

typedef struct tagRGBQUAD {
	S_BYTE    rgbRed;
	S_BYTE    rgbGreen;
	S_BYTE    rgbBlue;
	S_BYTE    rgbReserved;
} 
RGBQUAD;

typedef struct tagBITMAPINFOHEADER{
	S_DWORD      biSize;
	S_LONG        biWidth;
	S_LONG        biHeight;
	S_WORD       biPlanes;
	S_WORD       biBitCount;
	S_DWORD      biCompression;
	S_DWORD      biSizeImage;
	S_LONG        biXPelsPerMeter;
	S_LONG        biYPelsPerMeter;
	S_DWORD      biClrUsed;
	S_DWORD      biClrImportant;
}
BITMAPINFOHEADER;

typedef struct tagIcon
{
	S_WORD  width;
	S_WORD  height;
	S_WORD  BitsPixel;
	S_WORD	WidthBytes;
	S_VOID  *lpImage;
	S_VOID  *lpMask;
}
S_ICON, *SP_ICON;


SH_BITMAP hai_BmpDecode(S_VOID *pBmpData);
SH_BITMAP hai_PcxDecode(S_VOID *pPcxData);

//
//  function : S_BYTE hai_GetIcoFrameTotal(S_VOID *pIcoData)
//description: 返回ICON的总帧数
S_BYTE hai_GetIcoFrameTotal(S_VOID *pIcoData);

//
//  function : SH_ICON hai_IcoDecode(S_VOID *pIcoData, S_BYTE frame)
//description: 参数frame范围1--总帧数
SH_ICON hai_IcoDecode(S_VOID *pIcoData, S_BYTE frame);


#endif //_HAIPIC_H_

