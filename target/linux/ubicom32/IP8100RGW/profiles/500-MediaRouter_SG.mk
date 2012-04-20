#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/MediaRouter_SG
  NAME:=MediaRouter_SG
  PACKAGES:= samba3 luci-app-samba minidlna_binary kmod-Ubicom_WISH wish
endef

define Profile/MediaRouter_SG/Description
	Ubicom MediaRouter Profile with Media Support and S/G
endef
$(eval $(call Profile,MediaRouter_SG))

