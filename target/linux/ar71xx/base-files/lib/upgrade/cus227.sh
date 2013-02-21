USE_REFRESH=

# make sure we got the tools we need during the fw upgrade process
platform_add_ramfs_cus227_tools()
{
	install_bin /usr/sbin/fw_printenv /usr/sbin/fw_setenv
	install_bin /bin/busybox /usr/bin/cut
	install_bin /usr/bin/md5sum
	install_file /etc/fw_env.config
}
append sysupgrade_pre_upgrade platform_add_ramfs_cus227_tools

get_mtdpart_size() {
	local varname=$1
	local partname=$2

	local size
	size=$(grep ${partname} /proc/mtd | cut -f2 -d' ')
	eval "${varname}=${size}"
}

# This function read the MTD mapping in /proc/mtd and output
# its offset in flash
# @1) (OUTPUT) variable to store the offset to
# @2) partition name
get_mtdpart_offset() {
	local varname=$1
	local partname=$2
	local offset=0
	local mtdsize=0

	# Iterate over every line in mtd until the partition name matches
	local line
	while read line && echo ${line} | grep -vq ".*\"${partname}\""; do
		# If the line doesn't start with mtd* (like first line)
		# we just skip it
		echo ${line} | grep -q "^mtd.*" || continue
		# Here we extract the size info and add it to the previous offset
		mtdsize=$(echo ${line}|cut -f2 -d' ')
		offset=$(printf '%x' $((0x${offset} + 0x${mtdsize})))
	done < /proc/mtd
	eval "${varname}=${offset}"
}

cus227_update_u_boot_env() {
	local file="$1"

	local kernel_addr kernel_size
	local rootfs_offset
	local rootfs_addr rootfs_size rootfs_md5sum
	local bootargs bootcmd

	# Perform sanity checks to make sure we've got all the tools we need
	[ -x /usr/sbin/fw_printenv -a -x /usr/sbin/fw_setenv -a \
	  -x /bin/grep -a -x /usr/bin/cut -a -x /usr/bin/md5sum ] || {
		v "Error: missing tools to perform FW upgrade"
		return 1
	}
	[ -f /etc/fw_env.config ] || {
		v "/etc/fw_env.config does not exist"
		return 1
	}

	get_mtdpart_offset kernel_addr "k-2"
	get_mtdpart_offset rootfs_addr "r-2"
	get_mtdpart_size kernel_size "k-2"

	local bootcmd_old=$(fw_printenv -n bootcmd)
	local bootargs_old=$(fw_printenv -n bootargs)
	local rootfs_addr_old=$(fw_printenv -n rootfs_addr)
	local rootfs_size_old=$(fw_printenv -n rootfs_size)
	local rootfs_md5sum_old=$(fw_printenv -n rootfs_md5sum)

	# We deduce the rootfs offset in the image by reading the kernel
	# size in /proc/mtd and adding 1. The sysupgrade stores firmware
	# checksum in the first 512 bytes 
	local bs=512
	rootfs_offset=$((0x${kernel_size}/${bs} + 1))
	rootfs_size=$(dd if="${file}" bs=${bs} skip=${rootfs_offset} 2>/dev/null | wc -c)
	rootfs_md5sum=$(dd if="${file}" bs=${bs} skip=${rootfs_offset} | \
		md5sum -b | cut -f1 -d' ')

	v "Updating cus-227 u-boot-env..."
	bootargs=$(fw_printenv -n bootargs | sed \
		-e 's/(kernel)/(tmp-k-1)/' \
		-e 's/(rootfs)/(tmp-r-1)/' \
		-e 's/(firmware)/(tmp-fw-1)/' \
		-e 's/(vendor)/(tmp-v-1)/' \
		\
		-e 's/(k-2)/(kernel)/' \
		-e 's/(r-2)/(rootfs)/' \
		-e 's/(fw-2)/(firmware)/' \
		-e 's/(v-2)/(vendor)/' \
		\
		-e 's/(tmp-k-1)/(k-2)/' \
		-e 's/(tmp-r-1)/(r-2)/' \
		-e 's/(tmp-fw-1)/(fw-2)/' \
		-e 's/(tmp-v-1)/(v-2)/' \
	)
	bootcmd="nboot 0x81000000 0 0x${kernel_addr}"

	fw_setenv -s - << EOF
	bootargs ${bootargs}
	bootcmd ${bootcmd}
	rootfs_addr 0x${rootfs_addr}
	rootfs_size 0x$(printf '%x' ${rootfs_size})
	rootfs_md5sum ${rootfs_md5sum}
	bootargs_old ${bootargs_old}
	bootcmd_old ${bootcmd_old}
	rootfs_addr_old ${rootfs_addr_old}
	rootfs_size_old ${rootfs_size_old}
	rootfs_md5sum_old ${rootfs_md5sum_old}
EOF
	v "done"
}

platform_do_upgrade_cus227() {
	local file=$1
	sync
	if [ "$SAVE_CONFIG" -eq 1 -a -z "$USE_REFRESH" ]; then
		dd bs=512 skip=1 if="${file}" | mtd -j "$CONF_TAR" write - fw-2
	else
		dd bs=512 skip=1 if="${file}" | mtd write - fw-2
	fi
	cus227_update_u_boot_env "${file}"
	mtd erase rootfs_data
}

