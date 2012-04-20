#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/VPNRouter
  NAME:=VPNRouter
  PACKAGES:=openswan openvpn openvpn-easy-rsa luci-app-openvpn kmod-Ubicom_WISH wish
endef

define Profile/VPNRouter/Description
	Ubicom VPNRouter Profile
endef
$(eval $(call Profile,VPNRouter))

