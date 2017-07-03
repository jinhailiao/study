//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2015
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			command.h
// Description:		command manager
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-11  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "haidef.h"
#include "osrelate.h"

struct _CMD; 
struct _CMD_OBJ;

typedef int fnCmdProc(struct _CMD *pCmd, struct _CMD_OBJ *pCmdObj, char *pArg);
typedef int fnBeforeCmd(void);
typedef int fnAfterCmd(void);
//命令对象
typedef struct _CMD_OBJ
{
	char *m_pName;//名称，例如httx,dbcj,task
	fnCmdProc *m_pfnCmdProc;
	char *m_pHelp;
}CMD_OBJ;


typedef struct _CMD
{
	char *m_pCmdName;
	CMD_OBJ *m_pObjects;
	fnBeforeCmd *m_pfnBeforeCmd;
	fnAfterCmd *m_pfnAfterCmd;
	char *m_pHelp;
}CMD;


#define BEGIN_CMD_OBJ_MAP(x) static CMD_OBJ x[] = {
#define END_CMD_OBJ_MAP()   {NULL, NULL, NULL}};
#define CMD_OBJ_ENTRY(ObjName, fnCmdProc, Help) {ObjName, fnCmdProc, Help},

#define BEGIN_CMD_MAP(x) static CMD x[] = {
#define END_CMD_MAP()   {NULL, NULL, NULL, NULL, NULL}};
#define CMD_ENTRY(CmdName, Objects, BeforeCmd, AfterCmd, Help){CmdName, Objects, BeforeCmd, AfterCmd, Help},

#define DJS_CMD_START		"-start"
#define DJS_CMD_EXEC		"-exec"
#define DJS_CMD_STOP		"-stop"
#define DJS_CMD_SET			"-set"
#define DJS_CMD_LIST		"-list"
#define DJS_CMD_LOAD		"-load"
#define DJS_CMD_SAVE		"-save"
#define DJS_CMD_HELP		"-help"

#define MAIN_ARGV_MAX	3

#define HDJS_HELP_CMD	"help"

class Command
{
public:
	Command(int argc, char* argv[]);
	int Exec(void);

public:
	static int BeforeCmd(void);
	static int AfterCmd(void);
	static bool MutuxCheck(char *pMutuxName);
	static CMD_OBJ* GetCmdObj(const CMD* pCmd, const char *pCmdArg);

private:
	int m_argc;
	char* m_argv[MAIN_ARGV_MAX];
	CMD* m_pCmd;
	CMD_OBJ *m_pCmdObj;
};

class C_OBJPROC
{
public:
	static int Help(CMD *pCmd, CMD_OBJ *pCmdObj, char *pArg);
	static int StartAll(CMD *pCmd, CMD_OBJ *pCmdObj, char *pArg);
	static int Exec(CMD *pCmd, CMD_OBJ *pCmdObj, char *pArg);
	static int ServCtrl(CMD *pCmd, CMD_OBJ *pCmdObj, char *pArg);
	static int ConfigOP(CMD *pCmd, CMD_OBJ *pCmdObj, char *pArg);
	static int SetParam(CMD *pCmd, CMD_OBJ *pCmdObj, char *pArg);
	static int ListParam(CMD *pCmd, CMD_OBJ *pCmdObj, char *pArg);

private:
	static int ConfigLoad(CMD *pCmd, CMD_OBJ *pCmdObj, char *pArg);
	static int ConfigSave(CMD *pCmd, CMD_OBJ *pCmdObj, char *pArg);
};


#endif

