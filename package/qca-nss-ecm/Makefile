#
# Copyright (c) 2014 The Linux Foundation. All rights reserved.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=qca-nss-ecm
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

define KernelPackage/qca-nss-ecm
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Support
  DEPENDS:=@TARGET_ipq806x +kmod-qca-nss-drv
  TITLE:=QCA NSS Enhanced Connection Manager (ECM)
  FILES:=$(PKG_BUILD_DIR)/ecm_tracker_udp.ko \
	$(PKG_BUILD_DIR)/ecm_tracker_tcp.ko \
	$(PKG_BUILD_DIR)/ecm_tracker_datagram.ko \
	$(PKG_BUILD_DIR)/ecm_tracker.ko \
	$(PKG_BUILD_DIR)/ecm_front_end_ipv4.ko \
	$(PKG_BUILD_DIR)/ecm_front_end_ipv6.ko \
	$(PKG_BUILD_DIR)/ecm_db.ko \
	$(PKG_BUILD_DIR)/ecm_classifier_default.ko \
	$(PKG_BUILD_DIR)/ecm_conntrack_notifier.ko \
	$(PKG_BUILD_DIR)/ecm_classifier_nl.ko \
	$(PKG_BUILD_DIR)/ecm_classifier_hyfi.ko \
	$(PKG_BUILD_DIR)/ecm_interface.ko \
	$(PKG_BUILD_DIR)/ecm_bond_notifier.ko
endef

define KernelPackage/qca-nss-qdisc-basic/Description
This package contains the Qualcomm Atheros NSS Enhanced Connection Manager
endef

EXTRA_CFLAGS+= \
	-I$(STAGING_DIR)/usr/include/qca-nss-drv

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
		modules
endef

$(eval $(call KernelPackage,qca-nss-ecm))
