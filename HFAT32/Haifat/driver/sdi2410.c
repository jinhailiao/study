//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			sdi2410.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-09-09  create                  Kingsea
//---------------------------------------------------------------------------------
#include "hfstype.h"
#include "ftdevice.h"
#include "haiclib.h"

#ifdef ADD_S3C2410_SDI

#include "cfg2410.h"
#include "reg2410.h"

//note: only support SD_CARD partition 1 now.

//macro define
//
#define SDT_NO_CARD			0
#define SDT_MMC_CARD		1
#define SDT_SD_CARD			2
#define SDT_SDHC_CARD		3

#define SD_BUSW_1BIT		0x00
#define SD_BUSW_4BIT		0x01
#define SD_BUSW_MASK		0x01
#define SD_WTPROT_NON		0x00
#define SD_WTPROT_YES		0x02
#define SD_WTPROT_MSK		0x02

#define pSD_MBR(p)			(p+0)
#define pSD_MBR_PT1(p)		(p+446)
#define pSD_MBR_PT2(p)		(p+462)
#define pSD_MBR_PT3(p)		(p+478)
#define pSD_MBR_PT4(p)		(p+494)
#define pSD_MBR_SW(p)		(p+510)
#define sSD_MBR				446
#define sSD_MBR_PT			16
#define sSD_MBR_SW			2

#define pSD_PT_BOOTINDIC(p)			(p+0)
#define pSD_PT_STARTHEAD(p)			(p+1)
#define pSD_PT_STARTSECCYL(p)		(p+2)
#define pSD_PT_SYSTEMID(p)			(p+4)
#define pSD_PT_ENDHEAD(p)			(p+5)
#define pSD_PT_ENDSECCYL(p)			(p+6)
#define pSD_PT_RELATIVESEC(p)		(p+8)
#define pSD_PT_TOTALSECTOR(p)		(p+12)
#define sSD_PT_BOOTINDIC			1
#define sSD_PT_STARTHEAD			1
#define sSD_PT_STARTSECCYL			2
#define sSD_PT_SYSTEMID				1
#define sSD_PT_ENDHEAD				1
#define sSD_PT_ENDSECCYL			2
#define sSD_PT_RELATIVESEC			4
#define sSD_PT_TOTALSECTOR			4

#define SDI_INITIALCLK	400000
#define SDI_NORMALCLK	5000000

//command format
//
// 10    9     8           7        6     5...0
//LRSP WTRSP CMDStart startbit:0  host:1  CMDIDX
#define SDICMD_HOST		0x00000040UL//(1<<6)
#define SDICMD_START	0x00000100UL//(1<<8)
#define SDICMD_WTRSP	0x00000200UL//(1<<9)
#define SDICMD_L_RSP	0x00000400UL//(1<<10)
#define SDICMD_NORSP	0x00000000UL//(0<<9)
#define SDICMD_S_RSP	0x00000000UL//(0<<10)

#define SDICMD_IDXMSK	0x0000003FUL

//command class
//
#define SDICMD_GO_IDLE_STATE	0
#define SDICMD_OP_COND			1
#define SDICMD_ALL_SEND_CID		2
#define SDICMD_SEND_REL_ADDR	3
#define SDICMD_SEL_DSEL_CARD	7
#define SDICMD_SEND_CSD			9
#define SDICMD_SEND_CID			10
#define SDICMD_STOP_TRANS		12
#define SDICMD_SEND_STATUS		13
#define SDICMD_SET_BLOCKLEN		16
#define SDICMD_READ_SINGLBLK	17
#define SDICMD_READ_MULTIBLK	18
#define SDICMD_WRITE_SINGLBLK	24
#define SDICMD_WRITE_MULTIBLK	25
#define SDICMD_SEND_WRIT_PROT	30
#define SDICMD_APP_CMD			55

#define SDICMD_SET_BUS_WIDTH	6
#define SDICMD_SD_STATUS		13
#define SDICMD_SD_APP_OP_COND	41

#define SDCARD_BLOCK_LENG_MAX		1024
#define SDCARD_BUFFER_BLOCK_MAX		10
#define SDCARD_FAT_SECTOR_BYTES		512
#define SDCARD_BUFFER_NEEDFLUSH		0x01000000


//assert struct
//
typedef struct tagPartTab
{
	S_BYTE BootIndicator;//active partition:0x80;otherwise:0x00
	S_BYTE StartHead;
	S_WORD StartSecCyl;
	S_BYTE SystemID;//0x01 or 0x04 or 0x06
	S_BYTE EndHead;
	S_WORD EndSecCyl;
	S_DWORD RelativeSec;
	S_DWORD TotalSector;
}
S_PARTTAB, *SP_PARTTAB;

typedef struct tagCID
{
	S_BYTE MID;//binary:0x03
	S_BYTE OID[2];//ASCII:"SD"
	S_BYTE PNM[5];//ASCII:"SD128"
	S_BYTE PRV;//BCD:30
	S_BYTE PSN[4];//binary
	S_BYTE MDT[2];//BCD:yym(offset from 2000)
	S_BYTE CRC;//binary
}
S_CID, *SP_CID;

typedef struct tagCSD
{
	S_BYTE CSDStruct;
//	S_BYTE ReadAccessTime1;
//	S_BYTE ReadAccessTime2;
//	S_BYTE DataTransRate;
//	S_WORD CardCmdClasses;
	S_BYTE ReadBlockLen;
//	S_BYTE ReadBlockPartial;
	S_WORD DeviceSize;
	S_BYTE DeviceSizeMulti;
}
S_CSD, *SP_CSD;

typedef struct tagSDCARD
{
	S_BYTE CardType;
	S_BYTE flag;
	S_WORD BlockLen;
	S_WORD RCA;
	S_CID  CID;
	S_CSD  CSD;
	S_PARTTAB part[4];//only support part[0] now.
}
S_SDCARD, *SP_SDCARD;

static S_SDCARD sdcard;

//function prototype
//
int s3c2410sdiInit(void);
int s3c2410sdiStatus(void);
int s3c2410sdiRead(S_DWORD sector, S_BYTE *OutBuf);
int s3c2410sdiWrite(S_DWORD sector, S_BYTE *InBuf);
int s3c2410sdiCmd(S_DWORD cmd, S_DWORD arg, S_DWORD *OutVal);
int s3c2410sdiFlush(S_DWORD sector, S_BOOL FreeBuffer);

static int sdi_SendCmd(int rCMD, int rARG);
static int sdi_CMD55(S_WORD RCA);
static int sdi_ChkMMCOCR(void);
static int sdi_ChkSDOCR(void);
static int sdi_GetCID(S_WORD RCA, SP_CID pCID);
static int sdi_GetCSD(S_WORD RCA, SP_CSD pCSD);
static int sdi_MakeIdent(void);
static int sdi_MakeStandby(S_WORD *pRCA);
static int sdi_CardSelect(S_WORD RCA);
static int sdi_SetBusWidth(S_WORD RCA, int BUSW);
static int sdi_CheckDATend(void);

static int sdi_SDcardInit(void);
static int sdi_ReadBlock(S_DWORD block, S_BYTE *OutBuf);
static int sdi_WriteBlock(S_DWORD block, S_BYTE *InBuf);

static void sdi_IncUseCount(int BufNo);
static int sdi_FindLeastUseCluster(void);

//assert driver struct
//
const S_FATDEVDRV s3c2410sdiDrv =
{
	"S3C2410_SDI",
	s3c2410sdiInit,
	s3c2410sdiStatus,
	s3c2410sdiRead,
	s3c2410sdiWrite,
	s3c2410sdiCmd,
	s3c2410sdiFlush,
};

//Driver Buffer
//
static S_BYTE  SDCARDBuf[SDCARD_BUFFER_BLOCK_MAX][SDCARD_BLOCK_LENG_MAX];
static S_DWORD SDCARDFlag[SDCARD_BUFFER_BLOCK_MAX];
static S_DWORD SDCARDClst[SDCARD_BUFFER_BLOCK_MAX];


//driver function define
//
int s3c2410sdiInit(void)
{
	int err, try = 3;
	S_BYTE *pPart, *pMBR = SDCARDBuf[0];

	do
	{
		err = sdi_SDcardInit();
	}while (err && try--);
	if (err) return err;

	//read partition
	try = 3;
	do
	{
		err = sdi_ReadBlock(0, pMBR);
	}while (err && try--);
	if (err) return err;

	pPart = pSD_MBR_PT1(pMBR);
	sdcard.part[0].BootIndicator = *pSD_PT_BOOTINDIC(pPart);
	sdcard.part[0].StartHead   = *pSD_PT_STARTHEAD(pPart);
	sdcard.part[0].StartSecCyl = HAI_MAKEWORD(pSD_PT_STARTSECCYL(pPart));
	sdcard.part[0].SystemID    = *pSD_PT_SYSTEMID(pPart);
	sdcard.part[0].EndHead     = *pSD_PT_ENDHEAD(pPart);
	sdcard.part[0].EndSecCyl   = HAI_MAKEWORD(pSD_PT_ENDSECCYL(pPart));
	sdcard.part[0].RelativeSec = HAI_MAKEDWORD(pSD_PT_RELATIVESEC(pPart));
	sdcard.part[0].TotalSector = HAI_MAKEDWORD(pSD_PT_TOTALSECTOR(pPart));
	
	for (try = 0; try < SDCARD_BUFFER_BLOCK_MAX; try++)
		SDCARDFlag[try] = 0;
	for (try = 0; try < SDCARD_BUFFER_BLOCK_MAX; try++)
		SDCARDClst[try] = 0xFFFFFFFF;

	return err;
}

int s3c2410sdiStatus(void)
{
	return 0;//always OK.
}

int s3c2410sdiRead(S_DWORD sector, S_BYTE *OutBuf)
{
	int err, try;
	int i, m = SDCARD_BUFFER_BLOCK_MAX-1;
	S_DWORD cluster, current;

	sector += sdcard.part[0].RelativeSec;
	cluster = sector/(sdcard.BlockLen/SDCARD_FAT_SECTOR_BYTES);
	current = sector%(sdcard.BlockLen/SDCARD_FAT_SECTOR_BYTES);
	
	//find if in driver buffer
	for (i = 0; i < SDCARD_BUFFER_BLOCK_MAX; i++)
	{
		if (SDCARDClst[i] == 0xFFFFFFFF)
			m = i;
		else if (SDCARDClst[i] == cluster)
			break;
	}
	
	if (i == SDCARD_BUFFER_BLOCK_MAX)// not found out
	{
		if (SDCARDClst[m] != 0xFFFFFFFF) // have not idle cluster, make one idle cluster
		{
			m = sdi_FindLeastUseCluster();
			if ((SDCARDFlag[m]&SDCARD_BUFFER_NEEDFLUSH) == SDCARD_BUFFER_NEEDFLUSH)
			{
				try = 3;
				do
				{
					err = sdi_WriteBlock(SDCARDClst[m], SDCARDBuf[m]);
				}while (err && try--);

				if (err)
					return err;
			}
		}
		SDCARDFlag[m] = 0;
		SDCARDClst[m] = cluster;//ok, have one idle cluster
		try = 3;
		do
		{
			err = sdi_ReadBlock(SDCARDClst[m], SDCARDBuf[m]);
		}while (err && try--);
		if (err)
		{
			SDCARDClst[m] = 0xFFFFFFFF;//Oh, shit! fail on read disk
			return err;
		}
		i = m;//make it as found successful....
	}

	if (SDCARDClst[i] == cluster)//OK, found out
	{
#if 0
		S_BYTE *pDat = &SDCARDBuf[i][current*SDCARD_FAT_SECTOR_BYTES];
		hai_memcpy(OutBuf, pDat, SDCARD_FAT_SECTOR_BYTES);
#else
		int *pDat = (int *)&SDCARDBuf[i][current*SDCARD_FAT_SECTOR_BYTES];
		int *pOut = (int *)OutBuf;
		for (try = 0; try < SDCARD_FAT_SECTOR_BYTES/4; try++)
			*pOut++ = *pDat++;
#endif
		sdi_IncUseCount(i);
	}
	
	return 0;	
}

int s3c2410sdiWrite(S_DWORD sector, S_BYTE *InBuf)
{
	int err, try;
	int i, m = SDCARD_BUFFER_BLOCK_MAX-1;
	S_DWORD cluster, current;

	sector += sdcard.part[0].RelativeSec;
	cluster = sector/(sdcard.BlockLen/SDCARD_FAT_SECTOR_BYTES);
	current = sector%(sdcard.BlockLen/SDCARD_FAT_SECTOR_BYTES);
	
	//find if in driver buffer
	for (i = 0; i < SDCARD_BUFFER_BLOCK_MAX; i++)
	{
		if (SDCARDClst[i] == 0xFFFFFFFF)
			m = i;
		else if (SDCARDClst[i] == cluster)
			break;
	}
	
	if (i == SDCARD_BUFFER_BLOCK_MAX)// not found out
	{
		if (SDCARDClst[m] != 0xFFFFFFFF) // have not idle cluster, make one idle cluster
		{
			m = sdi_FindLeastUseCluster();
			if ((SDCARDFlag[m]&SDCARD_BUFFER_NEEDFLUSH) == SDCARD_BUFFER_NEEDFLUSH)
			{
				try = 3;
				do
				{
					err = sdi_WriteBlock(SDCARDClst[m], SDCARDBuf[m]);
				}while (err && try--);
				if (err)
					return err;
			}
		}
		SDCARDFlag[m] = 0;
		SDCARDClst[m] = cluster;//ok, have one idle cluster
		try = 3;
		do
		{
			err = sdi_ReadBlock(SDCARDClst[m], SDCARDBuf[m]);
		}while (err && try--);
		if (err)
		{
			SDCARDClst[m] = 0xFFFFFFFF;//Oh, shit! fail on read disk
			return err;
		}
		i = m;//make it as found successful....
	}

	if (SDCARDClst[i] == cluster)//OK, found out
	{
#if 0
		S_BYTE *pDat = &SDCARDBuf[i][current*SDCARD_FAT_SECTOR_BYTES];
		hai_memcpy(pDat, InBuf, SDCARD_FAT_SECTOR_BYTES);
#else
		int *pDat = (int *)&SDCARDBuf[i][current*SDCARD_FAT_SECTOR_BYTES];
		int *pIn  = (int *)InBuf;
		for (try = 0; try < SDCARD_FAT_SECTOR_BYTES/4; try++)
			*pDat++ = *pIn++;
#endif
		SDCARDFlag[i] |= SDCARD_BUFFER_NEEDFLUSH;
		sdi_IncUseCount(i);
	}
		
	return 0;
}

int s3c2410sdiCmd(S_DWORD cmd, S_DWORD arg, S_DWORD *OutVal)
{
	int err = 0;
	switch (cmd)
	{
	case DEVCMD_WRITE_PROTECT:
		*OutVal = (sdcard.flag&SD_WTPROT_MSK)?1:0;
		break;
	case DEVCMD_SECTOR_SIZE:
		*OutVal = SDCARD_FAT_SECTOR_BYTES;//this is a recommand val, otherwise:1024, 2048, 4096.
		break;
	case DEVCMD_TOTAL_SECTOR:
		*OutVal = sdcard.part[0].TotalSector;
		break;
	case DEVCMD_SEC_PER_CLUS:
		*OutVal = 0; //or 1, 2, 4, 8, 16, 32, 64, 128;
		break;
	default:
		*OutVal = 0;
		break;
	}
	return err;
}

int s3c2410sdiFlush(S_DWORD sector, S_BOOL FreeBuffer)
{
	int err, try;
	int i, start, end;
	
	if (sector == 0xFFFFFFFF)
	{
		start = 0; end = SDCARD_BUFFER_BLOCK_MAX;
	}
	else
	{
		S_DWORD cluster = (sector+sdcard.part[0].RelativeSec)/(sdcard.BlockLen/SDCARD_FAT_SECTOR_BYTES);
		for (i = 0; i < SDCARD_BUFFER_BLOCK_MAX; i++)
			if (SDCARDClst[i] == cluster)
				break;
		if (i == SDCARD_BUFFER_BLOCK_MAX)
			start = 0, end = 0;
		else
			start = i, end = i+1;
	}
	
	for (i = start; i < end; i++)
	{
		if ((SDCARDFlag[i]&SDCARD_BUFFER_NEEDFLUSH) == SDCARD_BUFFER_NEEDFLUSH)
		{
			try = 3;
			do
			{
				err = sdi_WriteBlock(SDCARDClst[i], SDCARDBuf[i]);
			}while (err && try--);
			if (err)
				return err;
			SDCARDFlag[i] &= ~SDCARD_BUFFER_NEEDFLUSH;
		}
		if (FreeBuffer)
		{
			SDCARDFlag[i] = 0x00;
			SDCARDClst[i] = 0xFFFFFFFF;
		}
	}
	return 0;
}

//sub function define
//
static int sdi_SDcardInit(void)
{
	int err;
	
    // Important notice for MMC test condition
    // Cmd & Data lines must be enabled pull up register
    hai_memset(&sdcard, 0x00, sizeof(sdcard));

    rSDIPRE    = PCLK/(2*SDI_INITIALCLK)-1;	// 400KHz
    rSDICON    = (1<<4)|(1<<1)|1;	// Type B, FIFO reset, clk enable
    rSDIBSIZE  = 0x200;		// 512byte(128word)
    rSDIDTIMER = 0xffff;		// Set timeout count

    for(err = 0; err < 0x1000; err++);  // Wait 74SDCLK for MMC card
	//CMD0: make idle
	if (err = sdi_SendCmd(SDICMD_GO_IDLE_STATE, 0))
		return err;
	//MMC=>CMD1;SD=>ACMD41: make ready
	if (!(err = sdi_ChkMMCOCR()))
		sdcard.CardType = SDT_MMC_CARD;
	else if (!(err = sdi_ChkSDOCR()))
		sdcard.CardType = SDT_SD_CARD;
	else
		return err;
	//CMD2: make identification
	if (err = sdi_MakeIdent())
		return err;
	//CMD3: make stand by
	if (sdcard.CardType == SDT_MMC_CARD)
		sdcard.RCA = 1;
	if (err = sdi_MakeStandby(&sdcard.RCA))//CMD3(MMC:Set RCA, SD:Ask RCA)
		return err;

	if (err = sdi_GetCID(sdcard.RCA, &sdcard.CID))
		return err;
	if (err = sdi_GetCSD(sdcard.RCA, &sdcard.CSD))
		return err;

	//normal clock
	rSDIPRE = PCLK/(2*SDI_NORMALCLK)-1;
	//CMD7: make transfer
	if (err = sdi_CardSelect(sdcard.RCA))
		return err;
	//ACMD6: set bus width
	if (sdcard.CardType != SDT_MMC_CARD)
	{
		sdcard.flag |= SD_BUSW_4BIT;
		if (err = sdi_SetBusWidth(sdcard.RCA, (sdcard.flag&SD_BUSW_MASK)?1:0))
			return err;
	}
	return 0;
}


static int sdi_SendCmd(int rCMD, int rARG)
{
	int state, err = -1;

	rSDICARG = rARG;
	rSDICCON = rCMD|SDICMD_START|SDICMD_HOST;
	
	if(rCMD&SDICMD_WTRSP)
	{
		do
		{
			state = rSDICSTA;
		}while(!((state&0x200)||(state&0x400)));    // Check cmd/rsp end
		
		if((rCMD&SDICMD_IDXMSK)==1 || (rCMD&SDICMD_IDXMSK)==9 || (rCMD&SDICMD_IDXMSK)==41)	// CRC no check
		{
			if((state&0xf00) != 0xa00)
			{
				if((state&0x400)==0x400)
					err = -1;	// Timeout error
			}
		}
		else
		{
			if((state&0x1f00) != 0xa00)
			{
				if((state&0x400)==0x400)
					err = -1;	// Timeout error
				else
					err = -2;	// crc fail
			}
		}
    }   
    else
	{
		do
		{
			state = rSDICSTA;
		}while((state&0x800)!=0x800);	// Check cmd end
    }

	rSDICSTA = state;// Clear cmd end state
	return err;
}

static int sdi_CMD55(S_WORD RCA)
{
	return sdi_SendCmd(SDICMD_WTRSP|SDICMD_APP_CMD, (int)RCA<<16);
}

static int sdi_ChkMMCOCR(void)
{
	int try;

	for(try = 0; try < 9; try++)
	{
		//0xffc000:(OCR:2.6V~3.6V)
		if (!sdi_SendCmd(SDICMD_S_RSP|SDICMD_WTRSP|SDICMD_OP_COND, 0xffc000))
		{
			if(rSDIRSP0 == 0x80ffc000) 
				return 0;	// Success
		}
	}
	return -1;
}

static int sdi_ChkSDOCR(void)
{
    int try, delay;

    for(try = 0; try < 9; try++)
    {
    	if (sdi_CMD55(0))    // Make ACMD
    		continue;
    	//0xff8000 : (OCR:2.7V~3.6V)
		if (!sdi_SendCmd(SDICMD_S_RSP|SDICMD_WTRSP|SDICMD_SD_APP_OP_COND, 0xff8000))
    	{
			if(rSDIRSP0 == 0x80ff8000) 
			    return 0;	// Success	    
		}
		for(delay = 0; delay < 1000; delay++); // Wait Card power up status
    }
	return -1;
}

static int sdi_MakeIdent(void)
{
	int err, try = 3;

	do
	{
		err = sdi_SendCmd(SDICMD_L_RSP|SDICMD_WTRSP|SDICMD_ALL_SEND_CID, 0);
	}while(err && try--);

	return err;
}

static int sdi_MakeStandby(S_WORD *pRCA)
{
	int err, try = 3, arg = (int)*pRCA;

	do
	{// CMD3(MMC:Set RCA, SD:Ask RCA-->SBZ)
		err = sdi_SendCmd(SDICMD_WTRSP|SDICMD_SEND_REL_ADDR, arg<<16);
	}while (err && try--);

	if (err)
		return err;

    if (*pRCA)//MMC
    	;
	else
		*pRCA = (rSDIRSP0&0xFFFF0000)>>16;

    if (rSDIRSP0&0x1e00 != 0x600)  //check if enter stand-by
		err = -1;

	return err;
}

static int sdi_GetCID(S_WORD RCA, SP_CID pCID)
{
	int err, try = 3, val;
	S_BYTE rsp[16];

	do
	{
		err = sdi_SendCmd(SDICMD_L_RSP|SDICMD_WTRSP|SDICMD_SEND_CID, (int)RCA<<16);
	}while(err && try--);
	
	if (err) return err;

	val = rSDIRSP0; HAI_WRITEDWORD(rsp+12, val);
	val = rSDIRSP1; HAI_WRITEDWORD(rsp+ 8, val);
	val = rSDIRSP2; HAI_WRITEDWORD(rsp+ 4, val);
	val = rSDIRSP3; HAI_WRITEDWORD(rsp+ 0, val);

	pCID->MID    = rsp[15];
	pCID->OID[1] = rsp[14];
	pCID->OID[0] = rsp[13];
	pCID->PNM[4] = rsp[12];
	pCID->PNM[3] = rsp[11];
	pCID->PNM[2] = rsp[10];
	pCID->PNM[1] = rsp[ 9];
	pCID->PNM[0] = rsp[ 8];
	pCID->PRV    = rsp[ 7];
	pCID->PSN[3] = rsp[ 6];
	pCID->PSN[2] = rsp[ 5];
	pCID->PSN[1] = rsp[ 4];
	pCID->PSN[0] = rsp[ 3];
	pCID->MDT[1] = rsp[ 2]&0x0F;
	pCID->MDT[0] = rsp[ 1];

	return err;
}

static int sdi_GetCSD(S_WORD RCA, SP_CSD pCSD)
{
	int err, try = 3, val;
	S_BYTE rsp[16];

	do
	{
		err = sdi_SendCmd(SDICMD_L_RSP|SDICMD_WTRSP|SDICMD_SEND_CSD, (int)RCA<<16);
	}while(err && try--);
	
	if (err) return err;

	val = rSDIRSP0; HAI_WRITEDWORD(rsp+12, val);
	val = rSDIRSP1; HAI_WRITEDWORD(rsp+ 8, val);
	val = rSDIRSP2; HAI_WRITEDWORD(rsp+ 4, val);
	val = rSDIRSP3; HAI_WRITEDWORD(rsp+ 0, val);

	sdcard.BlockLen = 512;//temp

	return err;
}

static int sdi_CardSelect(S_WORD RCA)
{
	int err, try = 3;

	do
	{
		err = sdi_SendCmd(SDICMD_WTRSP|SDICMD_SEL_DSEL_CARD, (int)RCA<<16);
	}while (err && try--);

	if(rSDIRSP0&0x1e00 != 0x800)
	    err = -1;

	return err;
}

static int sdi_SetBusWidth(S_WORD RCA, int BUSW)
{
	int err, try = 3;

	do
	{
		if (err = sdi_CMD55(RCA))
			continue;
		err = sdi_SendCmd(SDICMD_WTRSP|SDICMD_SET_BUS_WIDTH, BUSW<<1);
	}while (err && try--);

	return err;
}

static int sdi_ReadBlock(S_DWORD block, S_BYTE *OutBuf)
{
	int err, try = 3;
	int *pOut = (int *)OutBuf;
	int MaxRead = sdcard.BlockLen;
	int BusW = (sdcard.flag&SD_BUSW_MASK)?1:0;

	rSDICON |= (1<<1);// FIFO reset
	rSDIDCON = (1<<19)|(1<<17)|(BusW<<16)|(2<<12)|(1<<0);// Rx after cmd, blk, 4bit bus, Rx start, blk num

	do
	{
		err = sdi_SendCmd(SDICMD_WTRSP|SDICMD_READ_SINGLBLK, block);
	}while (err && try--);

	if (err) return err;

	for (try = 0; try < MaxRead; )
	{
		if((rSDIDSTA&0x20) == 0x20)
		{
		    rSDIDSTA = 0x20;
		    break;
		}
		err = rSDIFSTA;
		if((err&0x1000) == 0x1000)
		{
		    *pOut++ = rSDIDAT;
		    try += 4;
		}
	}

	err = sdi_CheckDATend();
    rSDIDSTA = 0x10;// Clear data Tx/Rx end

	return err;
}

static int sdi_WriteBlock(S_DWORD block, S_BYTE *InBuf)
{
 	int err, try = 3;
	int *pin = (int *)InBuf;
	int MaxWrite = sdcard.BlockLen;
	int BusW = (sdcard.flag&SD_BUSW_MASK)?1:0;

	rSDICON |= (1<<1);// FIFO reset
	rSDIDCON = (1<<20)|(1<<17)|(BusW<<16)|(3<<12)|(1<<0);// Tx after rsp, blk, 4bit bus, Tx start, blk num

	do
	{
		err = sdi_SendCmd(SDICMD_WTRSP|SDICMD_WRITE_SINGLBLK, block);
	}while (err && try--);

	for (try = 0; try < MaxWrite; )
	{
		err = rSDIFSTA;
		if((err&0x2000) == 0x2000) 
		{
		    rSDIDAT = *pin++;
		    try += 4;
		}
	}

	err = sdi_CheckDATend();
    rSDIDSTA = 0x10;// Clear data Tx/Rx end

	return err;
}

static int sdi_CheckDATend(void)
{
    int finish;

	do
	{
    	finish = rSDIDSTA;
    }while(!(finish&0x30));// Chek timeout or data end

    if((finish&0xfc) != 0x10)
    {
        rSDIDSTA = 0xec;// Clear error state
        return -1;
    }
    return 0;
}

static void sdi_IncUseCount(int BufNo)
{
	S_DWORD UseCnt = SDCARDFlag[BufNo]&0x0000FFFF;
	
	UseCnt = (UseCnt<0xFFFF)? (UseCnt+1):(UseCnt);//increase use count
	SDCARDFlag[BufNo] = (SDCARDFlag[BufNo]&0xFFFF0000)|(UseCnt&0x0000FFFF);
}

static int sdi_FindLeastUseCluster(void)
{
	int i = 0, m = 0;
	S_DWORD UseCnt = SDCARDFlag[i]&0x0000FFFF;
	
	for (i = 1; i < SDCARD_BUFFER_BLOCK_MAX; i++)
	{
		if (UseCnt > (SDCARDFlag[i]&0x0000FFFF))
		{
			UseCnt = SDCARDFlag[i]&0x0000FFFF;
			m = i;
		}
	}
	return m;
}


#endif

