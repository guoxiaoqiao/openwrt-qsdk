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
	kmod-nls-cp437 kmod-nls-iso8859-1

# TODO: trim to separate packages for features, or cdrouter, etc
QSDK_BASE:=kmod-ipt-nathelper-extra luci-app-upnp kmod-fs-ext4 \
          kmod-ipt-ipopt kmod-ipt-conntrack-qos kmod-nls-cp437 kmod-nls-iso8859-1 \
	  tftp-hpa sysstat mcproxy kmod-ipt-nathelper-rtsp \
          kmod-ipv6 iperf devmem2 ip ethtool ip6tables ds-lite \
          quagga quagga-ripd quagga-zebra quagga-watchquagga quagga-vtysh rp-pppoe-relay \
          -dnsmasq dnsmasq-dhcpv6 radvd wide-dhcpv6-client bridge \
          luci-app-ddns ddns-scripts mdadm\
          iputils-tracepath iputils-tracepath6 \
          file pure-ftpd xl2tpd ppp-mod-pptp flock pm-utils \

define Profile/QSDK_Standard
	NAME:=Qualcomm-Atheros SDK Standard Profile
	PACKAGES:=$(OPENWRT_STANDARD) $(NSS_STANDARD) $(SSDK_PKGS) \
		$(WIFI_OPEN_PKGS) $(STORAGE) $(QSDK_BASE)
endef

define Profile/QSDK_Standard/Description
	QSDK Standard package set configuration.
endef
$(eval $(call Profile,QSDK_Standard))
