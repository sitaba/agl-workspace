#!/bin/bash


if [ ! -e /dev/dri/card2 ];
then
	echo "There is no additional card device"
	echo "Add '-device virtio-vga' to qemu startup options"
	exit 1
fi


export XDG_RUNTIME_DIR=/run/user/1

mkdir -p $XDG_RUNTIME_DIR
chmod 0755 $XDG_RUNTIME_DIR

weston --tty=7 --debug --drm-device=card2 &

