include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

ULTRA_DIR:=$(CONFIG_ULTRA_DIR)
ULTRA_TOOLS:=$(ULTRA_DIR)/tools/bin/linux
MAINEXEC_DIR:=$(ULTRA_DIR)/projects/mainexec
ULTRA_BOARD_FILE=$(ULTRA_DIR)/boards/$(CONFIG_BOARD_NAME)

PKG_NAME:=ultra
PKG_FILE_DEPENDS:=$(ULTRA_DIR)

all: compile

clean:
	$(MAKE) -C $(MAINEXEC_DIR) clean

prepare:
	$(ULTRA_TOOLS)/ctool \
		-o $(MAINEXEC_DIR)/config/$(CONFIG_LPJ_NAME) \
		-b $(ULTRA_BOARD_FILE) \
		-c SDK_DIR $(ULTRA_DIR) \
		-c APP_IDENTITY_STRING $(CONFIG_IDENTITY_STRING) \
		-y -g

compile: prepare
	$(MAKE) -s -C $(MAINEXEC_DIR) default
	mkdir -p $(BIN_DIR)
	$(TARGET_CROSS)objcopy -O binary --gap-fill 0xff -R .ocm_stack \
		$(MAINEXEC_DIR)/ultra.elf $(BIN_DIR)/mainexec.bin
	gzip -9 < $(BIN_DIR)/mainexec.bin > $(BIN_DIR)/mainexec.gz
