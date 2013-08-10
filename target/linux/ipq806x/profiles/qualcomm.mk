#
# Copyright (c) 2013 Qualcomm Atheros, Inc.
#

define Profile/QSDK_Base
	PACKAGES:=luci uhttpd kmod-ipt-nathelper-extra luci-app-upnp kmod-fs-ext4 \
	  kmod-usb-core kmod-usb2 kmod-usb-storage kmod-fs-msdos kmod-fs-ntfs kmod-fs-vfat \
	  kmod-nls-cp437 kmod-nls-iso8859-1 tftp-hpa sysstat igmpproxy kmod-ipt-nathelper-rtsp \
	  kmod-ipv6 luci-app-qos iperf devmem2 ip ethtool \
	  quagga quagga-ripd quagga-zebra quagga-watchquagga rp-pppoe-relay \
	  -dnsmasq dnsmasq-dhcpv6 radvd wide-dhcpv6-client luci-app-samba bridge \
	  luci-app-ddns ddns-scripts uboot-ipq806x-cdp
endef

define Profile/QSDK_Open
	NAME:=Qualcomm-Atheros SDK Open Profile
	PACKAGES+=kmod-ath9k wpad-mini
endef

define Profile/QSDK_Open/Description
	QSDK Open package set configuration.
	This profile includes only open source packages and provides basic Wi-Fi router features using the QCA upstream Linux Wi-Fi driver. It supports:
	-Bridging and routing networking
	-LuCI web configuration interface
	-Integrated 11abgn support using the ath9k driver
endef
$(eval $(call Profile,QSDK_Open))

define Profile/QSDK_Retail
	$(Profile/QSDK_Base)
	NAME:=Qualcomm-Atheros SDK Retail Profile
	PACKAGES+=kmod-qca-wifi qca-hostap qca-hostapd-cli qca-wpa-cli qca-wpa-supplicant
endef

define Profile/QSDK_Retail/Description
	QSDK Retail package set configuration.
	This profile provides basic Wi-Fi router features using the QCA proprietary Wi-Fi driver. It supports:
	-Bridging and routing networking
	-LuCI web configuration interface
	-Integrated 11abgn/ac support using the proprietary qca-wifi driver
endef
$(eval $(call Profile,QSDK_Retail))

define Profile/QSDK_Carrier
	$(Profile/QSDK_Base)
	NAME:=Qualcomm-Atheros SDK Carrier Profile
	PACKAGES+=kmod-qca-wifi qca-hostap qca-hostapd-cli qca-wpa-cli qca-wpa-supplicant
endef

define Profile/QSDK_Carrier/Description
	QSDK Enterprise package set configuration.
endef
$(eval $(call Profile,QSDK_Carrier))

define Profile/QSDK_Enterprise
	$(Profile/QSDK_Base)
	NAME:=Qualcomm-Atheros SDK Enterprise Profile
	PACKAGES+=kmod-qca-wifi qca-hostap qca-hostapd-cli qca-wpa-cli qca-wpa-supplicant \
	  kmod-openswan openswan kmod-crypto-ocf
endef

define Profile/QSDK_Enterprise/Description
	QSDK Enterprise package set configuration.
endef
$(eval $(call Profile,QSDK_Enterprise))
