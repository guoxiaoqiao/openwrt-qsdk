
ARCH:=arm
SUBTARGET:=ipq807x_32
BOARDNAME:=QCA IPQ807x(32bit) based boards
CPU_TYPE:=cortex-a7

DEFAULT_PACKAGES += \
	kmod-usb-phy-ipq807x kmod-usb-dwc3-of-simple

define Target/Description
	Build firmware image for IPQ807x SoC devices.
endef
