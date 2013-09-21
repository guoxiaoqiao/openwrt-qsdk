# Copyright (c) 2013 Qualcomm Atheros, Inc.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=ipq806x
BOARDNAME:=QCA IPQ806X
FEATURES:=ubifs targz squashfs usb usbgadget

LINUX_VERSION:=3.4.0
KERNELNAME:="uImage"

# Kernel clone path and local repository reference can be done through menu config, else we we
# can hardcode it as below, this is just for a reference
CONFIG_EXTERNAL_KERNEL_TREE=$(TOPDIR)/qca/src/linux

CFLAGS:=-Os -pipe -march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=softfp

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += \
	mtd-utils kmod-button-hotplug uboot-envtools swconfig \
	kmod-qca-nss-drv kmod-qca-nss-gmac kmod-leds-gpio \
	kmod-ledtrig-default-on kmod-ledtrig-timer kmod-ledtrig-netdev

define Target/Description
	Build firmware images for Qualcomm Atheros IPQ806x (arm) platform.
endef

$(eval $(call BuildTarget))
