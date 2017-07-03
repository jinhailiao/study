//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			channel.cpp
// Description:		communication channel setting
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-23  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "channel.h"

S_CHANNEL::S_CHANNEL(void)
{
	memset(this, 0x00, sizeof(*this));
}

S_CHANNEL::S_CHANNEL(S_CHANNEL &r)
{
	if (this != &r)
		memcpy(this, &r, sizeof(*this));
}

S_CHANNEL::~S_CHANNEL()
{
}

S_CHANNEL & S_CHANNEL::operator=(S_CHANNEL &r)
{
	if (this != &r)
		memcpy(this, &r, sizeof(*this));

	return *this;
}

S_BOOL S_CHANNEL::NeedPPP(void)
{
	return m_Type==CHANNEL_TYPE_NET && m_PortSet.m_NetSet.m_NeedPPP==NET_CONNECT_PPP_YES;
}
