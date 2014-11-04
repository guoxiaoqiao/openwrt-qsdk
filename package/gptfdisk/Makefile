#
# Copyright (C) 2014 The Linux Foundation. All rights reserved.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=gptfdisk
PKG_RELEASE:=1
PKG_SOURCE_URL:=@SF/gptfdisk
PKG_VERSION:=0.8.10
PKG_MD5SUM:=9cf4246c181c324bdbd553fe9b348373

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz

include $(INCLUDE_DIR)/package.mk

define Package/gptfdisk
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=disc
  TITLE:=GPT fdisk
  DEPENDS:=+libuuid +libstdcpp
  URL:=http://sourceforge.net/projects/gptfdisk/
endef

define Package/gptfdisk/description
  GPT fdisk is a disk partitioning tool loosely modeled on Linux fdisk, but used
  for modifying GUID Partition Table (GPT) disks. The related FixParts utility
  fixes some common problems on Master Boot Record (MBR) disks.
endef

MAKE_OPTS:= \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS)"

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) $(MAKE_OPTS) gdisk
endef

define Package/gptfdisk/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/gdisk $(1)/usr/sbin
endef

$(eval $(call BuildPackage,gptfdisk))
