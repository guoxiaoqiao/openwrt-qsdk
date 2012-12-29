USE_REFRESH=

cus227_update_u_boot_env() {

	local BOOTARGS BOOTCMD BOOTCMDADDR

	v "Updating cus-227 u-boot-env..."
	[ -x /usr/sbin/fw_printenv -a -x /usr/sbin/fw_setenv ] || { v "fw_printenv or fw_setenv is not executable"; return; }
	[ -f /etc/fw_env.config ] || { v "/etc/fw_env.config does not exist"; return; }
	BOOTARGS=$(fw_printenv 2>/dev/null |  awk '/^bootargs/' | sed -e 's/bootargs=//' -e 's/(kernel)/(tmp-k-1)/' -e 's/(rootfs)/(tmp-r-1)/' -e 's/(firmware)/(tmp-fw-1)/' -e 's/(k-2)/(kernel)/' -e 's/(r-2)/(rootfs)/' -e 's/(fw-2)/(firmware)/' -e 's/(tmp-k-1)/(k-2)/' -e 's/(tmp-r-1)/(r-2)/' -e 's/(tmp-fw-1)/(fw-2)/')
	BOOTCMDADDR=$(echo $BOOTARGS | awk -F ' ' '{for(i=1;i<=NF;i++) {if($i ~ /mtdparts/) print $i}}' | awk -F, '{for(i=1;i<=NF;i++) {if($i ~ /(firmware)/) print $i}}' | sed -e 's/(firmware)//' | awk -F@ '{print $2}')
	BOOTCMD=$(fw_printenv 2>/dev/null |  awk '/^bootcmd/' | sed -e 's/bootcmd=//' -e 's/0[xX][a-fA-F0-9]*$//')
	echo "	bootargs "$BOOTARGS"
	bootcmd "$BOOTCMD $BOOTCMDADDR"" | fw_setenv -s -
	v "done"
}

platform_do_upgrade_cus227() {
	sync
	if [ "$SAVE_CONFIG" -eq 1 -a -z "$USE_REFRESH" ]; then
		dd bs=512 skip=1 if="$1" | mtd -j "$CONF_TAR" write - fw-2
	else
		dd bs=512 skip=1 if="$1" | mtd write - fw-2
	fi
	cus227_update_u_boot_env
	mtd erase rootfs_data
}

