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

LINUX_VERSION:=3.3.7

define Target/Description
	Build firmware images for Ubicom based routers and media devices.
endef

include $(INCLUDE_DIR)/target.mk

KERNEL_MAKEOPTS += LDFLAGS_BUILD_ID=""

define Kernel/Configure
	$(MAKE) -C $(PLATFORM_DIR)/image/ultra compile
	ln -sf $(CONFIG_ULTRA_DIR)/projects/mainexec/build/include/ocm_size.h $(LINUX_DIR)/arch/ubicom32/include/asm/ocm_size.h
	$(call Kernel/Configure/Default)
endef

$(eval $(call RequireCommand,ubicom32-elf-gcc, \
	Kernel and ultra currently require ubicom32-elf-gcc to build. \
))

$(eval $(call BuildTarget))
