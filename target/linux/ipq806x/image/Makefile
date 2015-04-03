# Copyright (c) 2014 The Linux Foundation. All rights reserved.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

UBIFS_OPTS = -m 2048 -e 124KiB -c 4096 -U -F
UBINIZE_OPTS = -m 2048 -p 128KiB

E2SIZE=$(shell echo $$(($(CONFIG_TARGET_ROOTFS_PARTSIZE)*1024)))

define Image/BuildKernel/FIT
	$(CP) $(KDIR)/Image $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.bin
	gzip -9 -c $(KDIR)/Image > $(KDIR)/Image.gz
	$(call CompressLzma,$(KDIR)/Image,$(KDIR)/Image.gz)
	$(call Image/BuildKernel/MkFIT,$(1), $(KDIR)/Image.gz, $(LINUX_DIR)/arch/arm/boot/dts/$(1).dtb,gzip,$(2),$(2))
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage.itb
	$(CP) $(LINUX_DIR)/arch/arm/boot/dts/$(1).dtb $(BIN_DIR)/$(IMG_PREFIX)-$(1).dtb
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(CP) $(KDIR)/Image-initramfs $(BIN_DIR)/$(IMG_PREFIX)-vmlinux-initramfs.bin
	$(call Image/BuildKernel/MkFIT,$(1), $(KDIR)/Image-initramfs, $(LINUX_DIR)/arch/arm/boot/dts/$(1).dtb, none,$(2),$(2))
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage-initramfs.itb
endif
endef

define Image/BuildKernel
	$(CP) $(KDIR)/vmlinux$(subst openwrt-ipq806x,,$(IMG_PREFIX)).debug $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.elf
	$(call Image/BuildKernel/FIT,qcom-ipq8064-ap148,0x42208000)
	$(call Image/BuildKernel/FIT,qcom-ipq8064-v2.0-ap148,0x42208000)
	$(call Image/BuildKernel/FIT,qcom-ipq8064-storm,0x42208000)
	$(call Image/BuildKernel/FIT,qcom-ipq8064-whirlwind,0x42208000)
	$(call Image/BuildKernel/FIT,qcom-qca961x-r3pc,0x80208000)
	$(call Image/BuildKernel/FIT,qcom-qca961x-r3pc-wifi,0x80208000)
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-$(1)-root.img bs=128k conv=sync
endef

$(eval $(call BuildImage))
