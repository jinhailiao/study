--note ��̨ͨѶ�����ļ�
--see upchannl.ini
--EAC:httx[1-6]������վͨ��[1-8],httx7���ñ��ص��Կ�,httx8���ú���,�ڹ�ʽhttx4���ú���
--FK|JC|PB:ȱʡ���� httx1������վͨ��,httx2���ñ��ص��Կ�,httx3���ú���,httx4���ò��Կ�


Httx
{
	Enable=true,
	No=1,
	ReportEnable=true,
	ResendTimes=3, --�ط�����
	WaitResponseTimeoutS=30, --�ն˵ȴ��Ӷ�վ��Ӧ�ĳ�ʱʱ��
	LoginPeriodS=10, --0��ʾ��Ҫ��½
	HeartBeatPeriodM=5, --0��ʾ��Ҫ����
	Channel="MS1", --��ͨ����(����)
}

Httx
{
	Enable=false,
	No=2,
	ReportEnable=false,
	ResendTimes=3, --�ط�����
	WaitResponseTimeoutS=30, --�ն˵ȴ��Ӷ�վ��Ӧ�ĳ�ʱʱ��
	LoginPeriodS=0, --0��ʾ��Ҫ��½
	HeartBeatPeriodM=0, --0��ʾ��Ҫ����
	Channel="LOCAL1", --��ͨ����(����)
}

Httx
{
	Enable=true,
	No=3,
	ReportEnable=false,
	ResendTimes=3, --�ط�����
	WaitResponseTimeoutS=30, --�ն˵ȴ��Ӷ�վ��Ӧ�ĳ�ʱʱ��
	LoginPeriodS=0, --0��ʾ��Ҫ��½
	HeartBeatPeriodM=0, --0��ʾ��Ҫ����
	Channel="SERIAL1", --��ͨ����(����)
}

Httx
{
	Enable=true,
	No=4,
	ReportEnable=false,
	ResendTimes=3, --�ط�����
	WaitResponseTimeoutS=30, --�ն˵ȴ��Ӷ�վ��Ӧ�ĳ�ʱʱ��
	LoginPeriodS=0, --0��ʾ��Ҫ��½
	HeartBeatPeriodM=0, --0��ʾ��Ҫ����
	Channel="bus_485III", --��ͨ����(����)
}

