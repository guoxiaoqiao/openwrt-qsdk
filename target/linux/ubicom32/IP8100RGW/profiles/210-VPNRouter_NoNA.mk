#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/VPNRouter_NoNA
  NAME:=VPNRouter_NoNA
  PACKAGES:=openswan openvpn openvpn-easy-rsa luci-app-openvpn kmod-Ubicom_WISH wish
endef

define Profile/VPNRouter_NoNA/Description
	Ubicom VPNRouter Profile without NA
endef
$(eval $(call Profile,VPNRouter_NoNA))

