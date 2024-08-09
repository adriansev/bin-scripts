#!/bin/bash
echo "Scan for new disk(s)..."
for host in $(ls /sys/class/scsi_host/)
do
        echo "$host"
        echo "- - -" > /sys/class/scsi_host/"$host"/scan
done

echo ""

echo "Scan for disk size change..."
for disk in $(ls /sys/class/block/ | grep sd | grep -v '[0-9]')
do
        echo "$disk"
        echo 1>/sys/class/block/"$disk"/device/rescan
done

