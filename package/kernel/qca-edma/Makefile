#
# Copyright (C) 2015 The Linux Foundation. All rights reserved.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=qca-edma
PKG_SOURCE_PROTO:=git
PKG_BRANCH:=master
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/qca-edma
     SUBMENU:=Network Devices
     TITLE:=Qualcomm IPQ40xx ethernet driver
     FILES:=$(LINUX_DIR)/drivers/net/ethernet/qcom/essedma/essedma.ko
     AUTOLOAD:=$(call AutoLoad,45,essedma)
endef

define KernelPackage/qca-edma/description
     Kernel modules for IPQ40xx integrated ethernet adapater.
endef

Build/Compile=true

define KernelPackage/qca-edma/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/qca-edma $(1)/etc/init.d/qca-edma
endef

$(eval $(call KernelPackage,qca-edma))
