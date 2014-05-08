# Copyright (c) 2013 The Linux Foundation. All rights reserved.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=ipq806x
BOARDNAME:=Qualcomm Atheros IPQ806X
FEATURES:=ubifs targz squashfs
CPU_TYPE:=cortex-a7

LINUX_VERSION:=3.14

KERNELNAME="uImage dtbs"
KERNELLOAD=0x41708000

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))
