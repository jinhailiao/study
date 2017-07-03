//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			message.cpp
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-01-13  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "message.h"
#include "haicfg.h"
#include "htrace.h"

#ifndef WIN32
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#endif

C_MESSAGE::C_MESSAGE(char QFlag)
{
	m_MsgQID = -1;
#ifndef WIN32
	key_t key = ftok(CFG_FILE_PROG_PATH, QFlag);
	if (key != -1)
		m_MsgQID = msgget(key,IPC_CREAT|00666);
#endif

	if(m_MsgQID == -1)
		printf("Create message queue error: %s.\n", strerror(errno));
}

C_MESSAGE::~C_MESSAGE()
{
#ifndef WIN32
	if (m_MsgQID != -1)
	{
		if (msgctl(m_MsgQID,IPC_RMID, NULL) == -1)
			printf("Unlink message queue error:%s.\n", strerror(errno));
	}
#endif
	m_MsgQID=-1;
}

struct S_MSGBUF
{
	int mtype;
	char mtext[C_MESSAGE::TEXT_BUF_MAX];
};

int C_MESSAGE::SendMsg(int MsgT, string &msg)
{
	int err = -1;

#ifndef WIN32
	struct S_MSGBUF MsgBuf;

	if (m_MsgQID == -1 || msg.size() >= sizeof(MsgBuf.mtext))
	{
		if (msg.size() >= sizeof(MsgBuf.mtext))
			printf("Message is too big!!!\n");
		return err;
	}
	MsgBuf.mtype = MsgT;
	memset(MsgBuf.mtext, 0x00, sizeof(MsgBuf.mtext));
	memcpy(MsgBuf.mtext, msg.data(), msg.size());

	if ((err = msgsnd(m_MsgQID, &MsgBuf, msg.size(),IPC_NOWAIT)) == -1)
	{
		printf("Message send failed!!!\n");
		ClearMsg(MsgT);
		err = msgsnd(m_MsgQID, &MsgBuf, msg.size(),IPC_NOWAIT);
	}

#endif
	return err;
}

int C_MESSAGE::RecvMsg(int MsgT, string &msg)
{
	int err = -1;

#ifndef WIN32
	struct S_MSGBUF MsgBuf;

	if (m_MsgQID == -1)
		return err;

	MsgBuf.mtype = MsgT;
	memset(MsgBuf.mtext, 0x00, sizeof(MsgBuf.mtext));

	err = msgrcv(m_MsgQID, &MsgBuf, sizeof(MsgBuf.mtext), MsgT, IPC_NOWAIT);
	if (err == -1)
	{
		if (errno == E2BIG)
		{
			printf("Message is too big!!!\n");
			msgrcv(m_MsgQID, &MsgBuf, sizeof(MsgBuf.mtext), MsgT, IPC_NOWAIT|MSG_NOERROR);
		}
	}
	else
	{
		msg.append(MsgBuf.mtext, err);
	}
#endif

	return err;
}

int C_MESSAGE::ClearMsg(int MsgT)
{
	string msg;

	return RecvMsg(MsgT, msg);
	return 0;
}

