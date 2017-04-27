
SUBTARGET:=ipq806x
BOARDNAME:=QCA IPQ806x based boards
CPU_TYPE:=cortex-a7

DEFAULT_PACKAGES += \
	uboot-2016-ipq806x lk-ipq806x \
	kmod-usb-dwc3-qcom kmod-usb-phy-qcom-dwc3 kmod-usb-dwc3-of-simple

define Target/Description
	Build firmware image for IPQ806x SoC devices.
endef
