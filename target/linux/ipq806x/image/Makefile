# Copyright (c) 2014 The Linux Foundation. All rights reserved.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

UBIFS_OPTS = -m 2048 -e 124KiB -c 4096 -U -F
UBI_OPTS = -m 2048 -p 128KiB

E2SIZE=$(shell echo $$(($(CONFIG_TARGET_ROOTFS_PARTSIZE)*1024)))

ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,3.18.0)),1)
IPQ806X_KERNEL_LOADADDR = 0x42008000
IPQ40XX_KERNEL_LOADADDR = 0x80008000
else
IPQ806X_KERNEL_LOADADDR = 0x42208000
IPQ40XX_KERNEL_LOADADDR = 0x80208000
endif

ifneq ($(CONFIG_TARGET_ROOTFS_UBIFS),)
define Image/mkfs/ubifs_fit
	$(call Image/mkfs/ubifs/generate,$(1))
	$(call Image/Build,ubi,$(1))
	$(call Image/UbiImage,$(1))
endef
endif

define Image/BuildKernel/FIT
	$(CP) $(KDIR)/Image $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.bin
	gzip -9 -c $(KDIR)/Image > $(KDIR)/Image.gz
	$(call CompressLzma,$(KDIR)/Image,$(KDIR)/Image.gz)
	$(call Image/BuildKernel/MkFIT,$(1), $(KDIR)/Image.gz, $(LINUX_DIR)/arch/arm/boot/dts/$(1).dtb,gzip,$(2),$(2))
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage.itb
	$(CP) $(LINUX_DIR)/arch/arm/boot/dts/$(1).dtb $(BIN_DIR)/$(IMG_PREFIX)-$(1).dtb
endef

define Image/BuildKernel/FITInitramfs
	$(CP) $(KDIR)/Image-initramfs $(BIN_DIR)/$(IMG_PREFIX)-vmlinux-initramfs.bin
	$(call Image/BuildKernel/MkFIT,$(1), $(KDIR)/Image-initramfs, $(LINUX_DIR)/arch/arm/boot/dts/$(1).dtb, none,$(2),$(2))
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage-initramfs.itb
endef

define Image/Prepare
	$(CP) $(LINUX_DIR)/vmlinux $(KDIR)/$(IMG_PREFIX)-vmlinux.elf
endef

define Image/BuildKernel/GenericFIT
	$(CP) $(KDIR)/Image $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.bin
	gzip -9 -c $(KDIR)/Image > $(KDIR)/Image.gz
	$(call CompressLzma,$(KDIR)/Image,$(KDIR)/Image.gz)
	$(call Image/BuildKernel/MkFIT,$(1), $(KDIR)/Image.gz,,gzip,$(2),$(2))
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage.itb
endef

define Image/BuildKernel/MultiDTBFIT
	$(CP) $(KDIR)/Image $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.bin
	gzip -9 -c $(KDIR)/Image > $(KDIR)/Image.gz
	$(call CompressLzma,$(KDIR)/Image,$(KDIR)/Image.gz)
	$(call Image/BuildKernel/MkFITMulti,$(1),$(KDIR)/Image.gz,$(2),gzip,$(3),$(3))
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage.itb
endef

# some tree's don't have all the device tree's so we filter out
# the ones that are not present
# $(1) is the filter to apply
FindDeviceTrees ?= $(notdir $(basename $(shell find $(LINUX_DIR)/arch/arm/boot/dts/ -name $(1)*.dts 2>/dev/null)))

define Image/BuildKernel/template
	$(CP) $(KDIR)/vmlinux$(subst openwrt-ipq806x,,$(IMG_PREFIX)).debug $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.elf

	$(foreach device, $(call FindDeviceTrees, qcom-ipq8064-), \
		$(call Image/BuildKernel/$(1),$(device),$(IPQ806X_KERNEL_LOADADDR));)

	$(foreach device, $(call FindDeviceTrees, qcom-ipq40xx-), \
		$(call Image/BuildKernel/$(1),$(device),$(IPQ40XX_KERNEL_LOADADDR));)
endef

define Image/BuildKernel
	$(call Image/BuildKernel/template,FIT)

	$(call Image/BuildKernel/GenericFIT,qcom-ipq40xx,$(IPQ40XX_KERNEL_LOADADDR))
	$(call Image/BuildKernel/MultiDTBFIT,qcom-ipq40xx-ap.dkxx, \
		$(call FindDeviceTrees, qcom-ipq40xx-ap) $(call FindDeviceTrees, qcom-ipq40xx-db), \
		$(IPQ40XX_KERNEL_LOADADDR))
endef

define Image/BuildKernel/Initramfs
	$(call Image/BuildKernel/template,FITInitramfs)
endef

define Image/UbiImage

ifeq ($(1),-ipq806x)
	$(call Image/Build/UbinizeImage,qcom-ipq8064-db149,$(BIN_DIR)/$(IMG_PREFIX)-qcom-ipq8064-db149-fit-uImage.itb,squashfs,$(UBI_OPTS))
	$(call Image/Build/UbinizeImage,qcom-ipq8064-ap148,$(BIN_DIR)/$(IMG_PREFIX)-qcom-ipq8064-ap148-fit-uImage.itb,squashfs,$(UBI_OPTS))
	$(call Image/Build/UbinizeImage,qcom-ipq8064-ak01,$(BIN_DIR)/$(IMG_PREFIX)-qcom-ipq8064-ak01-fit-uImage.itb,squashfs,$(UBI_OPTS))
	$(call Image/Build/UbinizeImage,qcom-ipq8064-ap145_1xx,$(BIN_DIR)/$(IMG_PREFIX)-qcom-ipq8064-ap145_1xx-fit-uImage.itb,squashfs,$(UBI_OPTS))
	$(call Image/Build/UbinizeImage,qcom-ipq8064-ap145,$(BIN_DIR)/$(IMG_PREFIX)-qcom-ipq8064-ap145-fit-uImage.itb,squashfs,$(UBI_OPTS))
	$(call Image/Build/UbinizeImage,qcom-ipq8064-ap160,$(BIN_DIR)/$(IMG_PREFIX)-qcom-ipq8064-ap160-fit-uImage.itb,squashfs,$(UBI_OPTS))
	$(call Image/Build/UbinizeImage,qcom-ipq8064-ap161,$(BIN_DIR)/$(IMG_PREFIX)-qcom-ipq8064-ap161-fit-uImage.itb,squashfs,$(UBI_OPTS))
	$(call Image/Build/UbinizeImage,qcom-ipq8064-ap160_2xx,$(BIN_DIR)/$(IMG_PREFIX)-qcom-ipq8064-ap160_2xx-fit-uImage.itb,squashfs,$(UBI_OPTS))
endif
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	dd if=$(KDIR)/root$(2).$(1) of=$(BIN_DIR)/$(IMG_PREFIX)$(2)-$(1)-root.img bs=2k conv=sync
endef

$(eval $(call BuildImage))
