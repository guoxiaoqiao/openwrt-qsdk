#
# Copyright (C) 2012 Qualcomm-Atheros Inc.
#

define Profile/QSDK_Open
	NAME:=Qualcomm-Atheros SDK Open Profile
	PACKAGES:=luci uhttpd kmod-usb-core kmod-usb2 kmod-usb-storage -kmod-ath5k \
	  kmod-ipt-nathelper-extra luci-app-upnp tftp-hpa sysstat igmpproxy kmod-ipt-nathelper-rtsp \
	  kmod-fs-msdos kmod-fs-ntfs kmod-fs-vfat kmod-nls-cp437 kmod-nls-iso8859-1 \
	  luci-app-ddns ddns-scripts kmod-ipv6 iwinfo luci-app-qos \
	  -wpad-mini hostapd hostapd-utils wpa-supplicant wpa-cli
endef

define Profile/QSDK_Open/Description
	QSDK Open package set configuration.
	This profile includes only open source packages and provides basic Wi-Fi router features using the QCA upstream Linux Wi-Fi driver. It supports:
	-Bridging and routing networking
	-LuCI web configuration interface
	-Integrated 11abgn support using the ath9k driver
endef
$(eval $(call Profile,QSDK_Open))

define Profile/QSDK_Main
	NAME:=Qualcomm-Atheros SDK Main Profile
	PACKAGES:=luci uhttpd kmod-ipt-nathelper-extra luci-app-upnp \
	  kmod-usb-core kmod-usb2 kmod-usb-storage kmod-fs-msdos kmod-fs-ntfs kmod-fs-vfat \
	  kmod-nls-cp437 kmod-nls-iso8859-1 tftp-hpa sysstat igmpproxy kmod-ipt-nathelper-rtsp \
	  kmod-ipv6 luci-app-qos kmod-art2-4.9-scorpion \
	  kmod-qca-wifi qca-hostap qca-hostapd-cli qca-wpa-cli qca-wpa-supplicant \
	  luci-app-ddns ddns-scripts qca-legacy-uboot-ap135 qca-legacy-uboot-db12x \
	  -kmod-ath9k -kmod-ath5k -kmod-ath -wpad-mini
endef

define Profile/QSDK_Main/Description
	QSDK Main package set configuration.
	This profile provides basic Wi-Fi router features using the QCA proprietary Wi-Fi driver. It supports:
	-Bridging and routing networking
	-LuCI web configuration interface
	-Integrated 11abgn/ac support using the proprietary qca-wifi driver
endef
$(eval $(call Profile,QSDK_Main))

define Profile/Skifta/Default
	PACKAGES:=-dropbear -firewall -ppp -wpad-mini alsa -luci-theme-openwrt \
		kmod-usb-core kmod-usb2 kmod-usb-storage kmod-sound-soc-cus227 \
		kmod-fs-msdos kmod-fs-ntfs kmod-fs-vfat avahi-daemon \
		kmod-nls-cp437 kmod-nls-iso8859-1 \
		coreutils-md5sum qca-romboot-cus227 \
		-hostapd -wpa-supplicant -hostapd-utils \
		kmod-qca-wifi qca-hostap qca-hostapd-cli qca-wpa-cli qca-wpa-supplicant \
		libffmpeg-full \
		orb luci-skifta alsa skifta qcom-state-mgr-skifta \
		-kmod-ath9k -kmod-ath5k -kmod-ath
endef

define Profile/Skifta/Description/Default
	Qualcomm-Atheros Skifta package set configuration
	This profile provides all the skifta features in the firmware for the
	Skifta supported boards. It includes
	- STA mode configuration
	- Skifta configuration manager
	- UPnP/DLNA Renderer
	- Audio drivers
	- Audio player software
	- Skifta Engine
	- Java virtual machine

	This profile integrates skifta specific software components that may not
	be delivered as part of this release. If any question, please contact
	the Qualcomm-Atheros sales team for more information on the Skifta
	products & deliverables.
endef

define Profile/Skifta
	$(call Profile/Skifta/Default)
	NAME:=Qualcomm-Atheros Skifta Profile
	PACKAGES+=rygel-orb
endef

define Profile/Skifta/Description
	$(call Profile/Skifta/Description/Default)
	This profile contains the DLNA Rygel open source stack
endef
$(eval $(call Profile,Skifta))

define Profile/Skifta_Access
	NAME:=Qualcomm-Atheros Skifta Profile with NFLC DLNA stack
	$(call Profile/Skifta/Default)
	PACKAGES+=nflc
endef

define Profile/Skifta_Access/Description
	$(call Profile/Skifta/Description/Default)
	This profile contains the DLNA Access stack
endef
$(eval $(call Profile,Skifta_Access))

define Profile/Factory/Default
	PACKAGES:=-dropbear -firewall -ppp -wpad-mini alsa -luci-theme-openwrt \
		-kmod-ath9k -kmod-ath5k -kmod-ath -hostapd -wpa-supplicant \
		-hostapd-utils kmod-usb-core kmod-usb2 kmod-usb-storage \
		kmod-nls-cp437 kmod-nls-iso8859-1 tftp-hpa dumpregs \
		kmod-fs-msdos kmod-fs-ntfs kmod-fs-vfat avahi-daemon
endef

define Profile/Factory/Description/Default
	Qualcomm-Atheros Factory package set configuration
	This profile provides the package set required to run factory focus
	tools. In supports the following:
	- ART2 (customized for a certain chip)
	- USB tools
endef

define Profile/Factory_Wasp
	NAME:=Qualcomm-Atheros Factory Profile for Wasp chipset
	$(call Profile/Factory/Default)
	PACKAGES+=kmod-art2-wasp qca-romboot-cus227
endef

define Profile/Factory_Wasp/Description
	$(call Profile/Factory/Description/Default)
	This profile contains the ART2 build for Wasp based platform
endef
$(eval $(call Profile,Factory_Wasp))
