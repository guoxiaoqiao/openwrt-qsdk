#
# Copyright (c) 2013 Qualcomm Atheros, Inc..
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=qca-nss-drv
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=ssh://qca-git01.qualcomm.com:29418/lklm/nss-drv
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
  DEPENDS:=@TARGET_ipq806x
  TITLE:=Kernel driver for NSS (core driver)
  FILES:=$(PKG_BUILD_DIR)/qca-nss-drv.ko
  AUTOLOAD:=$(call AutoLoad,10,qca-nss-drv)
endef

define KernelPackage/qca-nss-drv/Description
This package contains a NSS driver for QCA chipset
endef

define KernelPackage/qca-nss-connmgr
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for NSS (connection manager)
  DEPENDS:=kmod-qca-nss-drv +iptables-mod-extra
  FILES:=$(PKG_BUILD_DIR)/qca-nss-connmgr-ipv4.ko $(PKG_BUILD_DIR)/qca-nss-connmgr-ipv6.ko
  KCONFIG:=CONFIG_BRIDGE_NETFILTER=y
  AUTOLOAD:=$(call AutoLoad,50,qca-nss-connmgr-ipv4 qca-nss-connmgr-ipv6)
endef

define KernelPackage/qca-nss-connmgr/Description
Kernel modules for NSS connection manager
endef

define KernelPackage/qca-nss-connmgr/postinst
#!/bin/sh

if [ -z "$$IPKG_INSTROOT" ]; then
  ( . /etc/uci-defaults/99-qca-nss-connmgr )
  rm -f /etc/uci-defaults/99-qca-nss-connmgr
fi

exit 0
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include/qca-nss-drv
	$(CP) $(PKG_BUILD_DIR)/nss_api_if.h $(1)/usr/include/qca-nss-drv/
endef

EXTRA_CFLAGS+= \
	-DCONFIG_NSS_DEBUG_LEVEL=4 \
	-I$(PKG_BUILD_DIR)/nss_hal/include \
	-I$(PKG_BUILD_DIR)/nss_hal/$(BOARD)

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
		modules
endef

define KernelPackage/qca-nss-connmgr/install
	$(INSTALL_DIR) $(1)/etc/firewall.d $(1)/etc/uci-defaults
	$(INSTALL_DATA) ./files/qca-nss-connmgr.defaults $(1)/etc/uci-defaults/99-qca-nss-connmgr
	$(INSTALL_DATA) ./files/qca-nss-connmgr.firewall $(1)/etc/firewall.d/qca-nss-connmgr
endef

$(eval $(call KernelPackage,qca-nss-drv))
$(eval $(call KernelPackage,qca-nss-connmgr))
