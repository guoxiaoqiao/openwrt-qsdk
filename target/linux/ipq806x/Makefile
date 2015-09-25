# Copyright (c) 2013 The Linux Foundation. All rights reserved.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=ipq806x
BOARDNAME:=Qualcomm Atheros IPQ806X
FEATURES:=squashfs
CPU_TYPE:=cortex-a7
MAINTAINER:=John Crispin <blogic@openwrt.org>

LINUX_VERSION:=3.18.19

CONFIG_KERNEL_GIT_CLONE_URI:=$(TOPDIR)/qca/src/linux
CONFIG_KERNEL_GIT_BRANCH:=""
CONFIG_KERNEL_GIT_LOCAL_REPOSITORY:=""

KERNELNAME:=zImage Image dtbs

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += \
	uboot-ipq40xx uboot-ipq806x uboot-ipq806x-fwupgrade-tools uboot-envtools \
	kmod-leds-gpio kmod-gpio-button-hotplug kmod-button-hotplug swconfig \
	kmod-ata-core kmod-ata-ahci kmod-ata-ahci-platform \
	kmod-usb2 kmod-usb3 kmod-usb-dwc3-qcom kmod-usb-dwc3-ipq40xx


$(eval $(call BuildTarget))
