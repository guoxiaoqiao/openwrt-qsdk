#
# Copyright (c) 2013 Qualcomm Atheros, Inc.
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
