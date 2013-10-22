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
  AUTOLOAD:=$(call AutoLoad,31,qca-nss-drv)
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

define KernelPackage/qca-nss-tun6rd
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for NSS (connection manager) - tun6rd
  DEPENDS:=kmod-qca-nss-drv +kmod-qca-nss-connmgr +kmod-sit
  FILES:=$(PKG_BUILD_DIR)/qca-nss-tun6rd.ko
endef

define KernelPackage/qca-nss-tun6rd/Description
Kernel modules for NSS connection manager - Support for 6rd tunnel
endef

define KernelPackage/qca-nss-tunipip6
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for NSS (connection manager) - DS-lite and ipip6 Tunnel
  DEPENDS:=kmod-qca-nss-drv +kmod-qca-nss-connmgr +kmod-iptunnel6 +kmod-ip6-tunnel
  FILES:=$(PKG_BUILD_DIR)/qca-nss-tunipip6.ko
endef

define KernelPackage/qca-nss-tunipip6/Description
Kernel modules for NSS connection manager
Add support for DS-lite and ipip6 tunnel
endef

define KernelPackage/qca-nss-connmgr/postinst
#!/bin/sh

if [ -z "$$IPKG_INSTROOT" ]; then
  ( . /etc/uci-defaults/99-qca-nss-connmgr )
  rm -f /etc/uci-defaults/99-qca-nss-connmgr
fi

exit 0
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
	$(CP) $(PKG_BUILD_DIR)/nss_api_if.h $(1)/usr/include/qca-nss-drv/
endef

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

$(eval $(call KernelPackage,qca-nss-profile-drv))
$(eval $(call KernelPackage,qca-nss-drv))
$(eval $(call KernelPackage,qca-nss-connmgr))
$(eval $(call KernelPackage,qca-nss-tun6rd))
$(eval $(call KernelPackage,qca-nss-tunipip6))
