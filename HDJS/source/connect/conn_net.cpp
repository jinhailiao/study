//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			conn_net.cpp
// Description:		net connection
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-04-27  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "conn_net.h"
#include "htrace.h"
#include "appbase.h"
#ifndef WIN32
#include <sys/types.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <termios.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#endif

C_NETCONN::C_NETCONN(S_CHANNEL &channel):C_CONNECT(channel)
{
	m_CurSocket = -1;
}

C_NETCONN::~C_NETCONN()
{
	OnDisconnect();
}

S_BOOL C_NETCONN::OnConnect(void)
{
	if (m_Channel.m_PortSet.m_NetSet.m_Type == NET_CONNECT_TCP_CLIENT)
	{
		S_CHAR DstIP[32];
		m_Channel.m_PortSet.m_NetSet.GetStrIP(DstIP);
		logit<<m_Channel.m_Name<<":Begin to connect("<<DstIP<<":"<<m_Channel.m_PortSet.m_NetSet.m_PortNO<<") in "<<m_Channel.m_ConnectTimeoutS<<" seconds."<<endl;
		m_CurSocket = SocketConnect(DstIP, m_Channel.m_PortSet.m_NetSet.m_PortNO, m_Channel.m_ConnectTimeoutS);
		if (m_CurSocket == -1)
			logit<<m_Channel.m_Name<<":Connect to "<<DstIP<<":"<<m_Channel.m_PortSet.m_NetSet.m_PortNO<<" failed."<<endl;
		else
			logit<<m_Channel.m_Name<<":Connect to "<<DstIP<<":"<<m_Channel.m_PortSet.m_NetSet.m_PortNO<<" Succeed(sockfd="<<(int)m_CurSocket<<")."<<endl;
	}
	else if (m_Channel.m_PortSet.m_NetSet.m_Type == NET_CONNECT_TCP_SERVER)
	{
		SOCKET ListenSocket = SocketListen(m_Channel.m_PortSet.m_NetSet.m_PortNO);
		if (ListenSocket != -1)
		{
			struct sockaddr_in addr;
			m_CurSocket = SocketAccept(ListenSocket, addr, m_Channel.m_ConnectTimeoutS);
			CloseSocket(ListenSocket);
		}
	}

	return m_CurSocket != -1;
}

S_BOOL C_NETCONN::OnDisconnect(void)
{
	if (m_CurSocket != -1)
		CloseSocket(m_CurSocket);
	m_CurSocket = -1;
	return true;
}

S_BOOL C_NETCONN::ConnectSuccessful(void)
{
	return m_CurSocket != -1;
}

int C_NETCONN::ReadData(S_BYTE *pData, int size)
{
	if (m_CurSocket == -1)
		return -1;

	fd_set fs_read;
	struct timeval timeout;

	timeout.tv_sec  = m_Channel.m_CharReadTimeoutMS / 1000;
	timeout.tv_usec = (m_Channel.m_CharReadTimeoutMS % 1000)*1000;
	FD_ZERO(&fs_read);
	FD_SET(m_CurSocket, &fs_read);

	if (select((int)m_CurSocket + 1, &fs_read, NULL, NULL, &timeout) <= 0)
		return 0;

#ifndef WIN32
	return read(m_CurSocket, (char *)pData, size);
#else
	return recv(m_CurSocket, (char *)pData, size, 0);
#endif
}

int C_NETCONN::WriteData(S_BYTE *pData, int size)
{
	if (m_Channel.m_PortSet.m_NetSet.m_Type == NET_CONNECT_UDP
		|| m_Channel.m_PortSet.m_NetSet.m_Type == NET_CONNECT_UDP_SERVER
		|| m_Channel.m_PortSet.m_NetSet.m_Type == NET_CONNECT_UDP_CLIENT)
	{
//		string ipStr;
//		m_Channel.Setting.Net.GetIP(ipStr);
//		return UdpOnWriteData(ipStr.c_str(),m_Channel.Setting.Net.PortNo, ipSendData,iiCount);
	}	
	else
	{
		return TcpWriteData(pData, size);
	}
	return 0;
}

int C_NETCONN::TcpWriteData(S_BYTE *pData, int size)
{
	if (m_CurSocket == -1)
		return -1;

#ifndef WIN32
	int ret = write(m_CurSocket, pData, size);
	if (ret>0)
	{
		tcdrain(m_CurSocket);
	}
	return ret;
#else
	return send(m_CurSocket, (char *)pData, size, 0);
#endif
}

int C_NETCONN::WriteDataDelay(int SendByte)
{
	if (m_Channel.NeedPPP())
	{
		C_MSTIMOUT ts(SendByte*5);
		while (ts.timout() == false)
			SleepMS(10);
	}
 
	return 0;
}

void C_NETCONN::CloseSocket(SOCKET socket)
{
#ifdef WIN32
	closesocket(socket);
#else
	close(socket);
#endif
}

SOCKET C_NETCONN::SocketConnect(const S_CHAR* pDstIP, S_WORD DstPort, int timeoutS)
{
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD( 2, 2 );
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		loget<<m_Channel.m_Name<<":WSAStartup failed."<<endl;
		return -1;
	}
#endif

	SOCKET sockfd;
	struct sockaddr_in inaddr;
	struct hostent *he;
	
	if((he = gethostbyname(pDstIP)) == NULL)
	{
		loget<<m_Channel.m_Name<<":Get host by name from "<<pDstIP<<" failed ...."<<endl;
		return -1;
	}
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		loget<<m_Channel.m_Name<<":Create socket failed."<<endl;
		return -1;
	}

	memset(&inaddr, 0x00, sizeof(inaddr));
	inaddr.sin_family = AF_INET;
	inaddr.sin_port = htons(DstPort);
	inaddr.sin_addr = *((struct in_addr *)he->h_addr);

#ifdef WIN32
	if (connect(sockfd, (struct sockaddr *)&inaddr, sizeof(struct sockaddr)) != 0)
	{
		CloseSocket(sockfd);
		return -1;
	}
#else
	fcntl(sockfd, F_SETFL, O_NONBLOCK); 
	if (connect(sockfd, (struct sockaddr *)&inaddr, sizeof(struct sockaddr)) != 0)
	{
		if (errno == EINPROGRESS)// it is in the connect process 
		{
			struct timeval tv; 
			fd_set writefds; 
			tv.tv_sec = timeoutS; 
			tv.tv_usec = 0; 
			FD_ZERO(&writefds); 
			FD_SET(sockfd, &writefds); 
			if(select((int)sockfd+1, NULL, &writefds, NULL, &tv) <= 0)
			{
				CloseSocket(sockfd);
				return -1;
			}
		}
		else
		{
			CloseSocket(sockfd);
			return -1;
		}
	}
#endif

	int error;
#ifdef WIN32
	int len = sizeof(error); 
#else
	socklen_t len = sizeof(error); 
#endif
	//下面的一句一定要，主要针对防火墙 
	getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char *)&error, &len); 
	if (error != 0)
	{
		CloseSocket(sockfd);
		return -1;
	}
	SetKeepAlive(sockfd);

	return sockfd;
}

SOCKET C_NETCONN::SocketAccept(SOCKET sockfd, struct sockaddr_in& RemoteAddr, int timeoutS)
{
	return -1;
}

SOCKET C_NETCONN::SocketListen(S_WORD DstPort)
{
	return -1;
}

int C_NETCONN::SetKeepAlive(SOCKET sockfd)
{
	int keepAlive = 6;
	int keepInterval = 5;
	int keepCount = 3;

#ifdef WIN32
#else
	if (setsockopt(sockfd,SOL_SOCKET,SO_KEEPALIVE,(void*)&keepAlive,sizeof(keepAlive))==-1)
	{
		loget<<"Set SockOpt SO_KEEPALIVE failed on fd "<<(int)sockfd<<endl;
		return -1;
	}
	if (setsockopt(sockfd,SOL_TCP,TCP_KEEPINTVL,(void*)&keepInterval,sizeof(keepInterval))==-1)
	{
		loget<<"Set SockOpt TCP_KEEPINTVL failed on fd "<<(int)sockfd<<endl;
		return -1;
	}
	if (setsockopt(sockfd,SOL_TCP,TCP_KEEPCNT,(void*)&keepCount,sizeof(keepCount))==-1)
	{
		loget<<"Set SockOpt TCP_KEEPCNT failed on fd "<<(int)sockfd<<endl;
		return -1;
	}
#endif
	logit<<m_Channel.m_Name<<":Set KeepAlive Option(Interval="<<keepInterval<<",Count="<<keepCount<<")for fd "<<(int)sockfd<<" succeeded!"<<endl;
	return 0;
}
