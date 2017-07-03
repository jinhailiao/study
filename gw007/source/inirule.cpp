

#include "inirule.h"
#include "confuse.h"
#include "string.h"

S_AFN *S_RULE::find(void)
{
	if (m_AfnCnt >= CFG_AFN_MAX)
        return NULL;
	return &m_AfnItem[m_AfnCnt++];
}

S_RULES::S_RULES(void)
{
	memset(this, 0x00, sizeof(*this));
}

S_RULE *S_RULES::find(const char *pNameID)
{
	for (int i = 0; i < m_RuleCnt; i++)
	{
		if (strcmp(m_RuleItem[i].m_ID, pNameID) == 0)
			return &m_RuleItem[i];
	}
	return NULL;
}

S_RULE *S_RULES::find(void)
{
	if (m_RuleCnt >= CFG_RULE_MAX)
        return NULL;
	return &m_RuleItem[m_RuleCnt++];
}

cfg_opt_t OPT_RULE_SETTINGS[] =
{
	CFG_STR("Name","广西集抄规约", CFGF_NOCASE),
	CFG_STR("ID","GXJC", CFGF_NOCASE),
	CFG_INT("PwdLen", 2, CFGF_NOCASE),
	CFG_INT("Protocol", 1, CFGF_NOCASE),
	CFG_END()
};

cfg_opt_t OPT_AFN_SETTINGS[] =
{
	CFG_STR("Name","复位命令", CFGF_NOCASE),
	CFG_INT("ID", 1, CFGF_NOCASE),
	CFG_STR("Rule","GXJC", CFGF_NOCASE),
	CFG_INT("FunCode", 1, CFGF_NOCASE),
	CFG_BOOL("PW", cfg_false, CFGF_NOCASE),
	CFG_END()
};

cfg_opt_t OPT_TASKTEMPS_SETTINGS[] =
{
	CFG_SEC("Rule", OPT_RULE_SETTINGS, CFGF_MULTI | CFGF_NOCASE),
	CFG_SEC("AFN", OPT_AFN_SETTINGS, CFGF_MULTI | CFGF_NOCASE),
	CFG_END()
};

int LoadMasterRule(S_RULES &rules, const char *pfilename)
{
	cfg_t* cfg = cfg_init(OPT_TASKTEMPS_SETTINGS, CFGF_NONE);
	if (cfg_parse(cfg, pfilename) != CFG_SUCCESS)
	{
		cfg_free(cfg);
		return -1;
	}

	cfg_t* cfgRule;
	for(unsigned int i = 0; i < cfg_size(cfg, "Rule") && i < CFG_RULE_MAX; i++)
	{
		cfgRule = cfg_getnsec(cfg, "Rule", i);
		S_RULE *pRule = rules.find(cfg_getstr(cfgRule, "ID"));
		if (pRule == NULL)
			pRule = rules.find();
		pRule->m_Enable = true;
		strcpy(pRule->m_ID, cfg_getstr(cfgRule, "ID"));
		strcpy(pRule->m_Name, cfg_getstr(cfgRule, "Name"));
		pRule->m_Protocol = cfg_getint(cfgRule, "Protocol");
		pRule->m_PwdLen = cfg_getint(cfgRule, "PwdLen");
	}
	for(unsigned int i = 0; i < cfg_size(cfg, "AFN"); i++)
	{
		cfgRule = cfg_getnsec(cfg, "AFN", i);
		S_RULE *pRule = rules.find(cfg_getstr(cfgRule, "Rule"));
		if (pRule == NULL)
			continue;
		S_AFN *pAFN = pRule->find();
		if (pAFN == NULL)
			continue;
		pAFN->m_Enable = true;
		pAFN->m_FunCode = cfg_getint(cfgRule, "FunCode");
		pAFN->m_ID = cfg_getint(cfgRule, "ID");
		pAFN->m_PW = cfg_getbool(cfgRule, "PW")==cfg_true? true:false;
		strcpy(pAFN->m_Name, cfg_getstr(cfgRule, "Name"));
	}

	cfg_free(cfg);
	return 0;
}

