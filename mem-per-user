#!/bin/sh
OLDIFS=$IFS
IFS=$'\n'
tempsum=0
totalmem=0
for m in `ps -eo user,rss --sort user | sed -e 's/  */ /g' | awk -F'[ ]' {'print $0'}`; do
  nu=`echo $m|cut -d" " -f1`
  nm=`echo $m|cut -d" " -f2`
  # echo "$nu $nm $nu"
  if [ "$nu" != "$ou" ] && [ $(echo "$nm"|grep -E "^[0-9]+$") ] 
  then 
    if [ "$tempsum" -ne 0 ]; then echo "Printing total mem for $ou: $tempsum"; fi
    ou=$nu
    tempsum=$nm
    let "totalmem += $nm"
  else 
    let "tempsum += $nm" 
    let "totalmem += $nm"
  fi
done
echo "Total Memory in Use: $totalmem/$(free | grep Mem: | awk '{print $2}')"
IFS=$OLDIFS
