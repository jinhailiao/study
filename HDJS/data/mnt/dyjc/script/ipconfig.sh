#!/bin/sh
#function change ip
#see ipconfig.ini
#call ipcfgdo.sh

if [ -r /mnt/dyjc/set/user/ipconfig.ini ]; then
	IPCFGFILE=/mnt/dyjc/set/user/ipconfig.ini
elif [ -r /mnt/dyjc/set/default/ipconfig.ini ]; then
	IPCFGFILE=/mnt/dyjc/set/default/ipconfig.ini
fi

if [ "$IPCFGFILE" == "" ]; then
	echo "Warning:ipconfig.ini is not find!!!"
else
	IP=$(grep '^IP' $IPCFGFILE | cut -f1 -d# | cut -f2 -d= | sed 's/[[:space:]]//g')
	NETMASK=$(grep '^NETMASK' $IPCFGFILE | cut -f1 -d# | cut -f2 -d= | sed 's/[[:space:]]//g')
	GW=$(grep '^GW' $IPCFGFILE | cut -f1 -d# | cut -f2 -d= | sed 's/[[:space:]]//g')
fi

if [ "$IP" == "" ]; then
	IP=192.168.0.100
fi
if [ "$NETMASK" == "" ]; then
	NETMASK=255.255.255.0
fi
if [ "$GW" == "" ]; then
	GW=192.168.0.1
fi
MAC=00:01:02:B4:56:58

/mnt/dyjc/script/ipcfgdo.sh "$MAC" "$IP" "$NETMASK" "$GW"

