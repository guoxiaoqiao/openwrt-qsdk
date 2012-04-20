#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=ubicom32
BOARD:=ubicom32
BOARDNAME:=Ubicom IPxx
SUBTARGETS=IP8100RGW
FEATURES:=ramdisk

LINUX_VERSION:=2.6.36

define Target/Description
	Build firmware images for Ubicom based routers and media devices.
endef

include $(INCLUDE_DIR)/target.mk

KERNEL_MAKEOPTS += LDFLAGS_BUILD_ID=""

$(eval $(call BuildTarget))
