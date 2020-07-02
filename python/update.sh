#!/bin/bash
updateversion=$(yum check-update cdos-desktop-cloud|grep cdos-desktop-cloud)
if [ "$updateversion" == "" ];then
    echo 0
else
    echo 1
fi
#ioldversion=$(rpm -qa cdos-desktop-cloud)

#result=$(echo $oldversion | grep "${updateversion}") 
#if [[ "$result" != "" ]]
#then 
#    echo 0
#else
#    echo 1
#fi
