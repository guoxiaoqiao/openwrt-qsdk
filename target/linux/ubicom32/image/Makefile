include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

BLOCKSIZE:=256k
JFFS2_BLOCKSIZE:=$(BLOCKSIZE)

ULTRA_DIR:=$(CONFIG_ULTRA_DIR)
LOADER_DIR:=$(CONFIG_GDBLOADER_DIR)

ULTRA_TOOLS:=$(ULTRA_DIR)/tools/bin/linux
MAINEXEC_DIR:=$(ULTRA_DIR)/projects/mainexec

MKIMAGE:=$(ULTRA_TOOLS)/mkimage

IMAGE_LIST_MKIMAGE:=$(BIN_DIR)/mainexec.gz:$(KDIR)/initrd.gz:$(KDIR)/vmlinux.gz

# ramdisk is compiled into kernel
define Build/initramfs
	gzip -9 < /dev/null > $(KDIR)/initrd.gz
endef

define Build/clean
	$(MAKE) -C ultra clean
endef

define Image/Prepare
	$(MAKE) -C gdbloader compile
	$(MAKE) -C ultra compile
	$(call Build/initramfs)
endef

define Image/BuildKernel
	$(TARGET_CROSS)objcopy -O binary --gap-fill 0xff \
		--strip-debug \
		$(LINUX_DIR)/vmlinux $(KDIR)/vmlinux.bin

	gzip -9 < $(KDIR)/vmlinux.bin > $(KDIR)/vmlinux.gz

	$(MKIMAGE) -A ubicom32 -O linux -T multi -C gzip \
		-a 0xc0100000 -e 0xc0100000 \
		-n $(CONFIG_IDENTITY_STRING) -d $(IMAGE_LIST_MKIMAGE) $(BIN_DIR)/upgrade.ub
	$(call Image/Build/pack,$(BIN_DIR)/upgrade.ub)
endef

# Add the GDBLoader so we can push the firmware image via GDB
define Image/Build/pack
	$(TARGET_CROSS)objcopy \
	--only-keep-debug \
	--add-section .image=$(1) \
		--change-section-address=.image=0xb0080000 --set-section-flags \
		.image=contents,load \
	--add-section .downloader=$(LOADER_DIR)/loader.bin \
		--change-section-address=.downloader=0xd0000000 --set-section-flags \
		.downloader=contents,load \
	$(LINUX_DIR)/vmlinux $(1).elf
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	( \
		dd if=$(BIN_DIR)/upgrade.ub bs=$(BLOCKSIZE) conv=sync; \
		dd if=$(KDIR)/root.$(1)     bs=$(BLOCKSIZE) conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-$(1).img
	$(call Image/Build/pack,$(BIN_DIR)/$(IMG_PREFIX)-$(1).img)
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

$(eval $(call BuildImage))
