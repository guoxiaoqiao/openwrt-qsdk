#!/bin/sh
#
# Copyright (c) 2015 The Linux Foundation. All rights reserved.
# Copyright (C) 2011 OpenWrt.org

enable_smp_affinity_wifi() {
	local device="$1"
	local hwcaps smp_affinity=1

	hwcaps=$(cat /sys/class/net/$device/hwcaps)
	irq_affinity_num=`grep $device /proc/interrupts | cut -d ':' -f 1 | tr -d ' '`

	case "${hwcaps}" in
		*11an/ac)
			smp_affinity=2
		;;
	esac

	[ -n "$irq_affinity_num" ] && echo $smp_affinity > /proc/irq/$irq_affinity_num/smp_affinity
}
