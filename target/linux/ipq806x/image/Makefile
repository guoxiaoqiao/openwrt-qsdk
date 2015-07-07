# Copyright (c) 2014 The Linux Foundation. All rights reserved.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

UBIFS_OPTS = -m 2048 -e 124KiB -c 4096 -U -F
UBINIZE_OPTS = -m 2048 -p 128KiB

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
endef
endif

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

define Image/BuildKernel
	$(CP) $(KDIR)/vmlinux$(subst openwrt-ipq806x,,$(IMG_PREFIX)).debug $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.elf
	$(call Image/BuildKernel/FIT,qcom-ipq8064-ap148,$(IPQ806X_KERNEL_LOADADDR))
	$(call Image/BuildKernel/FIT,qcom-ipq8064-v2.0-ap148,$(IPQ806X_KERNEL_LOADADDR))
	$(call Image/BuildKernel/FIT,qcom-ipq8064-db149,$(IPQ806X_KERNEL_LOADADDR))
	$(call Image/BuildKernel/FIT,qcom-ipq8064-storm,$(IPQ806X_KERNEL_LOADADDR))
	$(call Image/BuildKernel/FIT,qcom-ipq8064-whirlwind,$(IPQ806X_KERNEL_LOADADDR))
	$(call Image/BuildKernel/FIT,qcom-ipq40xx-r3pc,$(IPQ40XX_KERNEL_LOADADDR))
	$(call Image/BuildKernel/FIT,qcom-ipq40xx-r3pc-wifi,$(IPQ40XX_KERNEL_LOADADDR))
	$(call Image/BuildKernel/FIT,qcom-ipq40xx-ap.dk01.1-c1,$(IPQ40XX_KERNEL_LOADADDR))
	$(call Image/BuildKernel/FIT,qcom-ipq40xx-ap.dk01.1-c2,$(IPQ40XX_KERNEL_LOADADDR))
	$(call Image/BuildKernel/FIT,qcom-ipq40xx-ap.dk04.1-c1,$(IPQ40XX_KERNEL_LOADADDR))
	$(call Image/BuildKernel/FIT,qcom-ipq40xx-ap.dk04.1-c2,$(IPQ40XX_KERNEL_LOADADDR))
	$(call Image/BuildKernel/FIT,qcom-ipq40xx-ap.dk04.1-c3,$(IPQ40XX_KERNEL_LOADADDR))
	$(call Image/BuildKernel/FIT,qcom-ipq40xx-ap.dk04.1-c4,$(IPQ40XX_KERNEL_LOADADDR))
	$(call Image/BuildKernel/FIT,qcom-ipq40xx-db.dk01.1-c1,$(IPQ40XX_KERNEL_LOADADDR))
	$(call Image/BuildKernel/FIT,qcom-ipq40xx-db.dk02.1-c1,$(IPQ40XX_KERNEL_LOADADDR))
	$(call Image/BuildKernel/GenericFIT,qcom-ipq40xx,$(IPQ40XX_KERNEL_LOADADDR))
	$(call Image/BuildKernel/MultiDTBFIT,qcom-ipq40xx-ap.dkxx,"qcom-ipq40xx-ap.dk04.1-c1 \
												qcom-ipq40xx-ap.dk04.1-c2 \
												qcom-ipq40xx-ap.dk04.1-c3 \
												qcom-ipq40xx-ap.dk01.1-c1 \
												qcom-ipq40xx-ap.dk01.1-c2 \
												qcom-ipq40xx-db.dk01.1-c1 \
												qcom-ipq40xx-db.dk02.1-c1 \
												qcom-ipq40xx-ap.dk04.1-c4"\
												,$(IPQ40XX_KERNEL_LOADADDR))
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	dd if=$(KDIR)/root$(2).$(1) of=$(BIN_DIR)/$(IMG_PREFIX)$(2)-$(1)-root.img bs=2k conv=sync
endef

$(eval $(call BuildImage))
