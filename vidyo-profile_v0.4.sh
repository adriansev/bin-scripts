#!/bin/bash

profile() {
    echo "============= profile script version =================="
    echo 0.4
    echo
	echo "============= running uname -a ========================"
	uname -a
	echo 
	echo "============= running cat /proc/cpuinfo ==============="
	cat /proc/cpuinfo
	echo 
	echo "============= scanning memory info ===================="
	echo "--- system:"
	grep Total /proc/meminfo
	echo
	echo "--- video:"
	grep Memory /var/log/Xorg.*.log
	echo
	echo "============= running cat /etc/issue =================="
	cat /etc/issue
	echo 
	echo "============= getting session type ===================="
	echo ${DESKTOP_SESSION}
	echo
	echo "============= running glxinfo ========================="
	glxinfo
	echo 
	echo "============= running lspci ==========================="
	if [ -f /sbin/lspci ]; then
	/sbin/lspci
	else
	lspci
	fi
	echo 
	echo "============= running lsusb ==========================="
	if [ -f /sbin/lsusb ]; then
	/sbin/lsusb
	else
	lsusb
	fi
	echo 
	echo "============= running aplay -l ========================"
	aplay -l
	echo
	echo "============= running processes ======================="
	ps alx
}

if test -z "$1" ; then
	corefile=`ls -t ~/core* 2>/dev/null | head -1`
else
	corefile="$1"
fi

if test -z "$corefile" ; then
	corefile=~/core ; fi

if test ! -f $corefile ; then
	echo "*** core file missing or invalid; specify as argument" ; fi

rm -rf ~/.vidyo-temp
mkdir ~/.vidyo-temp
cd ~/.vidyo-temp

date=`date +%Y%m%d-%H%M%S`

(cd ~/ ; ls -lt core* 2>/dev/null) >core.index

if test -f $corefile ; then
	cp -a $corefile core ; fi

cp /opt/vidyo/VidyoDesktop/VidyoDesktop .
cp ~/.vidyo/VidyoDesktop/*.log .
cp /var/log/Xorg.*.log .
if test -f /etc/X11/xorg.conf ; then
	cp /etc/X11/xorg.conf . ; fi
profile >system-profile.txt
echo "packing vidyo-debug-$date.tar.gz..."
tar -cvzf ~/Desktop/vidyo-debug-$date.tar.gz *
cd ~/
rm -rf ~/.vidyo-temp


