//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			conn_net.h
// Description:		net connection
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-04-27  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __CONN_NET_H__
#define __CONN_NET_H__

#include "haidef.h"
#include "connect.h"

#ifdef WIN32
	#include <winsock.h>
	#pragma comment(lib, "wsock32.lib")
#else
	#define SOCKET int
#endif

class C_NETCONN: public C_CONNECT
{
public:
	C_NETCONN(S_CHANNEL &channel);
	~C_NETCONN();

protected:
	S_BOOL OnConnect(void);
	S_BOOL OnDisconnect(void);
	S_BOOL ConnectSuccessful(void);
	int ReadData(S_BYTE *pData, int size);
	int WriteData(S_BYTE *pData, int size);
	int WriteDataDelay(int SendByte);

	int TcpWriteData(S_BYTE *pData, int size);
	int UdpWriteData(S_BYTE *pData, int size);

protected:
	void CloseSocket(SOCKET socket);
	SOCKET SocketConnect(const S_CHAR* pDstIP, S_WORD DstPort, int timeoutS);
	SOCKET SocketAccept(SOCKET sockfd, struct sockaddr_in& RemoteAddr, int timeoutS);
	SOCKET SocketListen(S_WORD DstPort);

	int SetKeepAlive(SOCKET sockfd);

protected:
	SOCKET m_CurSocket;
};

#endif //__CONN_NET_H__


