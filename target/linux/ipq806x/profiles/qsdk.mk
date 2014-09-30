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

STORAGE:=kmod-scsi-core kmod-usb-storage \
	kmod-fs-msdos kmod-fs-ntfs kmod-fs-vfat \
	kmod-nls-cp437 kmod-nls-iso8859-1 kmod-fs-ext4 \
	mdadm

CD_ROUTER:=kmod-ipt-nathelper-extra luci-app-upnp kmod-ipt-ipopt \
	kmod-ipt-conntrack-qos mcproxy kmod-ipt-nathelper-rtsp kmod-ipv6 \
	ip6tables ds-lite quagga quagga-ripd quagga-zebra quagga-watchquagga \
	quagga-vtysh rp-pppoe-relay -dnsmasq dnsmasq-dhcpv6 radvd \
	wide-dhcpv6-client bridge luci-app-ddns ddns-scripts xl2tpd ppp-mod-pptp

ALLJOYN_PKGS:=alljoyn alljoyn-about alljoyn-c alljoyn-config \
	alljoyn-controlpanel alljoyn-notification alljoyn-services_common

UTILS:=tftp-hpa sysstat iperf devmem2 ip ethtool iputils-tracepath \
	iputils-tracepath6 file pure-ftpd pm-utils kmod-art2-netlink

define Profile/QSDK_Standard
	NAME:=Qualcomm-Atheros SDK Standard Profile
	PACKAGES:=$(OPENWRT_STANDARD) $(NSS_STANDARD) $(SSDK_PKGS) \
		$(WIFI_OPEN_PKGS) $(STORAGE) $(CD_ROUTER) $(UTILS) \
		$(ALLJOYN_PKGS)
endef

define Profile/QSDK_Standard/Description
	QSDK Standard package set configuration.
endef
$(eval $(call Profile,QSDK_Standard))
