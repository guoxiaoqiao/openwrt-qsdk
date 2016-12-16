include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/version.mk

PKG_NAME:=u-boot
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_PROTO:=git
PKG_BRANCH:=master
PKG_RELEASE:=1

include $(INCLUDE_DIR)/local-development.mk
ifeq ($(DUMP)$(PKG_VERSION),)
  PKG_REV:=$(shell git ls-remote $(PKG_SOURCE_URL) $(PKG_BRANCH) | cut -b -7)
  PKG_VERSION:=g$(PKG_REV)
endif

include $(INCLUDE_DIR)/package.mk

define Package/uboot-2016-ipq806x
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=@TARGET_ipq806x
  TITLE:=U-boot for IPQ806x based platforms
  URL:=http://www.denx.de/wiki/U-Boot
  VARIANT:=ipq806x
endef

define Package/uboot-2016-ipq807x
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=@TARGET_ipq806x||@TARGET_ipq807x
  TITLE:=U-boot for IPQ807x based platforms
  URL:=http://www.denx.de/wiki/U-Boot
  VARIANT:=ipq807x
endef

define Package/uboot-2016-ipq40xx
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=@TARGET_ipq806x
  TITLE:=U-boot for IPQ40xx based platforms
  URL:=http://www.denx.de/wiki/U-Boot
  VARIANT:=ipq40xx
endef

define Build/Configure
	$(MAKE) -C $(PKG_BUILD_DIR) $(BUILD_VARIANT)_defconfig
endef

define Build/Compile
	VERSION_CODE='$(VERSION_NICK) $(VERSION_NUMBER)' \
	REVISION='$(REVISION)' \
	$(MAKE) -C $(PKG_BUILD_DIR) HOSTSTRIP=true CROSS_COMPILE=$(TARGET_CROSS) \
		QSDK_CFLAGS="$(TARGET_CFLAGS)" \
		QSDK_LDFLAGS="$(TARGET_LDFLAGS)" \
		DTC=${KERNEL_BUILD_DIR}/../../../qca/src/linux-4.4/scripts/dtc/dtc \
		all
endef

define Package/uboot-2016-ipq806x/install
	$(INSTALL_DIR) $(1)
	$(CP) $(PKG_BUILD_DIR)/u-boot $(BIN_DIR)/openwrt-ipq806x-u-boot.elf
	$(CP) $(PKG_BUILD_DIR)/u-boot.bin $(BIN_DIR)/openwrt-ipq806x-u-boot.img
	$(CP) $(PKG_BUILD_DIR)/u-boot.mbn $(BIN_DIR)/openwrt-ipq806x-u-boot.mbn
endef

define Package/uboot-2016-ipq807x/install
	$(INSTALL_DIR) $(1)
	$(CP) $(PKG_BUILD_DIR)/u-boot $(BIN_DIR)/openwrt-ipq807x-u-boot.elf
	$(CP) $(PKG_BUILD_DIR)/u-boot $(BIN_DIR)/openwrt-ipq807x-u-boot-stripped.elf
	$(STRIP) $(BIN_DIR)/openwrt-ipq807x-u-boot-stripped.elf
	$(CP) $(PKG_BUILD_DIR)/u-boot.bin $(BIN_DIR)/openwrt-ipq807x-u-boot.img
endef

define Package/uboot-2016-ipq40xx/install
	$(INSTALL_DIR) $(1)
	$(CP) $(PKG_BUILD_DIR)/u-boot $(BIN_DIR)/openwrt-ipq40xx-u-boot.elf
	$(CP) $(PKG_BUILD_DIR)/u-boot $(BIN_DIR)/openwrt-ipq40xx-u-boot-stripped.elf
	$(STRIP) $(BIN_DIR)/openwrt-ipq40xx-u-boot-stripped.elf
	$(CP) $(PKG_BUILD_DIR)/u-boot.bin $(BIN_DIR)/openwrt-ipq40xx-u-boot.img
endef


$(eval $(call BuildPackage,uboot-2016-ipq807x))
$(eval $(call BuildPackage,uboot-2016-ipq40xx))
$(eval $(call BuildPackage,uboot-2016-ipq806x))

