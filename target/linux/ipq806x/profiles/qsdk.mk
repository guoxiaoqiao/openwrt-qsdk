#
# Copyright (c) 2013 The Linux Foundation. All rights reserved.
#

define Profile/QSDK_Base
	PACKAGES:=luci uhttpd kmod-ipt-nathelper-extra luci-app-upnp \
	  kmod-usb-storage kmod-fs-msdos kmod-fs-ntfs kmod-fs-vfat \
	  kmod-sound-core kmod-sound-soc-ipq806x alsa mplayer kmod-ipt-ipopt \
	  ntfs-3g dosfsck e2fsprogs iozone fdisk mkdosfs kmod-ipt-conntrack-qos \
	  kmod-nls-cp437 kmod-nls-iso8859-1 tftp-hpa sysstat mcproxy kmod-ipt-nathelper-rtsp \
	  kmod-ipv6 iperf devmem2 ip ethtool ip6tables ds-lite rstp \
	  quagga quagga-ripd quagga-zebra quagga-watchquagga quagga-vtysh rp-pppoe-relay \
	  -dnsmasq dnsmasq-dhcpv6 radvd wide-dhcpv6-client luci-app-samba luci-proto-6x4 bridge \
	  luci-app-ddns ddns-scripts cups cups-client mdadm\
	  kmod-md-mod kmod-md-linear kmod-md-raid0 kmod-md-raid1 \
	  iputils-tracepath iputils-tracepath6 \
	  alljoyn alljoyn-about alljoyn-c alljoyn-config alljoyn-controlpanel \
	  alljoyn-notification alljoyn-services_common \
	  kmod-qca-ssdk-nohnat qca-ssdk-shell \
	  file pure-ftpd kmod-qca-nss-drv-qdisc xl2tpd ppp-mod-pptp pm-utils \
	  kmod-qca-nss-macsec qca-thermald rng-tools perf kmod-nat-sctp kmod-aq_phy \
	  kmod-qca_85xx_sw aq-fw-download kmod-regmap-i2c i2c-tools
endef

PACKAGES_WIFI_10_2:=kmod-qca-wifi-akronite-perf kmod-art2 qca-hostap qca-hostapd-cli \
	  qca-wpa-supplicant-macsec qca-wpa-cli qca-spectral qca-wapid sigma-dut \
	  qca-acfg qca-wrapd whc qca-thermald

PACKAGES_WIFI_10_4:=kmod-qca-wifi-10.4-akronite-perf kmod-art2 \
	qca-hostap-10.4 qca-hostapd-cli-10.4 qca-wpa-supplicant-10.4-macsec \
	qca-wpa-cli-10.4 qca-wapid-10.4 sigma-dut-10.4 qca-wpc-10.4 \
	qca-acfg-10.4 qca-wrapd-10.4 qca-spectral-10.4 qcmbr-10.4 whc \
	qca-wifi-fw-hw2-10.4-asic qca-wifi-fw-hw3-10.4-asic \
       qca-wifi-fw-hw4-10.4-asic qca-wifi-fw-hw4-10.4-emu_m2m qca-wifi-fw-hw4-10.4-emu_bb \
	qca-thermald-10.4

PACKAGES_NSS_ENTERPRISE:=kmod-qca-nss-ecm-noload kmod-openswan-nss \
	openswan-nss kmod-qca-nss-crypto kmod-qca-nss-cfi \
	kmod-qca-nss-drv-profile kmod-qca-nss-drv-capwapmgr \
	qca-nss-fw-enterprise kmod-qca-nss-drv-ipsecmgr \
	qca-nss-fw-enterprise_custA qca-nss-fw-enterprise_custC

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
	PACKAGES+=streamboost-noload kmod-qca-nss-ecm hyfi kmod-qca-nss-drv-profile \
		kmod-qca-nss-drv-tun6rd kmod-qca-nss-drv-tunipip6 qca-nss-fw-retail \
		kmod-crypto-ocf kmod-qca-nss-crypto kmod-qca-nss-cfi \
		$(PACKAGES_WIFI_10_2)
endef

define Profile/QSDK_Standard/Description
	QSDK Standard package set configuration.
	This profile provides basic Wi-Fi router features using the QCA 10.2 Wi-Fi driver. It supports:
	-Bridging and routing networking
	-LuCI web configuration interface
	-Integrated 11abgn/ac support using the qca-wifi driver
endef
$(eval $(call Profile,QSDK_Standard))

define Profile/QSDK_Standard_Beeliner
	$(Profile/QSDK_Base)
	NAME:=Qualcomm-Atheros SDK Standard Beeliner Profile
	PACKAGES+=streamboost-noload kmod-qca-nss-ecm hyfi \
		kmod-qca-nss-drv-tun6rd kmod-qca-nss-drv-tunipip6 qca-nss-fw-retail \
		kmod-crypto-ocf kmod-qca-nss-crypto kmod-qca-nss-cfi \
		$(PACKAGES_WIFI_10_4) kmod-qca-wil6210 iwinfo
endef

define Profile/QSDK_Standard_Beeliner/Description
	QSDK Standard package set configuration.
	This profile provides basic Wi-Fi router features using the QCA 10.4 Wi-Fi driver. It supports:
	-Bridging and routing networking
	-LuCI web configuration interface
	-Integrated 11abgn/ac support using the qca-wifi driver
endef
$(eval $(call Profile,QSDK_Standard_Beeliner))

define Profile/QSDK_Enterprise
	$(Profile/QSDK_Base)
	NAME:=Qualcomm-Atheros SDK Enterprise Profile
	PACKAGES+=luci-app-qos luci-app-openswan kmod-crypto-ocf \
		$(PACKAGES_NSS_ENTERPRISE) \
		$(PACKAGES_WIFI_10_2)
endef

define Profile/QSDK_Enterprise/Description
	QSDK Enterprise package set configuration.
endef
$(eval $(call Profile,QSDK_Enterprise))

define Profile/QSDK_Enterprise_Beeliner
	$(Profile/QSDK_Base)
	NAME:=Qualcomm-Atheros SDK Enterprise Profile
	PACKAGES+=luci-app-qos luci-app-openswan kmod-crypto-ocf \
                $(PACKAGES_NSS_ENTERPRISE) \
                $(PACKAGES_WIFI_10_4)
endef

define Profile/QSDK_Enterprise_Beeliner/Description
	QSDK Enterprise package set configuration with Beeliner wifi support.
endef
$(eval $(call Profile,QSDK_Enterprise_Beeliner))
