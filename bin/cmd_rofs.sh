#!/bin/sh

# Makes sure we run as root
if [ $(id -u) != 0 ]; then
  exec sudo "$0" "$@"
fi

fsro=`mount | grep " / " | grep -c "(ro[),]"`
if [ $fsro != "0" ]; then mount -o remount,rw / ; fi
"$@"
res=$?
if [ $fsro != "0" ]; then ( mount -o remount,ro / & ) ; fi
exit $res
