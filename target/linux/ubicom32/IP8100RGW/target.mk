BOARDNAME:=IP8100RGW
CFLAGS:=-Os -pipe -march=ubicom32v5 -DUBICOM32_ARCH_VERSION=5 -DIP8000
KERNEL_CC:=ubicom32-elf-gcc
LINUX_TARGET_CONFIG=$(PLATFORM_SUBDIR)/config-$(LINUX_VERSION).$(PROFILE)

define Target/Description
	Build firmware image for Ubicom IP8000RGW board
endef

