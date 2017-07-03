//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			nand2410.c
// Description:		base-on K9F1208U0A
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-09-19  create                  Kingsea
//---------------------------------------------------------------------------------
#include "hfstype.h"
#include "ftdevice.h"
#include "haiclib.h"


#ifdef ADD_S3C2410_NAND
#define U8 unsigned char
#include "cfg2410.h"
#include "reg2410.h"

//macro define
//
#define NAND_WRITE_VERIFY			1

#define NAND_SECTOR_BYTES			512
#define NAND_SECTOR_MAX				0x20000/*64MB*/
#define NAND_SECT_PER_CLST			32
#define NAND_BUFFER_CLST_MAX		4
#define NAND_ROOT_ENTRY_MAX			(31/*sector*/*(512/32))
#define NAND_FAT_SIZE				16

#define NAND_BUFFER_NEEDFLUSH		0x01000000


//function prototype
//
static void NandReset(void);
static int NandEraseBlock(S_DWORD block);
static int NandReadPage(int page, S_BYTE *OutBuf);
static int NandVerifyPage(int page);
static int NandWritePage(int page, S_BYTE *InBuf);
static S_WORD NandCheckID(void);

static int NANDReadCluster(S_DWORD cluster, S_BYTE *OutBuf);
static int NANDWriteCluster(S_DWORD cluster, S_BYTE *InBuf);
static void NandIncUseCount(int BufNo);
static int NandFindLeastUseCluster(void);

int NANDDevInit(void);
int NANDDevStatus(void);
int NANDDevRead(S_DWORD sector, S_BYTE *OutBuf);
int NANDDevWrite(S_DWORD sector, S_BYTE *InBuf);
int NANDDevCmd(S_DWORD cmd, S_DWORD arg, S_DWORD *OutVal);
int NANDDevFlush(S_DWORD sector, S_BOOL FreeBuffer);

//assert driver struct
//
const S_FATDEVDRV s3c2410NandDrv =
{
	"S3C2410NAND",
	NANDDevInit,
	NANDDevStatus,
	NANDDevRead,
	NANDDevWrite,
	NANDDevCmd,
	NANDDevFlush,
};

//NAND Driver Buffer
//
static S_BYTE  NANDBuf[NAND_BUFFER_CLST_MAX][NAND_SECTOR_BYTES*NAND_SECT_PER_CLST];
static S_DWORD NANDFlag[NAND_BUFFER_CLST_MAX];
static S_DWORD NANDClst[NAND_BUFFER_CLST_MAX];

int NANDDevInit(void)
{
	int i;

	NandReset();

	for (i = 0; i < NAND_BUFFER_CLST_MAX; i++)
		NANDFlag[i] = 0;
	for (i = 0; i < NAND_BUFFER_CLST_MAX; i++)
		NANDClst[i] = 0xFFFFFFFF;

	return 0;	
}

int NANDDevStatus(void)
{
	return 0;
}

int NANDDevRead(S_DWORD sector, S_BYTE *OutBuf)
{
	int i, m = NAND_BUFFER_CLST_MAX-1;
	S_DWORD cluster = sector/NAND_SECT_PER_CLST;
	S_DWORD current = sector%NAND_SECT_PER_CLST;
	
	//find if in driver buffer
	for (i = 0; i < NAND_BUFFER_CLST_MAX; i++)
	{
		if (NANDClst[i] == 0xFFFFFFFF)
			m = i;
		else if (NANDClst[i] == cluster)
			break;
	}
	
	if (i == NAND_BUFFER_CLST_MAX)// not found out
	{
		if (NANDClst[m] != 0xFFFFFFFF) // have not idle cluster, make one idle cluster
		{
			m = NandFindLeastUseCluster();
			if ((NANDFlag[m]&NAND_BUFFER_NEEDFLUSH) == NAND_BUFFER_NEEDFLUSH)
				if (NANDWriteCluster(NANDClst[m], NANDBuf[m]))
				{
					return -1;
				}
		}
		NANDFlag[m] = 0;
		NANDClst[m] = cluster;
		if (NANDReadCluster(NANDClst[m], NANDBuf[m]))//ok, have one idle cluster
		{
			NANDClst[m] = 0xFFFFFFFF;//Oh, shit! fail on read disk
			return -1;
		}
		i = m;//make it as found successful....
	}

	if (NANDClst[i] == cluster)//OK, found out
	{
		S_BYTE *pDat = &NANDBuf[i][current*NAND_SECTOR_BYTES];
#if 0		
		for (m = 0; m < NAND_SECTOR_BYTES; m++)
			OutBuf[m]  = pDat[m];
#else
		hai_memcpy(OutBuf, pDat, NAND_SECTOR_BYTES);
#endif
		NandIncUseCount(i);
	}
	
	return 0;	
}

int NANDDevWrite(S_DWORD sector, S_BYTE *InBuf)
{
	int i, m = NAND_BUFFER_CLST_MAX-1;
	S_DWORD cluster = sector/NAND_SECT_PER_CLST;
	S_DWORD current = sector%NAND_SECT_PER_CLST;
	
	//find if in driver buffer
	for (i = 0; i < NAND_BUFFER_CLST_MAX; i++)
	{
		if (NANDClst[i] == 0xFFFFFFFF)
			m = i;
		else if (NANDClst[i] == cluster)
			break;
	}
	
	if (i == NAND_BUFFER_CLST_MAX)// not found out
	{
		if (NANDClst[m] != 0xFFFFFFFF) // have not idle cluster, make one idle cluster
		{
			m = NandFindLeastUseCluster();
			if ((NANDFlag[m]&NAND_BUFFER_NEEDFLUSH) == NAND_BUFFER_NEEDFLUSH)
				if (NANDWriteCluster(NANDClst[m], NANDBuf[m]))
				{
					return -1;
				}
		}
		NANDFlag[m] = 0;
		NANDClst[m] = cluster;
		if (NANDReadCluster(NANDClst[m], NANDBuf[m]))//ok, have one idle cluster
		{
			NANDClst[m] = 0xFFFFFFFF;//Oh, shit! fail on read disk
			return -1;
		}
		i = m;//make it as found successful....
	}

	if (NANDClst[i] == cluster)//OK, found out
	{
		S_BYTE *pDat = &NANDBuf[i][current*NAND_SECTOR_BYTES];
#if 0		
		for (i = 0; i < NAND_SECTOR_BYTES; i++)
			pDat[i] = InBuf[i];
#else
		hai_memcpy(pDat, InBuf, NAND_SECTOR_BYTES);
#endif
		NANDFlag[i] |= NAND_BUFFER_NEEDFLUSH;
		NandIncUseCount(i);
	}
		
	return 0;
}

int NANDDevCmd(S_DWORD cmd, S_DWORD arg, S_DWORD *OutVal)
{
	int err = 0;
	switch (cmd)
	{
	case DEVCMD_WRITE_PROTECT:
		*OutVal = 0;
		break;
	case DEVCMD_SECTOR_SIZE:
		*OutVal = NAND_SECTOR_BYTES;//this is a recommand val, otherwise:1024, 2048, 4096.
		break;
	case DEVCMD_TOTAL_SECTOR:
		*OutVal = NAND_SECTOR_MAX;
		break;
	case DEVCMD_SEC_PER_CLUS:
		*OutVal = NAND_SECT_PER_CLST; //or 1, 2, 4, 8, 16, 32, 64, 128;
		break;
	case DEVCMD_ROOTENTCNT_F16:
		*OutVal = NAND_ROOT_ENTRY_MAX;
		break;
	case DEVCMD_FAT_SIZE:
		*OutVal = NAND_FAT_SIZE;
		break;
	default:
		*OutVal = 0;
		break;
	}
	return err;
}

int NANDDevFlush(S_DWORD sector, S_BOOL FreeBuffer)
{
	int i, start, end;
	
	if (sector == 0xFFFFFFFF)
	{
		start = 0; end = NAND_BUFFER_CLST_MAX;
	}
	else
	{
		S_DWORD cluster = sector/NAND_SECT_PER_CLST;
		for (i = 0; i < NAND_BUFFER_CLST_MAX; i++)
			if (NANDClst[i] == cluster)
				break;
		if (i == NAND_BUFFER_CLST_MAX)
		{
			start = 0; end = 0;
		}
		else
		{
			start = i; end = i+1;
		}
	}
	
	for (i = start; i < end; i++)
	{
		if ((NANDFlag[i]&NAND_BUFFER_NEEDFLUSH) == NAND_BUFFER_NEEDFLUSH)
		{
			if (NANDWriteCluster(NANDClst[i], NANDBuf[i]))
			{
				return -1;
			}
			NANDFlag[i] &= ~NAND_BUFFER_NEEDFLUSH;
		}
		if (FreeBuffer)
		{
			NANDFlag[i] = 0x00;
			NANDClst[i] = 0xFFFFFFFF;
		}
	}
	return 0;
}

#define NandCMD(cmd)	rNFCMD = cmd
#define NandADDR(addr)	rNFADDR = addr	
#define NandActive()	rNFCONF &= ~(1<<11)
#define NandInactive()	rNFCONF |= (1<<11)
#define NandResetECC()	rNFCONF |= (1<<12)
#define NandRead() 		(rNFDATA)
#define NandWrite(data) rNFDATA = data
#define NandWaitRB()    while(!(rNFSTAT&(1<<0))) 
#if 0
// HCLK=100Mhz
#define TACLS		0  //1clk(0ns) 
#define TWRPH0		3  //3clk(25ns)
#define TWRPH1		0  //1clk(10ns)  //TACLS+TWRPH0+TWRPH1>=50ns
#else
// HCLK=50Mhz
#define TACLS		0  //1clk(0ns)
#define TWRPH0		1  //2clk(25ns)
#define TWRPH1		0  //1clk(10ns)
#endif

#define NANDCMD_RESET			0xFF
#define NANDCMD_READ1_00		0x00
#define NANDCMD_READ1_01		0x01
#define NANDCMD_READ2			0x50
#define NANDCMD_PAGEPROG_S		0x80
#define NANDCMD_PAGEPROG_E		0x10
#define NANDCMD_BLOCKERASE_S	0x60
#define NANDCMD_BLOCKERASE_E	0xD0
#define NANDCMD_READSTATUS		0x70
#define NANDCMD_READID			0x90

static void NandReset(void)
{
    int i;
    
    rNFCONF=(1<<15)|(1<<14)|(1<<13)|(1<<12)|(1<<11)|(TACLS<<8)|(TWRPH0<<4)|(TWRPH1<<0);	
    // 1  1    1     1,   1      xxx,  r xxx,   r xxx        
    // En 512B 4step ECCR nFCE=H tACLS   tWRPH0   tWRPH1
    
	NandActive();
	NandCMD(NANDCMD_RESET);
	for(i = 0; i < 10; i++);
	NandWaitRB();
	NandInactive();
}

//block range: 0~4096
static int NandEraseBlock(S_DWORD block)
{
	int i, err;

	block <<= 5;//32 page per block
	NandActive();

	NandCMD(NANDCMD_BLOCKERASE_S);
	NandADDR((S_BYTE)(block&0xFF));// Page number=0
	NandADDR((S_BYTE)((block>>8)&0xFF));   
	NandADDR((S_BYTE)((block>>16)&0xFF));
	NandCMD(NANDCMD_BLOCKERASE_E);

	for(i = 0; i < 10; i++)
		;
	NandWaitRB();

	NandCMD(NANDCMD_READSTATUS);
	for(i = 0; i < 10; i++)
		;
	NandWaitRB();

	if (NandRead()&0x1)
		err = -1;
	else 
		err = 0;

	NandInactive();

	return err;
}

static int NandReadPage(int page, S_BYTE *OutBuf)
{
	int i;
	S_BYTE ECC0,ECC1,ECC2;
	S_BYTE spare[16];	   

	NandResetECC();
	NandActive();    
	NandCMD(NANDCMD_READ1_00);
	NandADDR(0);// Column = 0
	NandADDR(page&0xFF);
	NandADDR((page>>8)&0xFF);
	NandADDR((page>>16)&0xFF);

	for(i = 0; i < 10; i++)
		;
	NandWaitRB();

	for(i = 0; i < 512; i++)
		*OutBuf++ = NandRead();

	ECC0 = rNFECC0;
	ECC1 = rNFECC1;
	ECC2 = rNFECC2;
	for(i = 0; i < 16; i++)
		spare[i] = NandRead();

	NandInactive();    

	if(ECC0==spare[0] && ECC1==spare[1] && ECC2==spare[2])
		return 0;
	else
		return -1;
}

static int NandVerifyPage(int page)
{
	S_BYTE dummy[512];

	return NandReadPage(page, dummy);
}

static int NandWritePage(int page, S_BYTE *InBuf)
{
	int i, err = 0;
	S_BYTE spare[16];

	NandResetECC();
	NandActive();
	NandCMD(0x0);//??????
	NandCMD(NANDCMD_PAGEPROG_S);
	NandADDR(0);// Column 0
	NandADDR(page&0xFF);
	NandADDR((page>>8)&0xFF);
	NandADDR((page>>16)&0xFF);

	for(i = 0; i < 512; i++)
		NandWrite(*InBuf++);

	spare[0] = rNFECC0;	spare[1] = rNFECC1;	spare[2] = rNFECC2;	spare[3] = 0xFF;
	spare[4] = 0xFF;	spare[5] = 0xFF;	spare[6] = 0xFF;	spare[7] = 0xFF;
	spare[8] = 0xFF;	spare[9] = 0xFF;	spare[10] = 0xFF;	spare[11] = 0xFF;
	spare[12] = 0xFF;	spare[13] = 0xFF;	spare[14] = 0xFF;	spare[15] = 0xFF;

	for(i = 0; i < 16; i++)
		NandWrite(spare[i]);

	NandCMD(NANDCMD_PAGEPROG_E);

	for(i=0;i<10;i++)
		;
	NandWaitRB();

	NandCMD(NANDCMD_READSTATUS);
	for(i = 0; i < 3; i++);
	NandWaitRB();

	if (NandRead()&0x1)
		err = -1;
	NandInactive();

#if (NAND_WRITE_VERIFY==1)
	if (!err)
		err = NandVerifyPage(page);	
#endif

	return err;
}

static S_WORD NandCheckID(void)
{
	int i;
	S_WORD id;

	NandActive();

	NandCMD(NANDCMD_READID);
	NandADDR(0x0);
	for(i=0;i<10;i++)
		;

	id  = NandRead();
	id <<= 8;
	id |= NandRead();

	NandInactive();

	return id;
}

static int NANDReadCluster(S_DWORD cluster, S_BYTE *OutBuf)
{
	int i, try, err = 0;

	cluster <<= 5;
	for (i = 0; i < NAND_SECT_PER_CLST; i++)
	{
		try = 3;
		do
		{
			err = NandReadPage(cluster+i, OutBuf+i*NAND_SECTOR_BYTES);
		}while (err && try--);

		if (err) break;
	}
	
	return err;
}

static int NANDWriteCluster(S_DWORD cluster, S_BYTE *InBuf)
{
	int i, try, err = 0;

	try = 3;
	do
	{
		err = NandEraseBlock(cluster);
	}while (err && try--);

	if (err) return err;

	cluster <<= 5;
	for (i = 0; i < NAND_SECT_PER_CLST; i++)
	{
		try = 3;
		do
		{
			err = NandWritePage(cluster+i, InBuf+i*NAND_SECTOR_BYTES);
		}while (err && try--);

		if (err) break;
	}
	
	return err;
}

static int NandFindLeastUseCluster(void)
{
	int i = 0, m = 0;
	S_DWORD UseCnt = NANDFlag[i]&0x0000FFFF;
	
	for (i = 1; i < NAND_BUFFER_CLST_MAX; i++)
	{
		if (UseCnt > (NANDFlag[i]&0x0000FFFF))
		{
			UseCnt = NANDFlag[i]&0x0000FFFF;
			m = i;
		}
	}
	return m;
}

static void NandIncUseCount(int BufNo)
{
	S_DWORD UseCnt = NANDFlag[BufNo]&0x0000FFFF;
	
	UseCnt = (UseCnt<0xFFFF)? (UseCnt+1):(UseCnt);//increase use count
	NANDFlag[BufNo] = (NANDFlag[BufNo]&0xFFFF0000)|(UseCnt&0x0000FFFF);
}



#endif //#ifdef ADD_S3C2410_NAND

