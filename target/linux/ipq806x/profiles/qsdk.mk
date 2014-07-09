#
# Copyright (c) 2014 The Linux Foundation. All rights reserved.
#

NSS_STANDARD:= \
	qca-nss-fw-retail \
	kmod-qca-nss-drv \
	kmod-qca-nss-gmac

SSDK_PKGS:= kmod-qca-ssdk-nohnat qca-ssdk-shell  swconfig

WIFI_OPEN_PKGS:= kmod-qca-ath9k kmod-qca-ath10k wpad

OPENWRT_STANDARD:= \
	luci

define Profile/QSDK_Standard
	NAME:=Qualcomm-Atheros SDK Standard Profile
	PACKAGES:=$(OPENWRT_STANDARD) $(NSS_STANDARD) $(SSDK_PKGS) \
		$(WIFI_OPEN_PKGS)
endef

define Profile/QSDK_Standard/Description
	QSDK Standard package set configuration.
endef
$(eval $(call Profile,QSDK_Standard))
