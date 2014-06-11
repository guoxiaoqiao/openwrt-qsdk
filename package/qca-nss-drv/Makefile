#
# Copyright (c) 2013 The Linux Foundation. All rights reserved.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=qca-nss-drv
PKG_SOURCE_PROTO:=git
PKG_BRANCH:=master
PKG_RELEASE:=1

include $(INCLUDE_DIR)/local-development.mk
ifeq ($(DUMP)$(PKG_VERSION),)
  PKG_VERSION:=$(shell git ls-remote $(PKG_SOURCE_URL) $(PKG_BRANCH) | cut -b -7)
endif
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/qca-nss-drv
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  DEPENDS:=@TARGET_ipq806x qca-nss-fw
  TITLE:=Kernel driver for NSS (core driver)
  FILES:=$(PKG_BUILD_DIR)/qca-nss-drv.ko
  AUTOLOAD:=$(call AutoLoad,31,qca-nss-drv)
endef

define KernelPackage/qca-nss-drv/Description
This package contains a NSS driver for QCA chipset
endef

define KernelPackage/qca-nss-drv/install
	$(INSTALL_DIR) $(1)/lib/debug
	$(INSTALL_BIN) ./files/qca-nss-drv.debug $(1)/lib/debug/qca-nss-drv
endef

define KernelPackage/qca-nss-tun6rd
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for NSS (connection manager) - tun6rd
  DEPENDS:=kmod-qca-nss-drv +kmod-sit
  FILES:=$(PKG_BUILD_DIR)/qca-nss-tun6rd.ko
  AUTOLOAD:=$(call AutoLoad,51,qca-nss-tun6rd)
endef

define KernelPackage/qca-nss-tun6rd/Description
Kernel modules for NSS connection manager - Support for 6rd tunnel
endef

define KernelPackage/qca-nss-tunipip6
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for NSS (connection manager) - DS-lite and ipip6 Tunnel
  DEPENDS:=kmod-qca-nss-drv +kmod-iptunnel6 +kmod-ip6-tunnel
  FILES:=$(PKG_BUILD_DIR)/qca-nss-tunipip6.ko
  AUTOLOAD:=$(call AutoLoad,51,qca-nss-tunipip6)
endef

define KernelPackage/qca-nss-tunipip6/Description
Kernel modules for NSS connection manager
Add support for DS-lite and ipip6 tunnel
endef

define KernelPackage/qca-nss-profile-drv
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  DEPENDS:=kmod-qca-nss-drv
  TITLE:=Profiler for QCA NSS driver (IPQ806x)
  FILES:=$(PKG_BUILD_DIR)/profiler/qca-nss-profile-drv.ko
endef

define KernelPackage/qca-nss-profile-drv/Description
This package contains a NSS driver profiler for QCA chipset
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include/qca-nss-drv
	$(CP) $(PKG_BUILD_DIR)/exports/* $(1)/usr/include/qca-nss-drv/
endef

define KernelPackage/qca-nss-qdisc
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Support
  TITLE:=Qdisc for configuring shapers in NSS
  DEPENDS:=kmod-qca-nss-drv
  FILES:=$(PKG_BUILD_DIR)/qca-nss-qdisc.ko
  AUTOLOAD:=$(call AutoLoad,33,qca-nss-qdisc)
endef

define KernelPackage/qca-nss-qdisc/Description
Linux qdisc that aids in configuring shapers in the NSS
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
		modules
endef

$(eval $(call KernelPackage,qca-nss-profile-drv))
$(eval $(call KernelPackage,qca-nss-drv))
$(eval $(call KernelPackage,qca-nss-tun6rd))
$(eval $(call KernelPackage,qca-nss-tunipip6))
$(eval $(call KernelPackage,qca-nss-qdisc))
