# Copyright (c) 2013 The Linux Foundation. All rights reserved.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

UBIFS_OPTS = -m 2048 -e 124KiB -c 4096 -U -F
UBINIZE_OPTS = -m 2048 -p 128KiB
MBN_APPS_IMG = 0x13

# MBN uImage will be loaded at 0x44000000, 0x28 = size of MBN header
MBN_LOAD_ADDR = 0x44000028

E2SIZE=$(shell echo $$(($(CONFIG_TARGET_ROOTFS_PARTSIZE)*1024)))

ifneq ($(CONFIG_TARGET_ROOTFS_UBIFS),)
  define Image/mkfs/customfs
	$(CP) ./ubinize-mbn.cfg $(KDIR)
	(cd $(KDIR); \
	$(STAGING_DIR_HOST)/bin/ubinize $(UBINIZE_OPTS) -o $(KDIR)/root.ubi-mbn ubinize-mbn.cfg)
	$(call Image/Build,ubi,-mbn)
  endef
endif

define Image/BuildKernel
	$(CP) $(LINUX_DIR)/vmlinux $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.elf
	$(CP) $(LINUX_DIR)/arch/arm/boot/Image $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.bin
	dd if=$(LINUX_DIR)/arch/arm/boot/uImage of=$(KERNEL_BUILD_DIR)/uImage bs=2k conv=sync
	$(CP) $(KERNEL_BUILD_DIR)/uImage $(BIN_DIR)/$(IMG_PREFIX)-$(KERNEL)-uImage
	python $(BUILD_DIR_HOST)/firmware-utils/bin/mkmbnimg.py $(MBN_LOAD_ADDR) $(MBN_APPS_IMG) \
			$(KERNEL_BUILD_DIR)/uImage $(KERNEL_BUILD_DIR)/uImage.mbn
	$(CP) $(KERNEL_BUILD_DIR)/uImage.mbn $(BIN_DIR)/$(IMG_PREFIX)-$(KERNEL)-uImage.mbn
endef

define Image/Build
	$(call Image/Build/$(1),$(1),$(2))
	mkdir -p $(BIN_DIR)
	dd if=$(KDIR)/root.$(1)$(2) of=$(BIN_DIR)/$(IMG_PREFIX)-$(1)$(2)-root.img bs=2k conv=sync
	$(call Sysupgrade/$(1),$(2))
endef

define Sysupgrade
	$(CP) ./sysupgrade-$(1)$(2).its $(BIN_DIR)/$(IMG_PREFIX)-sysupgrade-$(1)$(2).its
	PATH=$$PATH:$(LINUX_DIR)/scripts/dtc $(STAGING_DIR_HOST)/bin/mkimage \
		-f $(BIN_DIR)/$(IMG_PREFIX)-sysupgrade-$(1)$(2).its \
		$(BIN_DIR)/$(IMG_PREFIX)-sysupgrade-$(1)$(2).bin
endef

define Sysupgrade/ubi
	$(call Sysupgrade,ubi,$(1))
endef

define Sysupgrade/squashfs
	$(call Sysupgrade,squashfs)
	$(call Sysupgrade,squashfs,-mbn)
endef

$(eval $(call BuildImage))
