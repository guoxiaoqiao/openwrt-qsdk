#
# Copyright (C) 2012 Qualcomm-Atheros Inc.
#

define Profile/QSDK_Open
	NAME:=Qualcomm-Atheros Open Profile
	PACKAGES:=luci uhttpd kmod-usb-core kmod-usb2 kmod-usb-storage -kmod-ath5k
endef

define Profile/QSDK_Open/Description
	QSDK Open package set configuration.
	This profile includes only open source packages and provides basic Wi-Fi router features using the QCA upstream Linux Wi-Fi driver. It supports:
	-Bridging and routing networking
	-LuCI web configuration interface
	-Integrated 11abgn support using the ath9k driver
endef
$(eval $(call Profile,QSDK_Open))

define Profile/QSDK_Main
	NAME:=Qualcomm-Atheros Main Profile
	PACKAGES:=luci uhttpd kmod-usb-core kmod-usb2 kmod-usb-storage kmod-qca-wifi \
	  qca-hostap qca-hostapd-cli qca-wpa-cli qca-wpa-supplicant qca-legacy-uboot-ap135 \
	  qca-legacy-uboot-db12x  -kmod-ath9k -kmod-ath5k -kmod-ath -wpad-mini
endef

define Profile/QSDK_Main/Description
	QSDK Main package set configuration.
	This profile provides basic Wi-Fi router features using the QCA proprietary Wi-Fi driver. It supports:
	-Bridging and routing networking
	-LuCI web configuration interface
	-Integrated 11abgn/ac support using the proprietary qca-wifi driver
endef
$(eval $(call Profile,QSDK_Main))
