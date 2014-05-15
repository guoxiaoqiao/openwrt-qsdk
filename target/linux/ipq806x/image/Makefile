# Copyright (c) 2014 The Linux Foundation. All rights reserved.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

UBIFS_OPTS = -m 2048 -e 124KiB -c 4096 -U -F
UBINIZE_OPTS = -m 2048 -p 128KiB

E2SIZE=$(shell echo $$(($(CONFIG_TARGET_ROOTFS_PARTSIZE)*1024)))

define Image/BuildKernel/FIT
	$(call Image/BuildKernel/MkFIT,$(1), $(LINUX_DIR)/arch/arm/boot/Image, $(LINUX_DIR)/arch/arm/boot/dts/$(1).dtb, none,0x42208000,0x42208000)
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage.itb
endef

define Image/BuildKernel
	$(CP) $(LINUX_DIR)/vmlinux $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.elf
	$(call Image/BuildKernel/FIT,qcom-ipq8064-ap148)
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-$(1)-root.img bs=2k conv=sync
endef

$(eval $(call BuildImage))
