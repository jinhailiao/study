/** @file
 *  @brief Lua���û�������
 *  @author jinhailiao
 *  @date 2017/03/06
 *  @version 0.1
 *  @see Lua����ĵ�
 */
#include "lua.hpp"
#include "basetype.h"
#include "luabase.h"
#include "luaparam.h"

#ifdef WIN32
	const char *pLuaPath = "..\\..\\mnt\\lua\\";
	const char *pParamPath = "..\\..\\mnt\\param\\";
#else
	const char *pLuaPath = "/mnt/lua/";
	const char *pParamPath = "/mnt/param/";
#endif

int C_LuaScript::LuaError(const char *pErrorInfo)
{
	printf("Lua call error:%s.", pErrorInfo);
	return 0;
}

int C_LuaScript::SetResourcePath(lua_State *L)
{
	lua_pushstring(L, pLuaPath);
	lua_setglobal(L, "LuaPath");

	lua_pushstring(L, pParamPath);
	lua_setglobal(L, "ParamPath");

	return 0;
}

int C_LuaScript::loadScritp(lua_State * L, const char *pLuaScritp)
{
	string strLuaPath(pLuaPath);
	strLuaPath.append(pLuaScritp);
	if (luaL_loadfile(L, strLuaPath.c_str()) || lua_pcall(L, 0, 0, 0))
	{
		LuaError(lua_tostring(L, -1));
		return -1;
	}
	return 0;
}

int C_LuaScript::loadenvironmentconfig(lua_State * L, const char *pConfigLua)
{
	return loadScritp(L, pConfigLua);
}

int C_LuaScript::LoadLuaConfig(void)
{
	lua_State *L = lua_open();
	luaL_openlibs(L); // ��LUA���ÿ�

	// ����ƽ̨·��
	SetResourcePath(L);

	// ���ػ�������
	if (loadenvironmentconfig(L, "config.lua") < 0)
	{
		lua_close(L);
		return -1;
	}

	// ����LUA����
	if (loadScritp(L, "main.lua") < 0)
	{
		lua_close(L);
		return -1;
	}

	// ��������
	S_MeterInfos MeterInfos;
	C_ConfigFile::LoadMeterConfig(L, MeterInfos);
	S_HttxInfos HttxInfos;
	C_ConfigFile::LoadHttxConfig(L, HttxInfos);

	lua_close(L);
	return 0;
}

