# Copyright (c) 2013 The Linux Foundation. All rights reserved.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=ipq806x
BOARDNAME:=Qualcomm Atheros IPQ806X
FEATURES:=ubifs targz squashfs
CPU_TYPE:=cortex-a7

LINUX_VERSION:=3.14.43

KERNELNAME="Image dtbs"

# Kernel clone path and local repository reference can be done through menu config, else we we
# can hardcode it as below, this is just for a reference
CONFIG_KERNEL_GIT_CLONE_URI:=$(TOPDIR)/qca/src/linux
CONFIG_KERNEL_GIT_BRANCH:=""
CONFIG_KERNEL_GIT_LOCAL_REPOSITORY:=""

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += \
	mtd-utils mtd-utils-mkfs.ubifs mtd-utils-ubiattach mtd-utils-ubidetach \
	mtd-utils-ubiformat mtd-utils-ubimkvol mtd-utils-ubiupdatevol \
	uboot-ipq806x uboot-qca961x kmod-usb-dwc3-qcom kmod-usb2 kmod-usb3 kmod-usb-dwc3-ipq40xx \
	uboot-envtools uboot-ipq806x-fwupgrade-tools

$(eval $(call BuildTarget))
