
ARCH:=aarch64
CPU_TYPE:=cortex-a53
SUBTARGET:=ipq50xx_64
BOARDNAME:=QCA IPQ50XX(64bit) based boards
KERNELNAME:=Image dtbs

DEFAULT_PACKAGES += \
	sysupgrade-helper kmod-usb-dwc3-of-simple

define Target/Description
        Build images for IPQ50xx 64 bit system.
endef
