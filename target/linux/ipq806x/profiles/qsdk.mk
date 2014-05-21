#
# Copyright (c) 2014 The Linux Foundation. All rights reserved.
#

NSS_STANDARD:= \
	qca-nss-fw-retail \
	kmod-qca-nss-drv \
	kmod-qca-nss-gmac

SSDK_PKGS:= kmod-qca-ssdk-nohnat swconfig

OPENWRT_STANDARD:= \
	luci

define Profile/QSDK_Standard
	NAME:=Qualcomm-Atheros SDK Standard Profile
	PACKAGES:=$(OPENWRT_STANDARD) $(NSS_STANDARD) $(SSDK_PKGS)
endef

define Profile/QSDK_Standard/Description
	QSDK Standard package set configuration.
endef
$(eval $(call Profile,QSDK_Standard))
