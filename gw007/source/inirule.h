#ifndef __INI_RULE_H__
#define __INI_RULE_H__

struct S_AFN
{
	bool m_Enable;
	char m_Name[64];
	int m_ID;
	int m_FunCode;
	bool m_PW;
};

#define CFG_AFN_MAX	64
struct S_RULE
{
	bool m_Enable;
	char m_Name[64];
	char m_ID[12];
	int m_PwdLen;
	int m_Protocol;
	int m_AfnCnt;
	S_AFN m_AfnItem[CFG_AFN_MAX];

public:
	S_AFN *find(void);
};

#define CFG_RULE_MAX 12
struct S_RULES
{
	int m_RuleCnt;
	S_RULE m_RuleItem[CFG_RULE_MAX];

public:
	S_RULES(void);
	S_RULE *find(const char *pNameID);
	S_RULE *find(void);
};

int LoadMasterRule(S_RULES &rules, const char *pfilename);


#endif //__INI_RULE_H__

