--���������ļ�

--����������
function Lua_Main()
	--dofile(CONFIG.ScriptPath .. "config.lua") -- ��C������Ԥ�ȼ���
	dofile(CONFIG.ScriptPath .. "func.lua")

	for k,v in pairs(CONFIG.ParamFiles) do
		dofile(CONFIG.ParamPath .. v)
	end

end

--ִ��������
Lua_Main()
