# Copyright (c) 2014 The Linux Foundation. All rights reserved.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

UBIFS_OPTS = -m 2048 -e 124KiB -c 4096 -U -F
UBINIZE_OPTS = -m 2048 -p 128KiB

E2SIZE=$(shell echo $$(($(CONFIG_TARGET_ROOTFS_PARTSIZE)*1024)))

define Image/BuildKernel/FIT
	$(CP) $(LINUX_DIR)/arch/arm/boot/dts/$(1).dtb $(BIN_DIR)/$(IMG_PREFIX)-$(1).dtb
	$(call Image/BuildKernel/MkFIT,$(1),$(LINUX_DIR)/arch/arm/boot/zImage,$(BIN_DIR)/$(IMG_PREFIX)-$(1).dtb,none,0x40A08000,0x40A08000)
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage.itb
endef

define Image/BuildKernel
	$(CP) $(LINUX_DIR)/vmlinux $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.elf
	$(CP) $(LINUX_DIR)/arch/arm/boot/Image $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.bin
	$(CP) $(LINUX_DIR)/arch/arm/boot/uImage $(BIN_DIR)/$(IMG_PREFIX)-$(KERNEL)-uImage
	$(CP) $(LINUX_DIR)/arch/arm/boot/dts/qcom-ipq8064-ap148.dtb $(BIN_DIR)/$(IMG_PREFIX)-qcom-ipq8064-ap148.dtb
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-$(1)-root.img bs=2k conv=sync
endef

$(eval $(call BuildImage))
