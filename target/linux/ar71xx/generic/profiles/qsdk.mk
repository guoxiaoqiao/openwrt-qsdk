#
# Copyright (c) 2013 The Linux Foundation. All rights reserved.
#

define Profile/QSDK_Base
	PACKAGES:=luci uhttpd kmod-ipt-nathelper-extra luci-app-upnp kmod-fs-ext4 \
	  kmod-usb-storage kmod-usb2 kmod-fs-msdos kmod-fs-ntfs kmod-fs-vfat \
	  ntfs-3g kmod-nls-cp437 kmod-nls-iso8859-1 tftp-hpa mcproxy \
	  kmod-ipt-nathelper-rtsp kmod-ipv6 \
	  quagga quagga-ripd quagga-zebra quagga-watchquagga rp-pppoe-relay \
	  -dnsmasq dnsmasq-dhcpv6 radvd wide-dhcpv6-client bridge \
	  luci-app-ddns ddns-scripts
endef

define Profile/QSDK_Test
	PACKAGES+=dosfsck e2fsprogs fdisk mkdosfs sysstat iperf devmem2 ip \
	  ethtool ip6tables iputils-tracepath iputils-tracepath6 iozone
endef

define Profile/QSDK_Open_Router
	$(Profile/QSDK_Base)
	$(Profile/QSDK_Test)
	NAME:=Qualcomm-Atheros SDK Open Router Profile
	PACKAGES+= -kmod-ath9k -kmod-ath5k -kmod-ath -wpad-mini \
	  alljoyn alljoyn-about alljoyn-c alljoyn-config alljoyn-controlpanel \
	  alljoyn-notification alljoyn-services_common \
	  hostapd hostapd-utils iwinfo kmod-qca-ath10k kmod-qca-ath9k kmod-qca-ath \
	  kmod-fast-classifier kmod-usb2 luci-app-qos wireless-tools \
	  wpa-supplicant-p2p wpa-cli qca-legacy-uboot-ap121 qca-legacy-uboot-ap143 \
	  qca-legacy-uboot-ap152-16M
endef

define Profile/QSDK_Open_Router/Description
  QSDK Open Router package set configuration.
  This profile includes only open source packages and is designed to fit in a 16M flash. It supports:
  - Bridging and routing networking
  - LuCI web configuration interface
  - USB hard drive support
  - Samba
  - IPv4/IPv6
  - DynDns
  - Integrated 11abgn support using the ath9k driver
endef
$(eval $(call Profile,QSDK_Open_Router))

define Profile/QSDK_Wireless_Router
	$(Profile/QSDK_Base)
	NAME:=Qualcomm-Atheros SDK Wireless Router Profile
	PACKAGES+=-kmod-ath9k -kmod-ath5k -kmod-ath -wpad-mini luci-app-qos \
	  qca-legacy-uboot-ap136 kmod-qca-ssdk-nohnat qca-ssdk-shell kmod-shortcut-fe-cm \
	  kmod-qca-wifi-perf qca-hostap qca-spectral qca-hostapd-cli qca-wpa-supplicant \
	  qca-wpa-cli qca-wrapd qca-wapid qca-acfg kmod-art2 qca-legacy-uboot-ap152-8M \
	  qca-legacy-uboot-ap151-8M qca-legacy-uboot-ap147-8M qca-legacy-uboot-db12x
endef

define Profile/QSDK_Wireless_Router/Description
  QSDK Wireless Router package set configuration.
  This profile is designed to fit in a 8M flash and supports the following features:
  - Bridging and routing networking
  - LuCI web configuration interface
  - USB hard drive support
  - Samba
  - IPv4/IPv6
  - DynDns
  - qca-wifi driver
endef
$(eval $(call Profile,QSDK_Wireless_Router))

define Profile/QSDK_Premium_Router
	$(Profile/QSDK_Base)
	$(Profile/QSDK_Test)
	NAME:=Qualcomm-Atheros SDK Premium Router Profile
	PACKAGES+= -kmod-ath9k -kmod-ath5k -kmod-ath -wpad-mini \
	  streamboost hyfi kmod-fast-classifier \
	  alljoyn alljoyn-about alljoyn-c alljoyn-config alljoyn-controlpanel \
	  alljoyn-notification alljoyn-services_common \
	  kmod-qca-wifi-perf qca-hostap qca-spectral qca-hostapd-cli qca-wpa-supplicant \
	  qca-wpa-cli qca-legacy-uboot-ap135 kmod-art2 sigma-dut qca-wrapd qca-wapid \
	  qca-acfg whc qca-legacy-uboot-ap152-16M kmod-qca-ssdk-nohnat qca-ssdk-shell \
	  qca-legacy-uboot-ap147-16M qca-legacy-uboot-ap151-16M \
	  mtd-utils mtd-utils-nandwrite qca-legacy-uboot-ap135-nand
endef

define Profile/QSDK_Premium_Router/Description
  QSDK Premium Router package set configuration.
  This profile is designed to fit in a 16M flash and supports the following features:
  - Bridging and routing networking
  - QCA-WiFi driver configuration
  - LuCI web configuration interface
  - Streamboost
  - USB hard drive support
  - Samba
  - IPv4/IPv6
  - DynDns
endef
$(eval $(call Profile,QSDK_Premium_Router))

define Profile/QSDK_IoE_Device
	NAME:=Qualcomm-Atheros SDK IoE Device Profile
	PACKAGES:=luci uhttpd kmod-ipt-nathelper-extra luci-app-upnp \
	  tftp-hpa mcproxy kmod-ipt-nathelper-rtsp kmod-ipv6 \
	  -dnsmasq dnsmasq-dhcpv6 radvd wide-dhcpv6-client bridge \
	  sysstat iperf devmem2 ip ethtool ip6tables

	PACKAGES+= -kmod-ath9k -kmod-ath5k -kmod-ath -wpad-mini \
	  alljoyn alljoyn-about alljoyn-c alljoyn-config alljoyn-controlpanel \
	  alljoyn-notification alljoyn-services_common \
	  hostapd hostapd-utils iwinfo  wpa-supplicant-p2p wpa-cli wireless-tools \
	  kmod-qca-ath10k kmod-qca-ath9k kmod-qca-ath \
	  kmod-fast-classifier kmod-usb2 kmod-i2c-gpio-custom \
	  qca-legacy-uboot-cus531-16M qca-legacy-uboot-cus531-dual \
	  qca-legacy-uboot-cus531-nand
endef

define Profile/QSDK_IoE_Device/Description
	QSDK IoE Device package set configuration.
	This profile is designed to fit in a 16M flash and supports the following features:
	- QCA-WiFi driver configuration
	- IPv4/IPv6
	- AllJoyn
endef
$(eval $(call Profile,QSDK_IoE_Device))
