#
# Copyright (C) 2015 The Linux Foundation. All rights reserved.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=linux-firmware
PKG_VERSION:=20150106

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://git.kernel.org/pub/scm/linux/kernel/git/firmware/linux-firmware.git
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_VERSION:=78535e88bfb95e2f50afb6f9e728df0fbec2cddc
PKG_SOURCE_SUBDIR:=$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/ar3k
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=bluetooth ar3k firmware
endef

Build/Compile=true

define Package/ar3k/install
	$(INSTALL_DIR) $(1)/lib
	$(INSTALL_DIR) $(1)/lib/firmware
	$(CP) $(PKG_BUILD_DIR)/ar3k $(1)/lib/firmware/
endef

$(eval $(call BuildPackage,ar3k))
