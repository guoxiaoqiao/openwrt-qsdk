# Copyright (c) 2014 The Linux Foundation. All rights reserved.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

UBIFS_OPTS = -m 2048 -e 124KiB -c 4096 -U -F
UBI_OPTS = -m 2048 -p 128KiB

E2SIZE=$(shell echo $$(($(CONFIG_TARGET_ROOTFS_PARTSIZE)*1024)))

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
ifeq ($(3),compressed-dtb)
	gzip -9 -c $(LINUX_DIR)/arch/arm/boot/dts/$(1).dtb > $(LINUX_DIR)/arch/arm/boot/dts/$(1).dtb.gz
	$(call Image/BuildKernel/MkFIT,$(1), $(KDIR)/Image.gz, $(LINUX_DIR)/arch/arm/boot/dts/$(1).dtb.gz,gzip,$(2),$(2),,$(4))
else
	$(call Image/BuildKernel/MkFIT,$(1), $(KDIR)/Image.gz, $(LINUX_DIR)/arch/arm/boot/dts/$(1).dtb,gzip,$(2),$(2))
endif
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage.itb
	$(CP) $(LINUX_DIR)/arch/arm/boot/dts/$(1).dtb $(BIN_DIR)/$(IMG_PREFIX)-$(1).dtb
	mkdir -p $(BIN_DIR)/dtbs/
	$(CP) $(LINUX_DIR)/arch/arm/boot/dts/qcom-ipq8064*.dtb $(BIN_DIR)/dtbs/
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(CP) $(KDIR)/Image-initramfs $(BIN_DIR)/$(IMG_PREFIX)-vmlinux-initramfs.bin
	$(call Image/BuildKernel/MkFIT,$(1), $(KDIR)/Image-initramfs, $(LINUX_DIR)/arch/arm/boot/dts/$(1).dtb, none,$(2),$(2))
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage-initramfs.itb
endif
endef

define Image/BuildKernel/GenericFIT
	$(CP) $(KDIR)/Image $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.bin
	gzip -9 -c $(KDIR)/Image > $(KDIR)/Image.gz
	$(call CompressLzma,$(KDIR)/Image,$(KDIR)/Image.gz)
	$(call Image/BuildKernel/MkFIT,$(1), $(KDIR)/Image.gz,,gzip,$(2),$(2))
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage.itb
endef

define Image/BuildKernel/MultiDTBFIT

ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(CP) $(KDIR)/Image-initramfs $(BIN_DIR)/$(IMG_PREFIX)-vmlinux-initramfs.bin
	$(call Image/BuildKernel/MkFITMulti,$(1),$(KDIR)/Image-initramfs,$(2),none,$(3),$(3))
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage-initramfs.itb
endif
	$(CP) $(KDIR)/Image $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.bin
	gzip -9 -c $(KDIR)/Image > $(KDIR)/Image.gz
	$(call CompressLzma,$(KDIR)/Image,$(KDIR)/Image.gz)
	$(call Image/BuildKernel/MkFITMulti,$(1),$(KDIR)/Image.gz,$(2),gzip,$(3),$(3))
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage.itb
endef

define Image/BuildKernel
	$(CP) $(KDIR)/vmlinux$(subst openwrt-ipq806x,,$(IMG_PREFIX)).debug $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.elf
	$(call Image/BuildKernel/FIT,qcom-ipq8064-storm,0x42208000)
	$(call Image/BuildKernel/FIT,qcom-ipq8064-whirlwind,0x42208000)
	$(call Image/BuildKernel/FIT,qcom-ipq40xx-r3pc,0x80208000)
	$(call Image/BuildKernel/FIT,qcom-ipq40xx-r3pc-wifi,0x80208000)
	$(call Image/BuildKernel/FIT,qcom-ipq40xx-ap.dk01.1-c1,0x80208000)
	$(call Image/BuildKernel/FIT,qcom-ipq40xx-ap.dk01.1-c2,0x80208000)
	$(call Image/BuildKernel/FIT,qcom-ipq40xx-ap.dk04.1-c1,0x80208000)
	$(call Image/BuildKernel/FIT,qcom-ipq40xx-ap.dk06.1-c1,0x80208000)
	$(call Image/BuildKernel/FIT,qcom-ipq40xx-ap.dk04.1-c2,0x80208000)
	$(call Image/BuildKernel/FIT,qcom-ipq40xx-ap.dk04.1-c3,0x80208000)
	$(call Image/BuildKernel/FIT,qcom-ipq40xx-ap.dk04.1-c4,0x80208000)
	$(call Image/BuildKernel/FIT,qcom-ipq40xx-db.dk01.1-c1,0x80208000)
	$(call Image/BuildKernel/FIT,qcom-ipq40xx-db.dk02.1-c1,0x80208000)
	$(call Image/BuildKernel/GenericFIT,qcom-ipq40xx,0x80208000)
	$(call Image/BuildKernel/MultiDTBFIT,qcom-ipq40xx-ap.dkxx,"qcom-ipq40xx-ap.dk04.1-c1 \
																qcom-ipq40xx-ap.dk04.1-c2 \
																qcom-ipq40xx-ap.dk04.1-c3 \
																qcom-ipq40xx-ap.dk01.1-c1 \
																qcom-ipq40xx-ap.dk01.1-c2 \
																qcom-ipq40xx-db.dk01.1-c1 \
																qcom-ipq40xx-db.dk02.1-c1 \
																qcom-ipq40xx-ap.dk04.1-c4 \
																qcom-ipq40xx-ap.dk06.1-c1"\
																,0x80208000)
	$(call Image/BuildKernel/MultiDTBFIT,qcom-ipq8064-db149,"qcom-ipq8064-db149 \
								qcom-ipq8064-v2.0-db149 \
								qcom-ipq8064-v3.0-db149"\
								,0x42208000)
	$(call Image/BuildKernel/MultiDTBFIT,qcom-ipq8064-ap148,"qcom-ipq8064-ap148 \
								qcom-ipq8064-v2.0-ap148 \
								qcom-ipq8064-v3.0-ap148"\
								,0x42208000)
	$(call Image/BuildKernel/MultiDTBFIT,qcom-ipq8064-ak01,"qcom-ipq8064-ak01 \
								qcom-ipq8064-v2.0-ak01 \
								qcom-ipq8064-v3.0-ak01"\
								,0x42208000)
	$(call Image/BuildKernel/MultiDTBFIT,qcom-ipq8064-ap145_1xx,"qcom-ipq8064-ap145_1xx \
								qcom-ipq8064-v2.0-ap145_1xx \
								qcom-ipq8064-v3.0-ap145_1xx"\
								,0x42208000)
	$(call Image/BuildKernel/MultiDTBFIT,qcom-ipq8064-ap145,"qcom-ipq8064-ap145 \
								qcom-ipq8064-v2.0-ap145 \
								qcom-ipq8064-v3.0-ap145"\
								,0x42208000)
	$(call Image/BuildKernel/MultiDTBFIT,qcom-ipq8064-ap160,"qcom-ipq8064-ap160 \
								qcom-ipq8064-v2.0-ap160 \
								qcom-ipq8064-v3.0-ap160"\
								,0x42208000)
	$(call Image/BuildKernel/MultiDTBFIT,qcom-ipq8064-ap161,"qcom-ipq8064-ap161 \
								qcom-ipq8064-v2.0-ap161 \
								qcom-ipq8064-v3.0-ap161"\
								,0x42208000)
	$(call Image/BuildKernel/MultiDTBFIT,qcom-ipq8064-ap160_2xx,"qcom-ipq8064-ap160_2xx \
								qcom-ipq8064-v2.0-ap160_2xx \
								qcom-ipq8064-v3.0-ap160_2xx"\
								,0x42208000)
	$(call Image/BuildKernel/MultiDTBFIT,qcom-ipq8064-ap148_1xx,"qcom-ipq8064-ap148_1xx \
								qcom-ipq8064-v2.0-ap148_1xx \
								qcom-ipq8064-v3.0-ap148_1xx"\
								,0x42208000)
endef

define Image/UbiImage

ifeq ($(1),-ipq806x)
	$(call Image/Build/UbinizeImage,qcom-ipq8064-db149,$(BIN_DIR)/$(IMG_PREFIX)-qcom-ipq8064-db149-fit-uImage.itb,squashfs,$(UBI_OPTS))
	$(call Image/Build/UbinizeImage,qcom-ipq8064-ap148,$(BIN_DIR)/$(IMG_PREFIX)-qcom-ipq8064-ap148-fit-uImage.itb,squashfs,$(UBI_OPTS))
	$(call Image/Build/UbinizeImage,qcom-ipq8064-ap148_1xx,$(BIN_DIR)/$(IMG_PREFIX)-qcom-ipq8064-ap148_1xx-fit-uImage.itb,squashfs,$(UBI_OPTS))
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
	dd if=$(KDIR)/root$(2).$(1) of=$(BIN_DIR)/$(IMG_PREFIX)$(2)-$(1)-root.img bs=128k conv=sync
endef

$(eval $(call BuildImage))
