--配置文件

--应用程序设置的路径
if LuaPath == nil then
	LuaPath = "./"
end
if ParamPath == nil then
	ParamPath = "./"
end

CONFIG = {}

--LUA功能脚本的目录
CONFIG.ScriptPath = LuaPath
--参数文件的目录
CONFIG.ParamPath = ParamPath

--此处配置参数文件名
CONFIG.ParamFiles = {"meter.lua", "httx.lua"}


