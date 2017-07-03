#!/bin/sh
#function change ip
#usage ./ifcfgdo.sh MAC IP NETMASK GW

echo "Change NetInfo ......"
echo "MAC=$1"
echo "IP=$2"
echo "NETMASK=$3"
echo "GW=$4"

#ifconfig eth0 down 
#ifconfig eth0 hw ether "$1" 
#ifconfig eth0 up 

#service network stop 
ifconfig eth0 "$2" netmask "$3" 
route add -net default gw "$4" 
#service network start 

sleep 1 

echo "Change NetInfo Done."

