#
# Copyright (c) 2014 The Linux Foundation. All rights reserved.
#

NSS_STANDARD:= \
	qca-nss-fw-retail \
	kmod-qca-nss-drv \
	kmod-qca-nss-gmac \
	kmod-qca-edma

NSS_ECM:= kmod-qca-nss-ecm

NSS_CLIENTS:= kmod-qca-nss-drv-qdisc

HW_CRYPTO:= kmod-crypto-qcrypto

SHORTCUT_FE:= kmod-shortcut-fe kmod-shortcut-fe-cm
QCA_RFS:= kmod-qca-rfs

SWITCH_SSDK_PKGS:= kmod-qca-ssdk-hnat qca-ssdk-shell  swconfig
SWITCH_OPEN_PKGS:= kmod-switch-ar8216 swconfig

WIFI_OPEN_PKGS:= kmod-qca-ath9k kmod-qca-ath10k wpad hostapd-utils \
		 kmod-art2-netlink sigma-dut-open wpa-cli

WIFI_10_4_PKGS:=kmod-qca-wifi-10.4-akronite-perf qca-wifi-fw-10.4-emu \
	qca-hostap-10.4 qca-hostapd-cli-10.4 qca-wpa-supplicant-10.4 \
	qca-wpa-cli-10.4 qca-spectral-10.4 qca-wapid-10.4 sigma-dut-10.4 \
	qcmbr-10.4 qca-wrapd-10.4

OPENWRT_STANDARD:= \
	luci

STORAGE:=kmod-scsi-core kmod-usb-storage \
	kmod-fs-msdos kmod-fs-ntfs kmod-fs-vfat \
	kmod-nls-cp437 kmod-nls-iso8859-1 kmod-fs-ext4 \
	mdadm

CD_ROUTER:=kmod-ipt-nathelper-extra luci-app-upnp kmod-ipt-ipopt \
	kmod-ipt-conntrack-qos mcproxy kmod-ipt-nathelper-rtsp kmod-ipv6 \
	ip6tables ds-lite quagga quagga-ripd quagga-zebra quagga-watchquagga \
	quagga-vtysh rp-pppoe-relay -dnsmasq dnsmasq-dhcpv6 radvd \
	wide-dhcpv6-client bridge luci-app-ddns ddns-scripts xl2tpd ppp-mod-pptp \
	iptables-mod-extra iptables-mod-ipsec iptables-mod-filter 6rd luci-proto-ipv6 \
	kmod-bonding luci-app-qos luci-app-radvd kmod-nat-sctp openswan arptables
ALLJOYN_PKGS:=alljoyn alljoyn-about alljoyn-c alljoyn-config \
	alljoyn-controlpanel alljoyn-notification alljoyn-services_common

UTILS:=tftp-hpa sysstat iperf devmem2 ip ethtool iputils-tracepath \
	iputils-tracepath6 file pure-ftpd pm-utils trace-cmd qca-thermald \
	luci-app-samba

BLUETOOTH:=bluez kmod-ath3k

define Profile/QSDK_Open
	NAME:=Qualcomm-Atheros SDK Open Profile
	PACKAGES:=$(OPENWRT_STANDARD) $(NSS_STANDARD) $(SWITCH_OPEN_PKGS) \
		$(WIFI_OPEN_PKGS) $(STORAGE) $(CD_ROUTER) $(UTILS) \
		$(ALLJOYN_PKGS) $(BLUETOOTH) $(NSS_ECM) $(NSS_CLIENTS)
endef

define Profile/QSDK_Open/Description
	QSDK Open package set configuration.
	Enables wifi open source packages
endef
$(eval $(call Profile,QSDK_Open))

define Profile/QSDK_Standard
	NAME:=Qualcomm-Atheros SDK Standard Profile
	PACKAGES:=$(OPENWRT_STANDARD) $(NSS_STANDARD) $(SWITCH_SSDK_PKGS) \
		$(WIFI_10_4_PKGS) $(STORAGE) $(CD_ROUTER) $(UTILS) \
		$(ALLJOYN_PKGS) $(SHORTCUT_FE) $(BLUETOOTH) $(HW_CRYPTO) $(QCA_RFS)
endef

define Profile/QSDK_Standard/Description
	QSDK Standard package set configuration.
	Enables qca-wifi 10.4 packages
endef

$(eval $(call Profile,QSDK_Standard))
