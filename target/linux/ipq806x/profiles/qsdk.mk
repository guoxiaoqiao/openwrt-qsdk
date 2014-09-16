#
# Copyright (c) 2013 The Linux Foundation. All rights reserved.
#

define Profile/QSDK_Base
	PACKAGES:=luci uhttpd kmod-ipt-nathelper-extra luci-app-upnp kmod-fs-ext4 \
	  kmod-usb-storage kmod-fs-msdos kmod-fs-ntfs kmod-fs-vfat \
	  kmod-sound-core kmod-sound-soc-ipq806x alsa mplayer kmod-ipt-ipopt \
	  ntfs-3g dosfsck e2fsprogs iozone fdisk mkdosfs kmod-ipt-conntrack-qos \
	  kmod-nls-cp437 kmod-nls-iso8859-1 tftp-hpa sysstat mcproxy kmod-ipt-nathelper-rtsp \
	  kmod-ipv6 iperf devmem2 ip ethtool ip6tables ds-lite \
	  quagga quagga-ripd quagga-zebra quagga-watchquagga quagga-vtysh rp-pppoe-relay \
	  -dnsmasq dnsmasq-dhcpv6 radvd wide-dhcpv6-client luci-app-samba bridge \
	  luci-app-ddns ddns-scripts cups cups-client mdadm\
	  kmod-md-mod kmod-md-linear kmod-md-raid0 kmod-md-raid1 \
	  iputils-tracepath iputils-tracepath6 \
	  alljoyn alljoyn-about alljoyn-c alljoyn-config alljoyn-controlpanel \
	  alljoyn-notification alljoyn-services_common \
	  kmod-qca-ssdk-nohnat qca-ssdk-shell \
	  kmod-art2 file pure-ftpd kmod-qca-nss-qdisc xl2tpd ppp-mod-pptp flock pm-utils \
	  kmod-qca-nss-macsec kmod-qca-wifi-akronite-perf qca-hostap qca-hostapd-cli \
	  qca-wpa-cli qca-spectral qca-wapid sigma-dut qca-acfg \
	  qca-wrapd qca-wifi-fw
endef

define Profile/QSDK_Open
	NAME:=Qualcomm-Atheros SDK Open Profile
	PACKAGES+=kmod-ath9k wpad-mini \
	  kmod-qca-nss-connmgr-noload
endef

define Profile/QSDK_Open/Description
	QSDK Open package set configuration.
	This profile includes only open source packages and provides basic Wi-Fi router features using the QCA upstream Linux Wi-Fi driver. It supports:
	-Bridging and routing networking
	-LuCI web configuration interface
	-Integrated 11abgn support using the ath9k driver
endef
$(eval $(call Profile,QSDK_Open))

define Profile/QSDK_Standard
	$(Profile/QSDK_Base)
	NAME:=Qualcomm-Atheros SDK Standard Profile
	PACKAGES+=streamboost-noload kmod-qca-nss-ecm hyfi \
		kmod-qca-nss-tun6rd kmod-qca-nss-tunipip6 qca-nss-fw-retail
endef

define Profile/QSDK_Standard/Description
	QSDK Standard package set configuration.
	This profile provides basic Wi-Fi router features using the QCA Wi-Fi driver. It supports:
	-Bridging and routing networking
	-LuCI web configuration interface
	-Integrated 11abgn/ac support using the qca-wifi driver
endef
$(eval $(call Profile,QSDK_Standard))

define Profile/QSDK_Enterprise
	$(Profile/QSDK_Base)
	NAME:=Qualcomm-Atheros SDK Enterprise Profile
	PACKAGES+=kmod-qca-nss-ecm-noload luci-app-qos \
	  kmod-openswan-nss openswan-nss luci-app-openswan \
	  kmod-crypto-ocf kmod-qca-nss-crypto kmod-qca-nss-cfi \
	  qca-nss-fw-enterprise kmod-qca-nss-ipsecmgr
endef

define Profile/QSDK_Enterprise/Description
	QSDK Enterprise package set configuration.
endef
$(eval $(call Profile,QSDK_Enterprise))
