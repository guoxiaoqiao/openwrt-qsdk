#! /bin/sh
#
# Copyright (c) 2013 The Linux Foundation. All rights reserved.
#

. /lib/functions.sh
include /lib/upgrade

kill_remaining TERM
sleep 3
kill_remaining KILL

ROOTFS_PART=$(grep rootfs_data /proc/mtd |cut -f4 -d' ')

[ -z $ROOTFS_PART ] && {
	ROOTFS_PART="$(find_mmc_part "rootfs_data")"
	run_ramfs ". /lib/functions.sh; include /lib/upgrade; sync; \
	dd if=/dev/zero of=${ROOTFS_PART} bs=1K count=2; reboot"
	exit 1
}

run_ramfs ". /lib/functions.sh; include /lib/upgrade; sync; mtd -r erase ${ROOTFS_PART}"
