# Copyright (c) 2014, 2020 The Linux Foundation. All rights reserved.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

UBIFS_OPTS = -m 2048 -e 124KiB -c 4096 -U -F
UBI_OPTS = -m 2048 -p 128KiB

IPQ807X_32_KERNEL_LOADADDR = 0x41208000
IPQ807X_KERNEL_LOADADDR = 0x41080000

ifneq ($(CONFIG_TARGET_ROOTFS_UBIFS),)
define Image/mkfs/ubifs_fit
	$(eval UBI_OPTS:="-m 2048 -p 128KiB")
	$(call Image/mkfs/ubifs/generate,$(1))
	$(call Image/Build,ubi,$(1))
endef
endif

define Image/BuildKernel/FIT
	$(CP) $(KDIR)/Image $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.bin
	gzip -9 -c $(KDIR)/Image > $(KDIR)/Image.gz
	$(call Image/BuildKernel/MkFIT,$(1), $(KDIR)/Image.gz, $(DTS_DIR)/$(1).dtb,gzip,$(2),$(2))
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage.itb
	mkdir -p $(BIN_DIR)/dtbs/
	$(CP) $(DTS_DIR)/ipq807*.dtb $(BIN_DIR)/dtbs/
	$(CP) $(KDIR)/Image $(BIN_DIR)/dtbs/
endef

define Image/BuildKernel/FITInitramfs
	$(CP) $(KDIR)/Image-initramfs $(BIN_DIR)/dtbs/
	$(CP) $(KDIR)/Image-initramfs $(BIN_DIR)/$(IMG_PREFIX)-vmlinux-initramfs.bin
	$(call Image/BuildKernel/MkFIT,$(1), $(KDIR)/Image-initramfs, $(DTS_DIR)/$(1).dtb, none,$(2),$(2))
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage-initramfs.itb
endef

define Image/Prepare
	$(CP) $(LINUX_DIR)/vmlinux $(KDIR)/$(IMG_PREFIX)-vmlinux.elf
endef

define Image/BuildKernel/MultiDTBFIT
	$(CP) $(KDIR)/Image $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.bin
	gzip -9 -c $(KDIR)/Image > $(KDIR)/Image.gz
	$(foreach dtb, $(2), gzip -9 -c $(DTS_DIR)/$(dtb).dtb > $(DTS_DIR)/$(dtb).dtb.gz;)
	$(call Image/BuildKernel/MkFITMulti,$(1),$(KDIR)/Image.gz,$(2),gzip,$(3),$(3),$(4),$(5))
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage.itb
endef

# some tree's don't have all the device tree's so we filter out
# the ones that are not present
# $(1) is the filter to apply
FindDeviceTrees ?= $(notdir $(basename $(shell find $(DTS_DIR)/ -name $(1)*.dtb $(2) 2>/dev/null)))

define Image/BuildKernel/template
	$(CP) $(KDIR)/vmlinux$(subst openwrt-ipq807x$(if $(SUBTARGET),-$(SUBTARGET)),,$(IMG_PREFIX)).debug $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.elf
endef

define Image/BuildKernel/ipq807x_32
	$(foreach device, $(call FindDeviceTrees, ipq8074-), \
		$(call Image/BuildKernel/$(1),$(device),$(IPQ807X_32_KERNEL_LOADADDR));)

	$(call Image/BuildKernel/MultiDTBFIT,ipq8074-hkxx, \
		$(call FindDeviceTrees, ipq8074-), $(IPQ807X_32_KERNEL_LOADADDR))
endef

define Image/BuildKernel/generic
	$(foreach device, $(call FindDeviceTrees, ipq8074-), \
		$(call Image/BuildKernel/$(1),$(device),$(IPQ807X_KERNEL_LOADADDR));)

	$(call Image/BuildKernel/MultiDTBFIT,ipq8074-hkxx, \
		$(call FindDeviceTrees, ipq8074-), $(IPQ807X_KERNEL_LOADADDR))
endef

define Image/BuildKernel
	$(call Image/BuildKernel/template,FIT)
	$(call Image/BuildKernel/$(SUBTARGET),FIT)
endef

define Image/BuildKernel/Initramfs
	$(CP) $(KDIR)/vmlinux-initramfs$(subst openwrt-ipq807x$(if $(SUBTARGET),-$(SUBTARGET)),,$(IMG_PREFIX)).debug $(BIN_DIR)/$(IMG_PREFIX)-vmlinux-initramfs.elf
	$(call Image/BuildKernel/template,FITInitramfs)
	$(call Image/BuildKernel/$(SUBTARGET),FITInitramfs)
endef

define Image/Build/ubifs
	$(call Image/mkfs/ubifs_fit,-$(SUBTARGET))
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	dd if=$(KDIR)/root$(2).$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-$(1)-root$(3).img bs=2k conv=sync
endef

$(eval $(call BuildImage))
