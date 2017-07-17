/** @file
 *  @brief Lua配置程序
 *  @author jinhailiao
 *  @date 2017/03/06
 *  @version 0.1
 *  @see Lua相关文档
 */
#ifndef __HAI_LUA_BASE_H__
#define __HAI_LUA_BASE_H__
#include "lua.hpp"

class C_LuaScript
{
public:
	static int LuaError(const char *pErrorInfo);
	
	static int SetResourcePath(lua_State *L);

	static int loadScritp(lua_State * L, const char *pLuaScritp);

	static int loadenvironmentconfig(lua_State * L, const char *pConfigLua);

	static int LoadLuaConfig(void);
};

#endif//__HAI_LUA_BASE_H__

