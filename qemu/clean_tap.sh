#!/bin/bash -ue

BRIDGE_NAME=${BRIDGE_NAME:-br-qemu}
TAPDEV_NAME_PREFIX=${TAPDEV_NAME_PREFIX:-tap}
TAPDEV_NUM=${TAPDEV_NUM:-$(ip addr show | grep "${TAPDEV_NAME_PREFIX}-[0-9]*" | awk '{print $2}' | tail -n1 | sed -n 's/^.*\([0-9]\+\).*$/\1/p')}

echo "[info] starting: clearn_tap.sh"
for num in $(seq $TAPDEV_NUM); do
	TAPDEV_NAME=${TAPDEV_NAME_PREFIX}-${num}
	if ip addr show dev ${TAPDEV_NAME} &> /dev/null; then
		sudo ip tuntap del dev ${TAPDEV_NAME} mode tap
	fi
	echo "[info] deleted tap device: ${TAPDEV_NAME}"
done

if ip addr show dev $BRIDGE_NAME &> /dev/null; then
	sudo ip link del ${BRIDGE_NAME} type bridge
	echo "[info] deleted bridge device: ${BRIDGE_NAME}"
fi


