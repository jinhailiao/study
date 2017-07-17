--参数加载文件

--定义主函数
function Lua_Main()
	--dofile(CONFIG.ScriptPath .. "config.lua") -- 在C函数中预先加载
	dofile(CONFIG.ScriptPath .. "func.lua")

	for k,v in pairs(CONFIG.ParamFiles) do
		dofile(CONFIG.ParamPath .. v)
	end

end

--执行主函数
Lua_Main()
