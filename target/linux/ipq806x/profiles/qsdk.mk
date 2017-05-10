define Profile/QSDK_Base
	PACKAGES:=luci uhttpd kmod-ipt-nathelper-extra luci-app-upnp \
	  kmod-usb-storage kmod-fs-msdos kmod-fs-ntfs kmod-fs-vfat \
	  kmod-sound-core kmod-sound-soc-ipq806x alsa kmod-ipt-ipopt \
	  ntfs-3g dosfsck e2fsprogs iozone fdisk mkdosfs kmod-ipt-conntrack-qos \
	  kmod-nls-cp437 kmod-nls-iso8859-1 sysstat mcproxy kmod-ipt-nathelper-rtsp \
	  kmod-ipv6 iperf devmem2 ip ethtool ip6tables ds-lite rstp \
	  quagga quagga-ripd quagga-zebra quagga-watchquagga quagga-vtysh rp-pppoe-relay \
	  -dnsmasq dnsmasq-dhcpv6 radvd wide-dhcpv6-client luci-app-samba luci-proto-6x4 bridge \
	  luci-app-ddns ddns-scripts mdadm\
	  kmod-md-mod kmod-md-linear kmod-md-raid0 kmod-md-raid1 \
	  iputils-tracepath iputils-tracepath6 \
	  kmod-qca-ssdk-nohnat qca-ssdk-shell \
	  file pure-ftpd kmod-qca-nss-drv-qdisc xl2tpd ppp-mod-pptp pm-utils \
	  kmod-qca-nss-macsec qca-thermald rng-tools perf kmod-nat-sctp kmod-aq_phy \
	  kmod-qca_85xx_sw aq-fw-download kmod-regmap-i2c i2c-tools qca-mcs-apps \
	  kmod-crypto-aes kmod-ipsec kmod-ipsec4 kmod-ipsec6 lacpd
endef

PACKAGES_WIFI_10_2:=kmod-qca-wifi-akronite-perf kmod-art2 qca-hostap-10.4 qca-hostapd-cli-10.4 \
	  qca-wpa-supplicant-10.4-macsec qca-wpa-cli-10.4 qca-spectral qca-wapid sigma-dut-10.4 \
	  qca-acfg qca-wrapd whc whc-ui qca-thermald-10.4 qca-wifi-fw-hw1-10.2 qca-wifi-fw-hw1-10.2-lteu \
	  qca-wifi-fw-hw1-10.2-maxclients qca-wifi-fw-hw2-10.2

PACKAGES_WIFI:=kmod-qca-wifi-unified-profile kmod-art2 \
	qca-hostap qca-hostapd-cli qca-wpa-supplicant-macsec \
	qca-wpa-cli qca-wapid sigma-dut-10.4 qca-wpc \
	qca-acfg qca-wrapd qca-spectral qcmbr-10.4 whc whc-ui \
	qca-wifi-fw-hw2-10.4-asic qca-wifi-fw-hw3-10.4-asic \
	qca-wifi-fw-hw4-10.4-asic qca-wifi-fw-hw4-10.4-emu_m2m qca-wifi-fw-hw4-10.4-emu_bb \
	qca-thermald-10.4 qca-wifi-fw-hw6-10.4-asic qca-wifi-fw-hw7-10.4-asic \
	qca-wifi-fw-hw10-10.4-asic athdiag


PACKAGES_NSS_ENTERPRISE:=kmod-qca-nss-ecm-noload kmod-openswan-nss \
	openswan-nss kmod-qca-nss-crypto kmod-qca-nss-cfi \
	kmod-qca-nss-drv-profile kmod-qca-nss-drv-capwapmgr \
	qca-nss-fw2-enterprise kmod-qca-nss-drv-ipsecmgr \
	qca-nss-fw2-enterprise_custA qca-nss-fw2-enterprise_custC \
	kmod-qca-nss-drv-dtlsmgr

BLUETOOTH:=kmod-qca-ath3k bluez btconfig

HYFI:=hyfi hyfi-ui
HYFI_PLC:=hyfi-plc hyfi-ui

FAILSAFE:=kmod-bootconfig

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
	PACKAGES+=streamboost-noload kmod-qca-nss-ecm $(HYFI) kmod-qca-nss-drv-profile \
		kmod-qca-nss-drv-tun6rd kmod-qca-nss-drv-tunipip6 qca-nss-fw2-retail \
		luci-app-openswan openswan-nss kmod-openswan-nss \
		kmod-qca-nss-drv-ipsecmgr kmod-crypto-ocf kmod-qca-nss-crypto kmod-qca-nss-cfi \
		$(PACKAGES_WIFI_10_2) $(BLUETOOTH) kmod-qca-wil6210 wigig-firmware-ipdock iwinfo \
		$(FAILSAFE)
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
	PACKAGES+=streamboost-noload kmod-qca-nss-ecm $(HYFI) \
		kmod-qca-nss-drv-tun6rd kmod-qca-nss-drv-tunipip6 qca-nss-fw2-retail \
		kmod-qca-nss-drv-l2tpv2 \
		kmod-qca-nss-drv-pptp \
		luci-app-openswan openswan-nss kmod-openswan-nss \
		kmod-qca-nss-drv-ipsecmgr kmod-crypto-ocf kmod-qca-nss-crypto kmod-qca-nss-cfi \
		$(PACKAGES_WIFI) kmod-qca-wil6210 wigig-firmware iwinfo $(FAILSAFE)
endef

define Profile/QSDK_Standard_Beeliner/Description
	QSDK Standard package set configuration.
	This profile provides basic Wi-Fi router features using the QCA Wi-Fi driver. It supports:
	-Bridging and routing networking
	-LuCI web configuration interface
	-Integrated 11abgn/ac support using the qca-wifi driver
endef
$(eval $(call Profile,QSDK_Standard_Beeliner))

define Profile/QSDK_Enterprise
	$(Profile/QSDK_Base)
	NAME:=Qualcomm-Atheros SDK Enterprise Profile
	PACKAGES+=luci-app-openswan kmod-crypto-ocf \
		$(PACKAGES_NSS_ENTERPRISE) \
		$(PACKAGES_WIFI_10_2) $(FAILSAFE)
endef

define Profile/QSDK_Enterprise/Description
	QSDK Enterprise package set configuration.
endef
$(eval $(call Profile,QSDK_Enterprise))

define Profile/QSDK_Enterprise_Beeliner
	$(Profile/QSDK_Base)
	NAME:=Qualcomm-Atheros SDK Enterprise Profile
	PACKAGES+=luci-app-openswan kmod-crypto-ocf \
                $(PACKAGES_NSS_ENTERPRISE) \
                $(PACKAGES_WIFI) $(FAILSAFE)
endef

define Profile/QSDK_Enterprise_Beeliner/Description
	QSDK Enterprise package set configuration with Beeliner wifi support.
endef
$(eval $(call Profile,QSDK_Enterprise_Beeliner))
