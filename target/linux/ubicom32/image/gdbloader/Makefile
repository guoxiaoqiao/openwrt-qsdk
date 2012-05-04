include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

LOADER_DIR:=$(CONFIG_GDBLOADER_DIR)

all: compile

clean:

compile:
	$(MAKE) -C $(LOADER_DIR) ARCH_EXTENSION=IP8000_PROD
