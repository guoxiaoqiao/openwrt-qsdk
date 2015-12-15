# Copyright (c) 2013 The Linux Foundation. All rights reserved.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=ipq806x
BOARDNAME:=Qualcomm Atheros IPQ806X
FEATURES:=squashfs
CPU_TYPE:=cortex-a7
MAINTAINER:=John Crispin <blogic@openwrt.org>

LINUX_VERSION:=3.18.21

KERNELNAME:=zImage Image dtbs

# check .config for an overridden Linux dir, otherwise use qca/src/linux
CONFIG_EXTERNAL_KERNEL_TREE=$(shell \
	grep -q CONFIG_EXTERNAL_KERNEL_TREE=\"\" $(TOPDIR)/.config && \
	echo '$(TOPDIR)/qca/src/linux' || \
	sed -n 's,CONFIG_EXTERNAL_KERNEL_TREE="\(.*\)",\1,p' $(TOPDIR)/.config)

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += \
	uboot-ipq40xx uboot-ipq806x uboot-ipq806x-fwupgrade-tools uboot-envtools \
	kmod-leds-gpio kmod-gpio-button-hotplug kmod-button-hotplug swconfig \
	kmod-ata-core kmod-ata-ahci kmod-ata-ahci-platform \
	kmod-usb2 kmod-usb3 kmod-usb-dwc3-qcom kmod-usb-dwc3-ipq40xx


$(eval $(call BuildTarget))
