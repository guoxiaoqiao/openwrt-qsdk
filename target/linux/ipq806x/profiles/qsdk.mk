#
# Copyright (c) 2015 The Linux Foundation. All rights reserved.
#

SWITCH_SSDK_PKGS:= kmod-qca-ssdk-hnat qca-ssdk-shell  swconfig

QCA_EDMA:=kmod-qca-edma

WIFI_OPEN_PKGS:= kmod-ath9k kmod-ath10k wpad hostapd-utils \
		 sigma-dut-open wpa-cli qcmbr-10.4-netlink

OPENWRT_STANDARD:= \
	luci

STORAGE:=kmod-scsi-core kmod-usb-storage \
	kmod-fs-msdos kmod-fs-vfat kmod-fs-ntfs \
	kmod-nls-cp437 kmod-nls-iso8859-1 \
	e2fsprogs

USB_ETHERNET:= kmod-usb-net-rtl8152 kmod-usb-net

UTILS:=luci-app-samba

NETWORKING:=bridge

COREBSP_UTILS:=pm-utils

BLUETOOTH:=kmod-bluetooth bluez-examples bluez-libs bluez-utils

define Profile/QSDK_Open
	NAME:=Qualcomm-Atheros SDK Open Profile
	PACKAGES:=$(OPENWRT_STANDARD) $(SWITCH_SSDK_PKGS) $(QCA_EDMA) \
	$(WIFI_OPEN_PKGS) $(STORAGE) $(USB_ETHERNET) $(UTILS) $(NETWORKING) \
	$(COREBSP_UTILS) $(BLUETOOTH)
endef

define Profile/QSDK_Open/Description
	QSDK Open package set configuration.
	Enables wifi open source packages
endef

$(eval $(call Profile,QSDK_Open))
