#
# Copyright (c) 2015 The Linux Foundation. All rights reserved.
#

IOE_BASE:=luci uhttpd luci-app-upnp mcproxy rp-pppoe-relay \
	  -dnsmasq dnsmasq-dhcpv6 radvd wide-dhcpv6-client bridge \
	  -swconfig luci-app-ddns ddns-scripts luci-app-qos \
	  kmod-nf-nathelper-extra kmod-ipt-nathelper-rtsp kmod-ipv6 \
	  kmod-usb2 kmod-i2c-gpio-custom kmod-button-hotplug

STORAGE:=kmod-scsi-core kmod-usb-storage \
	 kmod-fs-msdos kmod-fs-ntfs kmod-fs-vfat \
	 kmod-nls-cp437 kmod-nls-iso8859-1 \
	 mdadm ntfs-3g e2fsprogs fdisk mkdosfs

TEST_TOOLS:=sysstat devmem2 ethtool i2c-tools ip ip6tables iperf-mt \
	    iputils-ping iputils-ping6

ALLJOYN:=alljoyn alljoyn-about alljoyn-c alljoyn-config \
	 alljoyn-controlpanel alljoyn-notification \
	 alljoyn-services_common

WIFI_OPEN:=-kmod-ath5k -kmod-qca-ath -kmod-qca-ath9k -kmod-qca-ath10k \
	   kmod-ath kmod-ath9k hostapd hostapd-utils iwinfo wpa-supplicant \
	   wpa-cli wireless-tools -wpad-mini sigma-dut-open

BLUEZ:=bluez-daemon kmod-bluetooth usbutils

BLUETOPIA:=kmod-bluetopia-usb-driver bluetopia

BLUETOOTH:=$(BLUETOPIA)

WIFI_10_4:=kmod-qca-wifi-10.4-unified-perf kmod-art2 \
        qca-hostap-10.4 qca-hostapd-cli-10.4 qca-wpa-supplicant-10.4 \
        qca-wpa-cli-10.4 qca-wapid qca-wpc-10.4 \
        qca-acfg-10.4 qca-wrapd-10.4 qca-spectral-10.4 qcmbr-10.4 whc whc-ui \
        qca-wifi-fw-hw3-10.4-asic qca-wifi-fw-hw6-10.4-asic \
        qca-wifi-fw-hw9-10.4-asic qca-wifi-fw-hw10-10.4-asic \
        qca-iface-mgr-10.4

define Profile/QSDK_IOE_SB
	NAME:=Qualcomm-Atheros SDK IoE Single Band Profile
	PACKAGES:=$(IOE_BASE) $(TEST_TOOLS) $(WIFI_OPEN) \
		  qca-legacy-uboot-ap143-16M qca-legacy-uboot-ap143-32M \
		  qca-legacy-uboot-cus531-16M qca-legacy-uboot-cus531-dual \
		  qca-legacy-uboot-cus531-32M qca-legacy-uboot-cus531-nand \
		  qca-legacy-uboot-cus532k
endef

define Profile/QSDK_IOE_SB/Description
	QSDK IoE SB package set configuration.
	Enables WiFi 2.4G open source packages
endef

define Profile/QSDK_IOE_DBPAN
	NAME:=Qualcomm-Atheros SDK IoE Dual Band and Personal Area Network Profile
	PACKAGES:=$(IOE_BASE) $(TEST_TOOLS) $(WIFI_OPEN) $(BLUETOOTH) \
		  qca-legacy-uboot-cus531mp3-16M qca-legacy-uboot-cus531mp3-32M \
		  qca-legacy-uboot-cus531mp3-dual qca-legacy-uboot-cus531mp3-nand \
		  qca-legacy-uboot-ap147ioe-16M qca-legacy-uboot-ap147ioe-nand \
		  qca-legacy-uboot-ap147ioe-dual \
		  kmod-usb-serial kmod-usb-serial-pl2303 kmod-ath10k \
		  ath10k-firmware-qca9888
endef

define Profile/QSDK_IOE_DBPAN/Description
	QSDK IoE DBPAN package set configuration.
	Enables WiFi 2.4G, 5G, Bluetooth open source packages
endef

define Profile/QSDK_IOE_PREMIUM
	NAME:=Qualcomm-Atheros SDK IoE Dual Band and Personal Area Network Profile
	PACKAGES:=$(IOE_BASE) $(TEST_TOOLS) $(WIFI_10_4) $(BLUETOOTH) \
		  qca-legacy-uboot-cus531mp3-16M qca-legacy-uboot-cus531mp3-32M \
		  qca-legacy-uboot-cus531mp3-dual qca-legacy-uboot-cus531mp3-nand \
		  kmod-usb-serial kmod-usb-serial-pl2303
endef

define Profile/QSDK_IOE_PREMIUM/Description
	QSDK IoE premium package set configuration.
	Enables WiFi 2.4G, 5G, Bluetooth open source packages
endef

define Profile/QSDK_IOE_TEST
	NAME:=Qualcomm-Atheros SDK IoE test Profile
	PACKAGES:=$(IOE_BASE) $(TEST_TOOLS) $(WIFI_OPEN) kmod-bluetooth  \
		  kmod-usb-serial kmod-usb-serial-pl2303 kmod-art2  \
                  btdiag qcmbr-10.4
endef

define Profile/QSDK_IOE_TEST/Description
	QSDK IoE test package set configuration.
endef

$(eval $(call Profile,QSDK_IOE_SB))
$(eval $(call Profile,QSDK_IOE_DBPAN))
$(eval $(call Profile,QSDK_IOE_PREMIUM))
$(eval $(call Profile,QSDK_IOE_TEST))
