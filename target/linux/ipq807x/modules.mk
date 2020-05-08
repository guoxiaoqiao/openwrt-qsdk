define KernelPackage/usb-phy-ipq807x
  TITLE:=DWC3 USB QCOM PHY driver for IPQ807x
  DEPENDS:=@TARGET_ipq807x +kmod-usb-dwc3-of-simple
  KCONFIG:= \
	CONFIG_USB_QCOM_QUSB_PHY \
	CONFIG_USB_QCOM_QMP_PHY
  FILES:= \
	$(LINUX_DIR)/drivers/usb/phy/phy-msm-qusb.ko \
	$(LINUX_DIR)/drivers/usb/phy/phy-msm-ssusb-qmp.ko
  AUTOLOAD:=$(call AutoLoad,45,phy-msm-qusb phy-msm-ssusb-qmp,1)
  $(call AddDepends/usb)
endef

define KernelPackage/usb-phy-ipq807x/description
 This driver provides support for the USB PHY drivers
 within the IPQ807x SoCs.
endef

$(eval $(call KernelPackage,usb-phy-ipq807x))

define KernelPackage/msm-mproc
  TITLE:= Default kernel configs
  DEPENDS+= @TARGET_ipq_ipq807x||TARGET_ipq_ipq807x_64||TARGET_ipq_ipq60xx||TARGET_ipq_ipq60xx_64||TARGET_ipq807x
  KCONFIG:= \
	  CONFIG_QRTR=y \
	  CONFIG_QCOM_APCS_IPC=y \
	  CONFIG_QCOM_GLINK_SSR=y \
	  CONFIG_QCOM_Q6V5_WCSS=y \
	  CONFIG_MSM_RPM_RPMSG=y \
	  CONFIG_RPMSG_QCOM_GLINK_RPM=y \
	  CONFIG_REGULATOR_RPM_GLINK=y \
	  CONFIG_IPQ_SUBSYSTEM_RAMDUMP=y \
	  CONFIG_QCOM_SYSMON=y \
	  CONFIG_RPMSG=y \
	  CONFIG_RPMSG_CHAR=y \
	  CONFIG_RPMSG_QCOM_GLINK_SMEM=y \
	  CONFIG_RPMSG_QCOM_SMD=y \
	  CONFIG_QRTR_SMD=y \
	  CONFIG_QCOM_Q6V5_SSR=y \
	  CONFIG_MAILBOX=y \
	  CONFIG_DIAG_OVER_QRTR=y
endef

define KernelPackage/msm-mproc/description
Default kernel configs.
endef

$(eval $(call KernelPackage,msm-mproc))
