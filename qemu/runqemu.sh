#!/bin/bash


#stty intr '^]'
#echo "[info] you should use '^]' to stop qemu"
#trap "stty intr ^C" EXIT


echo "[info] load environment"
source x86-64_qemu.env

if [ -n "${KVM}" ] && [ ! -e "/dev/kvm" ];
then
	echo "Not found: /dev/kvm"
	echo "You should start this container with --privileged"
        exit
fi


if [ $# -ne 0 ] && [ "$1" == "--no-snap" ] || [ "$1" == "-n" ];
then
        echo "Disable: snapshot"
        SNAPSHOT=""
fi


#create_tap.sh


echo "[info] start qemu"
eval sudo ${QEMU} \
	${KVM} \
	${SNAPSHOT} \
	${MACHINE} \
	${CPU} \
	${MEMORY} \
	${DEVICE_NET} \
	${DEVICE_RNG} \
	${DEVICE_BLK} \
	${DEVICE_USB} \
	${DEVICE_GRAPHICS} \
	${SERIAL} \
	${KERNEL} \
	${APPEND}
