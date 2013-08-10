#
# Copyright (c) 2013 Qualcomm Atheros, Inc.
#

define Profile/QSDK_Open
	NAME:=Qualcomm-Atheros SDK Open Profile
	PACKAGES:=luci uhttpd kmod-usb-core kmod-usb2 kmod-usb-storage -kmod-ath5k \
	  kmod-ipt-nathelper-extra luci-app-upnp tftp-hpa sysstat igmpproxy kmod-ipt-nathelper-rtsp \
	  kmod-fs-msdos kmod-fs-ntfs kmod-fs-vfat kmod-nls-cp437 kmod-nls-iso8859-1 \
	  luci-app-ddns ddns-scripts kmod-ipv6 iwinfo luci-app-qos \
	  quagga quagga-ripd quagga-zebra quagga-watchquagga rp-pppoe-relay \
	  -dnsmasq dnsmasq-dhcpv6 radvd wide-dhcpv6-client \
	  -wpad-mini hostapd hostapd-utils wpa-supplicant wpa-cli wireless-tools
endef

define Profile/QSDK_Open/Description
	QSDK Open package set configuration.
	This profile includes only open source packages and provides basic Wi-Fi router features using the QCA upstream Linux Wi-Fi driver. It supports:
	-Bridging and routing networking
	-LuCI web configuration interface
	-Integrated 11abgn support using the ath9k driver
endef
$(eval $(call Profile,QSDK_Open))

define Profile/QSDK_Base
	PACKAGES:=luci uhttpd kmod-ipt-nathelper-extra luci-app-upnp kmod-fs-ext4 \
	  kmod-usb-core kmod-usb2 kmod-usb-storage kmod-fs-msdos kmod-fs-ntfs kmod-fs-vfat \
	  kmod-nls-cp437 kmod-nls-iso8859-1 tftp-hpa sysstat igmpproxy kmod-ipt-nathelper-rtsp \
	  kmod-ipv6 luci-app-qos kmod-ebtables iperf devmem2 ip ethtool \
	  quagga quagga-ripd quagga-zebra quagga-watchquagga rp-pppoe-relay \
	  kmod-qca-wifi qca-hostap qca-hostapd-cli qca-wpa-cli qca-wpa-supplicant \
	  -dnsmasq dnsmasq-dhcpv6 radvd wide-dhcpv6-client luci-app-samba bridge \
	  luci-app-ddns ddns-scripts qca-legacy-uboot-ap135 qca-legacy-uboot-db12x uboot-ipq806x-cdp \
	  -kmod-ath9k -kmod-ath5k -kmod-ath -wpad-mini
endef

define Profile/QSDK_Retail
	$(Profile/QSDK_Base)
	NAME:=Qualcomm-Atheros SDK Retail Profile
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
	PACKAGES+=
endef

define Profile/QSDK_Carrier/Description
	QSDK Enterprise package set configuration.
endef
$(eval $(call Profile,QSDK_Carrier))

define Profile/QSDK_Enterprise
	$(Profile/QSDK_Base)
	NAME:=Qualcomm-Atheros SDK Enterprise Profile
	PACKAGES+=kmod-openswan openswan kmod-crypto-ocf
endef

define Profile/QSDK_Enterprise/Description
	QSDK Enterprise package set configuration.
endef
$(eval $(call Profile,QSDK_Enterprise))
