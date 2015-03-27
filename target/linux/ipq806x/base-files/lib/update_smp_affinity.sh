#!/bin/sh
#
# Copyright (c) 2014 The Linux Foundation. All rights reserved.
# Copyright (C) 2011 OpenWrt.org

enable_smp_affinity_wifi() {
	local device="$1"
	local radioidx=${device#wifi}
	local max_radio_supported=3

	no_of_cpu=`grep processor /proc/cpuinfo | wc -l`
	no_of_radio=`ls -l /sys/class/net/wifi* | wc -l`
	smp_affinity=`expr $no_of_cpu - $radioidx + $no_of_radio - $max_radio_supported`

	if [ "$no_of_cpu" -ge 2 -a "$smp_affinity" -ge 0 -a "$smp_affinity" -lt "$no_of_cpu" ]; then
		let "smp_affinity = 1 << $smp_affinity"
		irq_affinity_num=`grep $device /proc/interrupts | cut -d ':' -f 1 | tr -d ' '`
		[ -n "$irq_affinity_num" ] && echo $smp_affinity > /proc/irq/$irq_affinity_num/smp_affinity
	fi
}
