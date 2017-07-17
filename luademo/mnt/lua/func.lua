--���ò������س���

--{{{��Ʋ������ú���
--���ȱʡ����
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

--������ú���
Meters = {}
function Meter( t )
	setmetatable(t, {__index=MeterDefault})
	Meters[t.No] = t
end

--}}}��Ʋ������ú���


--{{{��վͨѶ�������ú���
--ͨѶȱʡ����
HttxDefault =
{
	Enable=true,
	No=1,
	ReportEnable=true,
	ResendTimes=3, --�ط�����
	WaitResponseTimeoutS=30, --�ն˵ȴ��Ӷ�վ��Ӧ�ĳ�ʱʱ��
	LoginPeriodS=10, --0��ʾ��Ҫ��½
	HeartBeatPeriodM=5, --0��ʾ��Ҫ����
	MonTrafficLimit=0, --0��ʾ����Ҫ��������
	MsgConfirmCode=0, --0��ʾ��������Ϣ��֤ 1-255��Ϣ��֤������
	Channel="MS1", --��ͨ����(����)
}

--ͨѶ���ú���
Httxs = {}
function Httx( t )
	setmetatable(t, {__index=HttxDefault})
	Httxs[t.No] = t
end

--}}}��վͨѶ�������ú���

