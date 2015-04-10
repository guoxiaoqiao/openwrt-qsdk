#!/bin/sh
#
# Copyright (c) 2015 The Linux Foundation. All rights reserved.
# Copyright (C) 2011 OpenWrt.org

enable_smp_affinity_wifi() {
        irq_wifi=`grep -m2 ath10k_pci /proc/interrupts | cut -d ':' -f 1 | tail -n1 | tr -d ' '`
	[ -n "$irq_wifi" ] && echo 2 > /proc/irq/$irq_wifi/smp_affinity
}
