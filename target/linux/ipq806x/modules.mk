#
# Copyright (C) 2014 The Linux Foundation. All rights reserved.
#

define KernelPackage/qca-edma
     SUBMENU:=$(NETWORK_DEVICES_MENU)
     TITLE:=Qualcomm 961x ethernet driver
     FILES:=$(LINUX_DIR)/drivers/net/ethernet/qcom/essedma/essedma.ko
     AUTOLOAD:=$(call AutoLoad,45,essedma)
endef

define KernelPackage/qca-edma/description
     Kernel modules for QCA961x integrated ethernet adapater.
endef

$(eval $(call KernelPackage,qca-edma))
