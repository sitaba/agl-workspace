#!/bin/bash


#stty intr '^]'
#echo "[info] you should use '^]' to stop qemu"
#trap "stty intr ^C" EXIT

echo "[info] TAP_MODE: ${TAP_MODE:-true}"
if ${TAP_MODE}; then
	echo "[info] load environment: x86-64_qemu_tap-1.env"
	source x86-64_qemu_tap-1.env
	echo "[info] create tap network"
	BRIDGE_ADDR=${BRIDGE_ADDR}/24 ./create_tap.sh
	trap "./clean_tap.sh" EXIT
else
	echo "[info] load environment: x86-64_qemu.env"
	source x86-64_qemu.env
fi


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
