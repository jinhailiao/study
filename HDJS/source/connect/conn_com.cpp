//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			conn_com.cpp
// Description:		serial connection
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-04-27  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "conn_com.h"
#include "osrelate.h"
#include "htrace.h"


C_COMCONN::C_COMCONN(S_CHANNEL &Channel):C_CONNECT(Channel)
{
	m_Comfd = -1;
}

C_COMCONN::~C_COMCONN()
{
	OnDisconnect();
}

S_BOOL C_COMCONN::OnConnect(void)
{
	if ((m_Comfd = InitCom()) < 0)
		return false;

	SetCom();
	return true;
}

#ifndef WIN32

#include <unistd.h>
#include <termios.h>
#include <errno.h>

S_BOOL C_COMCONN::ClearInBuffer(void)
{
	if (ConnectSuccessful() == true)
	{
		if (tcflush(m_Comfd, TCIFLUSH) == 0)
			return true;
	}
	return false;
}

S_BOOL C_COMCONN::ClearOutBuffer(void)
{
	if (ConnectSuccessful() == true)
	{
		if (tcflush(m_Comfd, TCOFLUSH) == 0)
			return true;
	}
	return false;
}

S_BOOL C_COMCONN::OnDisconnect(void)
{
	if (m_Comfd != -1)
	{
		close(m_Comfd);
		logwt<<"close fd="<<m_Comfd<<"..."<<endl;
	}
	m_Comfd = -1;
	return true;
}

S_BOOL C_COMCONN::ConnectSuccessful(void)
{
	return m_Comfd != -1;
}

int C_COMCONN::ReadData(S_BYTE *pData, int size)
{
	if (m_Comfd == -1)
		return -1;
#if 0
	int ReadBytes = read(m_Comfd, (char*)pData, size);
	if (ReadBytes < 0)
	{
		if (errno != EAGAIN)
			return -1;
		else
		{
			SleepMS(1000);
			ReadBytes = read(m_Comfd, (char*)pData, size);
		}
	}
#else
	struct timeval ReadTimeout;
	ReadTimeout.tv_sec = m_Channel.m_CharReadTimeoutMS / 1000;
	ReadTimeout.tv_usec = (m_Channel.m_CharReadTimeoutMS % 1000)*1000;
	fd_set fs_read;
	FD_ZERO(&fs_read);
	FD_SET(m_Comfd, &fs_read);
	if (select(m_Comfd + 1, &fs_read, NULL, NULL, &ReadTimeout) <= 0)
		return 0;
	int ReadBytes = read(m_Comfd, pData, size);
#endif

	return ReadBytes;	
}

int C_COMCONN::WriteData(S_BYTE *pData, int size)
{
	if (m_Comfd == -1)
		return -1;

	int WriteBytes = write(m_Comfd, (const char*)pData, size);
	if (WriteBytes > 0)
		tcdrain(m_Comfd);//不做出错处理，因为网络不支持tcdrain

	return WriteBytes;
}

int C_COMCONN::InitCom(void)
{
	if (m_Channel.m_PortSet.m_CommSet.m_PortNO < 1)
		return -1;
	
	char cDevName[15];
	sprintf(cDevName, "/dev/ttyS%d", m_Channel.m_PortSet.m_CommSet.m_PortNO-1);

	int fd = -1;
	if ((fd = open(cDevName, O_RDWR|O_NONBLOCK,0666)) < 0)
		return -1;

	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
	{
		close(fd);
		return -1;
	}
	logwt<<"Init com "<<cDevName<<" successful, fd="<<fd<<"."<<endl;
	return fd;
}

S_BOOL C_COMCONN::SetCom(void)
{
	struct termios ioCom;

	if (m_Comfd < 0)
		return false;

	S_COMMSET &ComSet = m_Channel.m_PortSet.m_CommSet;

//	tcgetattr(m_Comfd, &ioCom);
	memset(&ioCom, 0x00, sizeof(ioCom));

	ioCom.c_cflag &= ~PARENB;
	if (ComSet.m_Parity == COMM_PARITY_ODD)
		ioCom.c_cflag |= PARENB|PARODD;
	else if (ComSet.m_Parity == COMM_PARITY_EVEN)
		ioCom.c_cflag |= PARENB;
	
	ioCom.c_cflag &= ~CSTOPB;
	if (ComSet.m_StopBit == COMM_STOP_BIT2)
		ioCom.c_cflag |= CSTOPB;

	ioCom.c_cflag &= ~CSIZE;
	if (ComSet.m_DataBit == COMM_DATA_BIT7)
		ioCom.c_cflag |= CS7;
	else
		ioCom.c_cflag |= CS8;

	ioCom.c_iflag = 0;
	ioCom.c_lflag = 0;
	ioCom.c_oflag = 0;		
	ioCom.c_cflag |= CREAD|HUPCL|CLOCAL;
	ioCom.c_cc[VMIN] = 1;
	ioCom.c_cc[VTIME] = 0;
	
	int BaudAttr = GetBaudAttr(ComSet.m_Baud);
	cfsetispeed(&ioCom, BaudAttr);
	cfsetospeed(&ioCom, BaudAttr);

	tcsetattr(m_Comfd, TCSANOW, &ioCom);
	logwt<<"Set Com:"<<ComSet.m_PortNO<<","<<ComSet.m_Baud<<","<<ComSet.m_DataBit<<","<<ComSet.m_StopBit<<","<<ComSet.m_Parity<<"."<<endl;

	return true;
}

int C_COMCONN::GetBaudAttr(int Baud)
{
	int BaudAttr[] = {B115200,B57600,B38400, B19200, B9600, B4800, B2400, B1200,B600, B300};
	int BaudSpeed[] = {115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 600, 300};
	for (size_t i= 0;  i < (sizeof(BaudSpeed)/sizeof(BaudSpeed[0]));  i++)
	{
		if  (Baud == BaudSpeed[i])
			return BaudAttr[i];
	}
	return B19200;
}
	
#else

#include <Windows.h>
#define        XON 0x11
#define        XOFF 0x13

S_BOOL C_COMCONN::ClearInBuffer(void)
{
	return true;
}

S_BOOL C_COMCONN::ClearOutBuffer(void)
{
	return true;
}

S_BOOL C_COMCONN::OnDisconnect(void)
{
	if (m_Comfd != -1)
		CloseHandle((HANDLE)m_Comfd);
	m_Comfd = -1;
	return true;
}

S_BOOL C_COMCONN::ConnectSuccessful(void)
{
	return m_Comfd != -1;
}

int C_COMCONN::ReadData(S_BYTE *pData, int size)
{
	DWORD lrc;                                 ///纵向冗余校验
	DWORD endtime, nBytesRead = 0;                            /////////jiesuo
	static OVERLAPPED ol;
	int ReadNumber=0;    
	int numCount=0 ;                             //控制读取的数目
	DWORD dwErrorMask,nToRead;  
	COMSTAT comstat;    
	ol.Offset=0;                            ///相对文件开始的字节偏移量
	ol.OffsetHigh=0;                        ///开始传送数据的字节偏移量的高位字，管道和通信时调用进程可忽略。
	ol.hEvent=NULL;                         ///标识事件，数据传送完成时设为信号状态
	ol.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);    
	endtime=GetTickCount()+500;//GetTickCount()取回系统开始至此所用的时间(毫秒) 
	ClearCommError((HANDLE)m_Comfd,&dwErrorMask,&comstat);
	nToRead=min(size,comstat.cbInQue);  
	if(int(nToRead)<0) 
	{
		CloseHandle(ol.hEvent);
		return -1;
	}
	if(!ReadFile((HANDLE)m_Comfd,pData,nToRead,&nBytesRead,&ol))
	{    
		if((lrc=GetLastError())==ERROR_IO_PENDING)
		{
			///////////////////
			endtime=GetTickCount()+500;//GetTickCount()取回系统开始至此所用的时间(毫秒)
			while(!GetOverlappedResult((HANDLE)m_Comfd,&ol,&nBytesRead,FALSE))//该函数取回重叠操作的结果
			{
				if(GetTickCount()>endtime)
					break;
			}    
		}        
	}
	CloseHandle(ol.hEvent);
	return nBytesRead;    
}

int C_COMCONN::WriteData(S_BYTE *pData, int size)
{
	DWORD nBytesWrite = 0,endtime,lrc;
	static OVERLAPPED ol;
	DWORD dwErrorMask,dwError;
	COMSTAT comstat;
	ol.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	ol.Offset=0;              
	ol.OffsetHigh=0;
	ol.hEvent=NULL;               ///标识事件，数据传送完成时，将它设为信号状态
	ClearCommError((HANDLE)m_Comfd,&dwErrorMask,&comstat);
	if(!WriteFile((HANDLE)m_Comfd,pData,size,&nBytesWrite,&ol)) 
	{
		if((lrc=GetLastError())==ERROR_IO_PENDING)
		{
			endtime=GetTickCount()+1000;
			while(!GetOverlappedResult((HANDLE)m_Comfd,&ol,&nBytesWrite,FALSE))
			{   
				dwError=GetLastError();
				if(GetTickCount()>endtime)
				{    
					break;
				}
				if(dwError=ERROR_IO_INCOMPLETE) 
					continue;          //未完全读完时的正常返回结果 
				else
				{
					//    发生错误，尝试恢复！
					ClearCommError((HANDLE)m_Comfd,&dwError,&comstat);
					break;
				}
			}
		}      
	}    
	FlushFileBuffers((HANDLE)m_Comfd);
	PurgeComm((HANDLE)m_Comfd,PURGE_TXCLEAR);        
	CloseHandle(ol.hEvent);
	return nBytesWrite;
}

int C_COMCONN::InitCom(void)
{
	if (m_Channel.m_PortSet.m_CommSet.m_PortNO < 1)
		return -1;
	
	char sDevName[32];
	sprintf(sDevName,"COM%d",m_Channel.m_PortSet.m_CommSet.m_PortNO);
	HANDLE hComm = CreateFile(sDevName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,NULL); // 以重叠方式打开串口
	if(hComm == INVALID_HANDLE_VALUE)
	{
		sprintf(sDevName, "COM%d", 1);//在Windows中强制改为1
		hComm = CreateFile(sDevName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,NULL); // 以重叠方式打开串口
		if(hComm == INVALID_HANDLE_VALUE)
			return -1;
	}
	return (int)hComm;
}

S_BOOL C_COMCONN::SetCom(void)
{
	DCB dcb;                                              ///与端口匹配的设备    
	COMMTIMEOUTS TimeOuts;                                ///串口输出时间 超时设置

	SetupComm((HANDLE)m_Comfd, 2048, 2048);              //设置缓冲区
	memset(&TimeOuts, 0, sizeof(TimeOuts));    
	TimeOuts.ReadIntervalTimeout = MAXDWORD;           // 把间隔超时设为最大，把总超时设为0将导致ReadFile立即返回并完成操作
	TimeOuts.ReadTotalTimeoutMultiplier = 0;           //读时间系数
	TimeOuts.ReadTotalTimeoutConstant = 0;            //读时间常量  
	TimeOuts.WriteTotalTimeoutMultiplier = 50;       //总超时=时间系数*要求读/写的字符数+时间常量
	TimeOuts.WriteTotalTimeoutConstant = 2000;       //设置写超时以指定WriteComm成员函数中的                                              
	SetCommTimeouts((HANDLE)m_Comfd, &TimeOuts);           //GetOverlappedResult函数的等待时间*/
	if(!GetCommState((HANDLE)m_Comfd, &dcb))               ////串口打开方式、端口、波特率 与端口匹配的设备
	{
//		loget<<"GetCommState Failed"<<endl;
		return FALSE;
	}

	dcb.fParity=TRUE;                          //允许奇偶校验        
	dcb.fBinary=TRUE;
	if(m_Channel.m_PortSet.m_CommSet.m_Parity == COMM_PARITY_NO)
		dcb.Parity = NOPARITY;
	else if(m_Channel.m_PortSet.m_CommSet.m_Parity == COMM_PARITY_ODD)
		dcb.Parity = ODDPARITY;
	else //(Parity==2)
		dcb.Parity = EVENPARITY;
	if(m_Channel.m_PortSet.m_CommSet.m_StopBit == COMM_STOP_BIT1)
		dcb.StopBits=ONESTOPBIT;
	else //(stopbit=="2")
		dcb.StopBits=TWOSTOPBITS;

	BOOL m_bEcho=FALSE;                        ///
	int m_nFlowCtrl=0;
	BOOL m_bNewLine=FALSE;                     ///
	dcb.BaudRate = m_Channel.m_PortSet.m_CommSet.m_Baud;                     // 波特率
	dcb.ByteSize = m_Channel.m_PortSet.m_CommSet.m_DataBit;                     // 每字节位数    
	// 硬件流控制设置
	dcb.fOutxCtsFlow = (m_nFlowCtrl==1);
	dcb.fRtsControl = (m_nFlowCtrl==1?RTS_CONTROL_HANDSHAKE:RTS_CONTROL_ENABLE);    
	// XON/XOFF流控制设置（软件流控制！）
	dcb.fInX=dcb.fOutX = (m_nFlowCtrl==2);
	dcb.XonChar = XON;
	dcb.XoffChar = XOFF;
	dcb.XonLim = 50;
	dcb.XoffLim = 50;    
	if(SetCommState((HANDLE)m_Comfd, &dcb))     
		return TRUE;          ////com的通讯口设置    
	else
		return FALSE;
}

#endif

