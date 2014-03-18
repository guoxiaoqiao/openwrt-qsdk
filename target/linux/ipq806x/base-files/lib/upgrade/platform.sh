#
# Copyright (c) 2013 Qualcomm Atheros, Inc.
# Copyright (C) 2011 OpenWrt.org
#

. /lib/ipq806x.sh

RAMFS_COPY_DATA=/lib/ipq806x.sh
RAMFS_COPY_BIN=/usr/bin/dumpimage

get_full_section_name() {
	local img=$1
	local sec=$2

	dumpimage -l ${img} | grep "^ Image.*(${sec}.*)" | \
		sed 's,^ Image.*(\(.*\)),\1,'
}

image_contains() {
	local img=$1
	local sec=$2

	dumpimage -l ${img} | grep -q "^ Image.*(${sec}.*)" || return 1
}

print_sections() {
	local img=$1

	dumpimage -l ${img} | awk '/^ Image.*(.*)/ { print gensub(/Image .* \((.*)\)/,"\\1", $0) }'
}

image_demux() {
	local img=$1

	for sec in $(print_sections ${img}); do
		local fullname=$(get_full_section_name ${img} ${sec})

		dumpimage -i ${img} -o /tmp/${fullname}.bin ${fullname} > /dev/null || { \
			echo "Error while extracting \"${sec}\" from ${img}"
			return 1
		}
	done
	return 0
}

image_is_FIT() {
	if ! dumpimage -l $1 > /dev/null 2>&1; then
		echo "$1 is not a valid FIT image"
		return 1
	fi
	return 0
}

switch_layout() {
	local layout=$1

	# Layout switching is only required as the  boot images (up to u-boot)
	# use 512 user data bytes per code word, whereas Linux uses 516 bytes.
	# It's only applicable for NAND flash. So let's return if we don't have
	# one.

	[ -d /sys/devices/platform/msm_nand/ ] || return

	case "${layout}" in
		boot|1) echo 1 > /sys/devices/platform/msm_nand/boot_layout;;
		linux|0) echo 0 > /sys/devices/platform/msm_nand/boot_layout;;
		*) echo "Unknown layout \"${layout}\"";;
	esac
}

do_flash_mtd() {
	local sec=$1
	local mtdname=$2

	local mtdpart=$(grep "\"${mtdname}\"" /proc/mtd | awk -F: '{print $1}')
	local pgsz=$(cat /sys/class/mtd/${mtdpart}/writesize)
	dd if=/tmp/${sec}.bin bs=${pgsz} conv=sync | mtd write - -e ${mtdname} ${mtdname}
}

flash_section() {
	local sec=$1

	local board=$(ipq806x_board_name)
	case "${sec}" in
		hlos*) switch_layout linux; do_flash_mtd ${sec} "kernel";;
		fs*) switch_layout linux; do_flash_mtd ${sec} "rootfs";;
		sbl1*) switch_layout boot; do_flash_mtd ${sec} "SBL1";;
		sbl2*) switch_layout boot; do_flash_mtd ${sec} "SBL2";;
		sbl3*) switch_layout boot; do_flash_mtd ${sec} "SBL3";;
		u-boot*) switch_layout boot; do_flash_mtd ${sec} "APPSBL";;
		ddr-${board}*) switch_layout boot; do_flash_mtd ${sec} "DDRCONFIG";;
		ssd*) switch_layout boot; do_flash_mtd ${sec} "SSD";;
		tz*) switch_layout boot; do_flash_mtd ${sec} "TZ";;
		rpm*) switch_layout boot; do_flash_mtd ${sec} "RPM";;
		*) echo "Section ${sec} ignored"; return 1;;
	esac
}

platform_check_image() {
	local board=$(ipq806x_board_name)

	local mandatory="fs"
	local optional="sbl1 sbl2 sbl3 u-boot ddr-${board} ssd tz rpm"
	local ignored="mibib"

	image_is_FIT $1 || return 1
	for sec in ${mandatory}; do
		image_contains $1 ${sec} || {\
			echo "Error: mandatory section \"${sec}\" missing from \"$1\". Abort..."
			return 1
		}
	done

	for sec in ${optional}; do
		image_contains $1 ${sec} || {\
			echo "Warning: optional section \"${sec}\" missing from \"$1\". Continue..."
		}
	done

	for sec in ${ignored}; do
		image_contains $1 ${sec} && {\
			echo "Warning: section \"${sec}\" will be ignored from \"$1\". Continue..."
		}
	done

	image_demux $1 || {\
		echo "Error: \"$1\" couldn't be extracted. Abort..."
		return 1
	}
}

platform_do_upgrade() {
	local board=$(ipq806x_board_name)

	# verify some things exist before erasing
	if [ ! -e $1 ]; then
		echo "Error: Can't find $1 after switching to ramfs, aborting upgrade!"
		reboot
	fi

	for sec in $(print_sections $1); do
		if [ ! -e /tmp/${sec}.bin ]; then
			echo "Error: Cant' find ${sec} after switching to ramfs, aborting upgrade!"
			reboot
		fi
	done

	case "$board" in
	db149 | ap148 | ap145)
		for sec in $(print_sections $1); do
			flash_section ${sec}
		done
		switch_layout linux

		# TODO restore overlay and config
		return 0;
		;;
	esac

	echo "Upgrade failed!"
	return 1;
}
