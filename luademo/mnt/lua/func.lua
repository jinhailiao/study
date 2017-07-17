--配置参数加载程序

--{{{表计参数配置函数
--表计缺省参数
MeterDefault =
{
	No=1,
	Enable=true,
	PortSetNo=1,
	PortSetBaud=1200,
	PortSetDataBit=8,
	PortSetStopBit=1,
	PortSetParity=2,
	MeterRule="GB645_1997",
	VipFlag=false,
	RatedVoltage=220,
	RatedCurrent=10,
	MeterAddr="000000002211",
	Password="000000",
	UserName="000000",
}

--表计配置函数
Meters = {}
function Meter( t )
	setmetatable(t, {__index=MeterDefault})
	Meters[t.No] = t
end

--}}}表计参数配置函数


--{{{主站通讯参数配置函数
--通讯缺省参数
HttxDefault =
{
	Enable=true,
	No=1,
	ReportEnable=true,
	ResendTimes=3, --重发次数
	WaitResponseTimeoutS=30, --终端等待从动站响应的超时时间
	LoginPeriodS=10, --0表示不要登陆
	HeartBeatPeriodM=5, --0表示不要心跳
	MonTrafficLimit=0, --0表示不须要流量控制
	MsgConfirmCode=0, --0表示不进行消息认证 1-255消息认证方案号
	Channel="MS1", --主通道名(必需)
}

--通讯配置函数
Httxs = {}
function Httx( t )
	setmetatable(t, {__index=HttxDefault})
	Httxs[t.No] = t
end

--}}}主站通讯参数配置函数

