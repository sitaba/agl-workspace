#!/bin/bash -ue


BRIDGE_NAME=${BRIDGE_NAME:-br-qemu}
BRIDGE_ADDR=${BRIDGE_ADDR:-192.168.1.1/24}
TAPDEV_NUM=${TAPDEV_NUM:-1}
TAPDEV_NAME_PREFIX=${TAPDEV_NAME_PREFIX:-tap}

## if you want to remove the network devices, do following
# - delete ip settings
#   sudo ip addr del ${BRIDGE_ADDR} dev ${BRIDGE_NAME}
# - delete devices
#   sudo ip tuntap del dev ${TAPDEV_NAME} mode tap
#   sudo ip link del ${BRIDGE_NAME} type bridge

echo "[info] starting: create_tap.sh"
if ! ip addr show dev $BRIDGE_NAME &> /dev/null; then
	sudo ip link add $BRIDGE_NAME type bridge
	if ! ip addr show dev $BRIDGE_NAME &> /dev/null; then
		echo "[Failed] Creation Network Bridge: $BRIDGE_NAME"
		exit
	fi
else
	echo "[info] Already existing: $BRIDGE_NAME"
fi

echo "[info] Linking up: ${BRIDGE_NAME}"
sudo ip link set ${BRIDGE_NAME} up

if ! ip addr show dev ${BRIDGE_NAME} | grep -q "{BRIDGE_ADDR}"; then
	echo "[info] Adding bridge ip addr: ${BRIDGE_ADDR}"
	sudo ip addr add ${BRIDGE_ADDR} dev ${BRIDGE_NAME}
else
	echo "[info] Bridge has already set ip addr: ${BRIDGE_ADDR}"
fi


for num in $(seq $TAPDEV_NUM); do
	TAPDEV_NAME=${TAPDEV_NAME_PREFIX}-${num}
	if ! ip addr show dev ${TAPDEV_NAME} &> /dev/null; then
		echo "[info] Creating: ${TAPDEV_NAME}"
		sudo ip tuntap add dev ${TAPDEV_NAME} mode tap
		if ! ip addr show dev ${TAPDEV_NAME} &> /dev/null; then
			echo "[Failed] Creation Network Tap: ${TAPDEV_NAME}"
			exit
		fi
	else
		echo "[info] Already exsiting: ${TAPDEV_NAME}"
	fi

	if ! ip addr show dev ${TAPDEV_NAME} | grep -q "master ${BRIDGE_NAME}"; then
		echo "[info] Setting tap master: ${TAPDEV_NAME}"
		sudo ip link set ${TAPDEV_NAME} master ${BRIDGE_NAME}
		if ! ip addr show dev ${TAPDEV_NAME} | grep -q "master ${BRIDGE_NAME}"; then
			echo "[Failed] Linking tap to master: ${TAPDEV_NAME}"
			exit
		fi
	else
		echo "[info] Already linking: ${TAPDEV_NAME}"
	fi
	echo "[info] Linking up: ${TAPDEV_NAME}"
	sudo ip link set ${TAPDEV_NAME} up
done


