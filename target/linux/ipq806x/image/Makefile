# Copyright (c) 2013 Qualcomm Atheros, Inc.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

JFFS2_BLOCKSIZE=128k
JFFS2OPTS += --little-endian --pagesize=0x800 --no-cleanmarkers --pad

E2SIZE=$(shell echo $$(($(CONFIG_TARGET_ROOTFS_PARTSIZE)*1024)))

define Image/BuildKernel
	mkimage -A arm -O linux -T kernel -C none \
		-a 80208000 -e 80208000 \
		-n "OpenWRT Kernel Image" \
		-d $(KDIR)/vmlinux $(BIN_DIR)/$(IMG_PREFIX)-$(KERNEL)-uImage

	mkimage -A arm -O linux -T script -C none \
		-a 0 -e 0 \
		-n 'uImage boot cmd' \
		-d ../files/boot.cmd $(BIN_DIR)/boot.scr

	$(CP) $(KDIR)/vmlinux $(BIN_DIR)/$(IMG_PREFIX)-$(KERNEL)-Image
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	mkdir -p $(BIN_DIR)
	$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-root.img
endef

$(eval $(call BuildImage))
