#
# Copyright (c) 2015 The Linux Foundation. All rights reserved.
#

SWITCH_SSDK_PKGS:= kmod-qca-ssdk-hnat qca-ssdk-shell  swconfig

QCA_EDMA:=kmod-qca-edma

OPENWRT_STANDARD:= \
	luci

define Profile/QSDK_Open
	NAME:=Qualcomm-Atheros SDK Open Profile
	PACKAGES:=$(OPENWRT_STANDARD) $(SWITCH_SSDK_PKGS) $(QCA_EDMA)
endef

define Profile/QSDK_Open/Description
	QSDK Open package set configuration.
	Enables wifi open source packages
endef

$(eval $(call Profile,QSDK_Open))
