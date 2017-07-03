//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			sysipc.cpp
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-01-13  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "haidef.h"
#include "sysipc.h"
#include "server.h"

C_MESSAGE C_SYSIPC::m_MsgObj;
S_BYTE C_SYSIPC::ReqSeq = 0;
S_BYTE C_SYSIPC::ResSeq = 0;

int C_SYSIPC::Send2Server(int SerID, S_BYTE MsgT, const string &msg)
{
	int SelfID = C_SERVER::GetCurrentServID();
	string MSG;

	ReqSeq += 1;

	MSG.push_back(MsgT);
	MSG.push_back(ReqSeq);
	MSG.append((S_CHAR*)&SelfID, sizeof(SelfID));
	MSG.append(msg);

	logit<<"Send "<<C_SERVER::GetServerName(SerID)<<" msg:"<<HEX<<msg<<"."<<endl;

	return m_MsgObj.SendMsg(SerID+MTYPE_BASE_VAL, MSG);
}

int C_SYSIPC::Recv4Server(S_BYTE MsgT, string &msg)
{
	string MSG;
	if (m_MsgObj.RecvMsg(C_SERVER::GetCurrentServID()+MTYPE_BASE_VAL, MSG) >= 6)
	{
		if (MSG[0] != MsgT || MSG[1] != ReqSeq)
		{
			loget<<"Recv not expected msg:"<<HEX<<MSG<<"."<<endl;
			return -1;
		}
		int SerID;
		copy(MSG.begin()+2, MSG.begin()+2+sizeof(SerID), &SerID);
		msg.append(MSG.begin()+6, MSG.end());
		logit<<"Recv "<<C_SERVER::GetServerName(SerID)<<" msg:"<<HEX<<msg<<"."<<endl;
		return 0;
	}
	return -1;
}

int C_SYSIPC::Wait4Server(S_BYTE MsgT, string &msg)
{
	for (int i = 0; i < SYSIPC_TIMEOUT_S; ++i)
	{
		if (Recv4Server(MsgT, msg) == 0)
			return 0;
		SleepSecond(1);
	}
	return -1;
}

int C_SYSIPC::Query4Server(int &SerID, S_BYTE &MsgT, string &msg)
{
	string MSG;
	if (m_MsgObj.RecvMsg(C_SERVER::GetCurrentServID()+MTYPE_BASE_VAL, MSG) >= 6)
	{
		MsgT = MSG[0];
		ResSeq = MSG[1];
		copy(MSG.begin()+2, MSG.begin()+2+sizeof(SerID), &SerID);
		msg.append(MSG.begin()+6, MSG.end());
		logit<<"Query "<<C_SERVER::GetServerName(SerID)<<" msg:"<<HEX<<msg<<"."<<endl;
		return 0;
	}
	return -1;
}

int C_SYSIPC::Reply2Server(int SerID, S_BYTE MsgT, const string &msg)
{
	int SelfID = C_SERVER::GetCurrentServID();
	string MSG;

	MSG.push_back(MsgT);
	MSG.push_back(ResSeq);
	MSG.append((S_CHAR*)&SelfID, sizeof(SelfID));
	MSG.append(msg);

	logit<<"Reply "<<C_SERVER::GetServerName(SerID)<<" msg:"<<HEX<<msg<<"."<<endl;

	return m_MsgObj.SendMsg(SerID+MTYPE_BASE_VAL, MSG);
}


