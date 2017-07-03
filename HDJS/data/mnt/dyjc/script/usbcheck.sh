#!/bin/sh
#function: usb check

if [ -r /var/run/usbcheck.pid ]; then
	kill -KILL `cat /var/run/usbcheck.pid`
	rm -f /var/run/usbcheck.pid
fi

echo $$ >> /var/run/usbcheck.pid

while [ 1 ]; do
	echo "usb check..."
	sleep 60
done

