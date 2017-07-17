--note 后台通讯配置文件
--see upchannl.ini
--EAC:httx[1-6]配置主站通道[1-8],httx7配置本地调试口,httx8配置红外,壁挂式httx4配置红外
--FK|JC|PB:缺省配置 httx1配置主站通道,httx2配置本地调试口,httx3配置红外,httx4配置测试口


Httx
{
	Enable=true,
	No=1,
	ReportEnable=true,
	ResendTimes=3, --重发次数
	WaitResponseTimeoutS=30, --终端等待从动站响应的超时时间
	LoginPeriodS=10, --0表示不要登陆
	HeartBeatPeriodM=5, --0表示不要心跳
	Channel="MS1", --主通道名(必需)
}

Httx
{
	Enable=false,
	No=2,
	ReportEnable=false,
	ResendTimes=3, --重发次数
	WaitResponseTimeoutS=30, --终端等待从动站响应的超时时间
	LoginPeriodS=0, --0表示不要登陆
	HeartBeatPeriodM=0, --0表示不要心跳
	Channel="LOCAL1", --主通道名(必需)
}

Httx
{
	Enable=true,
	No=3,
	ReportEnable=false,
	ResendTimes=3, --重发次数
	WaitResponseTimeoutS=30, --终端等待从动站响应的超时时间
	LoginPeriodS=0, --0表示不要登陆
	HeartBeatPeriodM=0, --0表示不要心跳
	Channel="SERIAL1", --主通道名(必需)
}

Httx
{
	Enable=true,
	No=4,
	ReportEnable=false,
	ResendTimes=3, --重发次数
	WaitResponseTimeoutS=30, --终端等待从动站响应的超时时间
	LoginPeriodS=0, --0表示不要登陆
	HeartBeatPeriodM=0, --0表示不要心跳
	Channel="bus_485III", --主通道名(必需)
}

