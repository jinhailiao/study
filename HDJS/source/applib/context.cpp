//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			context.cpp
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-13  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "context.h"
#include "cfgfile.h"
#include "haiver.h"


bool C_ContextOP::m_ContextNew = false;///< only use for daemon
bool C_ContextOP::m_StartAllServer = false;///< only use for daemon
SP_CONTEXT C_ContextOP::m_pContext = NULL;
C_SHAREMEM C_ContextOP::m_ShareMem;



/** @fn S_SYSCFG::SetRTUA
 *  @brief 
 *  @param[in] pRTUA:
 *  @note 
 */
void S_SYSCFG::SetRTUA(const char *pRTUA)
{
	if (strlen(pRTUA) >= 8)
	{
		m_RTUA[0] = ((pRTUA[0]-'0') << 4) + ((pRTUA[1]-'0') & 0x0F);// convert to BCD
		m_RTUA[1] = ((pRTUA[2]-'0') << 4) + ((pRTUA[3]-'0') & 0x0F);
		m_RTUA[2] = (Char2Hex(pRTUA[4]) << 4) + (Char2Hex(pRTUA[5]) & 0x0F);// convert to HEX
		m_RTUA[3] = (Char2Hex(pRTUA[6]) << 4) + (Char2Hex(pRTUA[7]) & 0x0F);
	}
}

/** @fn S_SYSCFG::GetRTUAstr
 *  @brief 
 *  @param[out] pRTUA:
 *  @note 
 *  @return NULL:
 */
char *S_SYSCFG::GetRTUAstr(char *pRTUA)
{
	if (pRTUA == NULL)
		return NULL;

	for (int i = 0; i < 2; i++)
	{
		pRTUA[i*2+0] = ((m_RTUA[i]>>4)&0x0F) + '0';//BCD convert to ASCII
		pRTUA[i*2+1] = ((m_RTUA[i]>>0)&0x0F) + '0';
	}
	for (int i = 2; i < 4; i++)
	{
		pRTUA[i*2+0] = Hex2Char((m_RTUA[i]>>4)&0x0F);//HEX convert to ASCII
		pRTUA[i*2+1] = Hex2Char((m_RTUA[i]>>0)&0x0F);
	}

	return pRTUA;
}


bool S_MPS::RegisterMP(int MP, S_WORD DevT, S_WORD DevSN)
{
	if (m_MP[MP].GetMpAttr() != S_MP::MP_ATTR_NONE)
		return false;

	S_MP aMP(DevT, DevSN);
	m_MP[MP] = aMP;

	return true;
}

bool S_MPS::UnregisterMP(int MP, S_WORD DevT, S_WORD DevSN)
{
	S_MP NullMP(S_MP::MP_ATTR_NONE, 0);

	if (DevT == 0xFFFF || DevSN == 0xFFFF)
		m_MP[MP] = NullMP;
	else if (m_MP[MP].GetMpAttr() == DevT && m_MP[MP].GetMpDevSN() == DevSN)
		m_MP[MP] = NullMP;
	else
		return false;
	return true;
}

bool _tagContext::init(void)
{
	memset(this, 0x00, sizeof(*this));
	m_ContextSign = GetSoftVersion();
	m_SysConfig.Init();

	for (int i = HDJS_SERID_DAEMON; i < HDJS_SERID_MAX; i++)
	{
		m_TraceSet[i] = S_TRACESET((S_BYTE *)C_SERVER::GetServerName(i));
		m_ServInfo[i] = S_SERVINFO((S_BYTE)i);
	}

	m_DaemonInfo = S_DAEMONINFO(&m_ServInfo[HDJS_SERID_DAEMON]);
	m_PppInfo    = S_PPPINFO(&m_ServInfo[HDJS_SERID_PPP]);
	m_HttxInfo[HTTX_ID_1]  = S_HTTXINFO(&m_ServInfo[HDJS_SERID_HTTX1]);
	m_HttxInfo[HTTX_ID_2]  = S_HTTXINFO(&m_ServInfo[HDJS_SERID_HTTX2]);
	m_HttxInfo[HTTX_ID_3]  = S_HTTXINFO(&m_ServInfo[HDJS_SERID_HTTX3]);

	m_ConnectStat.Init();

	return true;
}

//
//
//
bool C_ContextOP::InitContext(void)
{
	if (m_ShareMem.Create(CFG_FILE_CONTEXT_FILE, sizeof(S_CONTEXT)) == false)
	{
		loget<<"Shared file \""<<CFG_FILE_CONTEXT_FILE<<"\""<<" create failed!!!"<<endl;
		return false;
	}
	m_pContext = (SP_CONTEXT)m_ShareMem.GetShareMem();
	C_MEMSIGN memsign(GetSoftVersion());
#ifndef WIN32
	if (m_pContext->m_ContextSign == memsign)
		return true;
#endif
	logit<<"CONTEXT will init...."<<endl;
	m_pContext->init();
	if (LoadConfig() == false)
	{
		loget<<"Important config loads failed!!!"<<endl;
		return false;
	}
	ParamParse();
	//OK, Flush it
	SaveContext();
	SetContextNewest();
	logit<<"CONTEXT load OK."<<endl;
	return true;
}

S_CONTEXT& C_ContextOP::instance(void)
{
	return *m_pContext;
}

S_CONTEXT* C_ContextOP::GetContext(void)
{
	return m_pContext;
}

bool C_ContextOP::SaveContext(void)
{
	return m_ShareMem.Save();
}

bool C_ContextOP::SaveContext(void *pstart, int size)
{
	return m_ShareMem.Save(pstart, size);
}

bool C_ContextOP::SetAllServerActive(void)
{
	for (int i = HDJS_SERID_DAEMON; i < HDJS_SERID_MAX; i++)
		m_pContext->m_ServInfo[i].SetActive();
	return true;
}

bool C_ContextOP::LoadConfig(void)
{
	C_CFGSYSTEM::Load(m_pContext->m_SysConfig);

	C_CFGTRACE::Load(m_pContext->m_TraceSet);
	C_CFGUCHNNL::Load(m_pContext->m_UpChannel, CHANNEL_UP_TOTAL_MAX);
	C_CFGDCHNNL::Load(m_pContext->m_DnChannel, CHANNEL_DN_TOTAL_MAX);
	C_CFGHTTX::Load(m_pContext->m_HttxInfo, HTTX_ID_MAX);
	C_CFGPPP::Load(m_pContext->m_PppInfo);

	C_CFGFORWARD::Load(m_pContext->m_forward);

	return true;
}

bool C_ContextOP::ParamParse(void)
{
	logit<<"Parameter parser start......"<<endl;
	//parse parameter
	//alarm for invalid parameter
	return true;
}

S_CHANNEL* C_ContextOP::FindChannel(S_CHANNEL *pChannel, int size, const char *pChannelName)
{
	for (int i = 0; i < size; ++i)
	{
		if (strcasecmp((char *)pChannel[i].m_Name, pChannelName) == 0)
			return &pChannel[i];
	}
	return NULL;
}

