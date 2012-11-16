#
# Copyright (C) 2012 Qualcomm Atheros
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=makefun

include $(INCLUDE_DIR)/host-build.mk

define Host/Compile
	true
endef

define Host/Install
	$(INSTALL_BIN) ./src/makefun $(STAGING_DIR_HOST)/bin/
endef

$(eval $(call HostBuild))
