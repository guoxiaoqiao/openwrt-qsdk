#
# Copyright (c) 2013 Qualcomm Atheros, Inc.
# Copyright (C) 2011 OpenWrt.org
#

USE_REFRESH=1

. /lib/ipq806x.sh
. /lib/upgrade/common.sh

RAMFS_COPY_DATA=/lib/ipq806x.sh
RAMFS_COPY_BIN="/usr/bin/dumpimage /bin/mktemp /usr/sbin/mkfs.ubifs
	/usr/sbin/ubiattach /usr/sbin/ubidetach /usr/sbin/ubiformat /usr/sbin/ubimkvol
	/usr/sbin/ubiupdatevol"

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

image_has_mandatory_section() {
	local img=$1
	local mandatory_sections=$2

	for sec in ${mandatory_sections}; do
		image_contains $img ${sec} || {\
			return 1
		}
	done
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

VERSION_FILE="/sys/devices/system/qfprom/qfprom0/version"
AUTHENTICATE_FILE="/sys/devices/system/qfprom/qfprom0/authenticate"
TMP_VERSION_FILE="/etc/config/sysupgrade_version"
PRIMARY_BOOT_FILE="/proc/boot_info/rootfs/primaryboot"
TMP_PRIMARY_BOOT_FILE="/etc/config/sysupgrade_primaryboot"
unsecure_version=0x0
local_version_string=0x0
authenticate_string=0

# is_authentication_check_enabled() checks whether installed image is
# secure(1) or not(0)
is_authentication_check_enabled() {
	if [ -e $AUTHENTICATE_FILE ]; then
		read authenticate_string < $AUTHENTICATE_FILE
	fi
	if [ "$authenticate_string" == "0" ]; then
		echo "Returning 0 from is_authentication_check_enabled"
		return 0
	else
		echo "Returning 1 from is_authentication_check_enabled"
		return 1
	fi
}

# get_local_image_version() check the version file & if it exists, read the
# hexadecimal value & save it into global variable local_version_string
get_local_image_version() {
	if [ -e $VERSION_FILE ]; then
		read local_version_string < $VERSION_FILE
		return 1
	fi
	echo "Returning 0 from get_local_image_version"
	return 0
}

# is_version_check_enabled() checks whether version check is
# enabled(non-zero value) or not
is_version_check_enabled() {
	get_local_image_version && return 0
	if [ "$local_version_string" == "$unsecure_version" ]; then
		echo "Returning 0 from is_version_check_enabled because "\
			"$local_version_string is ZERO"
		return 0
	else
		echo "Returning 1 from is_version_check_enabled because "\
			"$local_version_string is non-ZERO"
		return 1
	fi
}

sw_id=0
# get_sw_id_from_component_bin() parses the MBN header & checks total size v/s
# actual component size. If both differ, it means signature & certificates are
# appended at end.
# Extract the attestation certificate & read the Subject & retreive the SW_ID.
get_sw_id_from_component_bin() {
	sw_id=100
	component_size=`hexdump -n 4 -s 16 $1 | awk '{print $3 $2}'`
	code_size=`hexdump -n 4 -s 20 $1 | awk '{print $3 $2}'`
	echo $1
	echo "Total size=" $component_size
	echo "Actual code size=" $code_size
	if [ "$component_size" != "$code_size" ]; then
		echo "Image with version information"
		sw_id=0
		dd if=$1 of=temp_cert_file bs=1 count=2048 skip=`printf "%d\n" \
			$((0x$code_size + 0x128))`
		sw_id=`openssl x509 -in temp_cert_file -noout -text -inform der \
			| grep "SW_ID" | awk -v \
		FS="(OU=01 |SW_ID)" '{print $2}' | cut -c-8`
		sw_id=`printf "%d" 0x$sw_id`

		echo "SW ID=" $sw_id
	else
		echo "Image without version information"
		return 0
	fi
	return 1
}

max_failsafe_version=43
max_nonfailsafe_version=20
failsafe_version=0
nonfailsafe_version=0
failsafe_sw_id=0
nonfailsafe_sw_id=0
is_sw_version_present=0

# Local image version string stores the version information as below.
# Bit 0: Installed image is secure. Check Version
# Bit 1 to 20: Number of bits set specifies the version of non-failsafe
#              image. Max version stored can be 20
# Bit 21 to 63: Number of bits set specifies the version of failsafe
#              image. Max version stored can be 43
decode_local_image_version() {
	failsafe_version=0
	nonfailsafe_version=0
	get_local_image_version

	local_version_string=`echo $local_version_string | cut -c 3-`
	len=${#local_version_string}
	LSBcount=0
	for i in $(seq $(expr $len - 1) -1 0); do
		xDigit="${local_version_string:$i:1}"
		LSBcount=`expr $LSBcount + 1`
		case "$LSBcount" in
		1)
			case "$xDigit" in
				3)	nonfailsafe_version=`expr $nonfailsafe_version + 1`;;
				7)	nonfailsafe_version=`expr $nonfailsafe_version + 2`;;
				f|F)	nonfailsafe_version=`expr $nonfailsafe_version + 3`;;
			esac;;
		2|3|4|5)
			case "$xDigit" in
				1)	nonfailsafe_version=`expr $nonfailsafe_version + 1`;;
				3)	nonfailsafe_version=`expr $nonfailsafe_version + 2`;;
				7)	nonfailsafe_version=`expr $nonfailsafe_version + 3`;;
				f|F)	nonfailsafe_version=`expr $nonfailsafe_version + 4`;;
			esac;;
		6)
			case "$xDigit" in
				1)	nonfailsafe_version=`expr $nonfailsafe_version + 1`;;
				3)	nonfailsafe_version=`expr $nonfailsafe_version + 1`
					failsafe_version=`expr $failsafe_version + 1`;;
				7)	nonfailsafe_version=`expr $nonfailsafe_version + 1`
					failsafe_version=`expr $failsafe_version + 2`;;
				f|F)	nonfailsafe_version=`expr $nonfailsafe_version + 1`
					failsafe_version=`expr $failsafe_version + 3`;;
				2)	failsafe_version=`expr $failsafe_version + 1`;;
				6)	failsafe_version=`expr $failsafe_version + 2`;;
				e|E)	failsafe_version=`expr $failsafe_version + 3`;;
			esac;;
		7|8|9|10|11|12|13|14|15|16)
			case "$xDigit" in
				1)	failsafe_version=`expr $failsafe_version + 1`;;
				3)	failsafe_version=`expr $failsafe_version + 2`;;
				7)	failsafe_version=`expr $failsafe_version + 3`;;
				f|F)	failsafe_version=`expr $failsafe_version + 4`;;
			esac;;
		*)
			echo "Error: Too big number"
			;;
		esac
	done
	echo "nonfailsafe=$nonfailsafe_version, failsafe=$failsafe_version"
	return 1
}

temp_kernel_path=/tmp/tmp_kernel.bin
# In case of NAND image, Kernel image is ubinized & version information is
# part of Kernel image. Hence need to un-ubinize the image.
# To get the kernel image, Find the volume with volume id 0. Kernel image
# is fragmented and hence to assemble it to get complete image.
# In UBI image, first look for UBI#, which is magic number used to identify
# each eraseble block. Parse the UBI header, which starts with UBI# & get
# the VID(volume ID) header offset as well as Data offset.
# Traverse to VID heade offset & check the volume ID. If it is ZERO, Kernel
# image is stored in this volume. Use Data offset to extract the Kernel image.
# Since Kernel image has MBN header, use Total component size from MBN header
# to copy the exact kernel image to a temporary location.
extract_kernel_binary() {
	leb_sizeD=0
	hexdump -C -n 1000000 $1 | grep "UBI#" | awk '{print $1 }' | while read -r line_no
	do
		line_noD=`printf "%d" 0x$line_no`
		n_line_no=$(expr $line_noD + 16)

		if [[ "$line_noD" -ne "0" ]] && [[ "$leb_sizeD" -eq "0" ]]; then
			leb_sizeD=$line_noD
		fi

		echo LEB_SIZE=$leb_sizeD
		vid_hdr_offset=`hexdump -C -s $n_line_no -n 8 $1 | awk '{print $2$3$4$5}'`
		vid_hdr_offsetD=`printf "%d" 0x$vid_hdr_offset`
		vid_hdr_locationD=$(expr $line_noD + $vid_hdr_offsetD)

		data_offset=`hexdump -C -s $n_line_no -n 8 $1 | awk '{print $6$7$8$9}'`

		volume_id=`hexdump -C -s $vid_hdr_locationD -n 16 $1 | awk '{print $10$11$12$13}'`
		volume_idD=`printf "%d" 0x$volume_id`

		lnum=`hexdump -C -s $vid_hdr_locationD -n 16 $1 | awk '{print $14$15$16$17}'`
		lnumD=`printf "%d" 0x$lnum`

		if [ $(expr $volume_idD + 0) -eq 0 ] && [ $(expr $lnumD + 0) -eq 0 ]; then
			data_offsetD=`printf "%d" 0x$data_offset`
			data_locationD=$(expr $line_noD + $data_offsetD)
			#hexdump -C -s $data_locationD -n 40 $1
			component_addressD=$(expr $data_locationD + 16)
			component_size=`hexdump -n 4 -s $component_addressD $1 | \
				awk '{print $3 $2}'`
			component_sizeD=`printf "%d" 0x$component_size`
			component_sizeD=$(expr $component_sizeD + 40)
			echo component_addressD=$component_addressD, component_size=$component_size
			maxCopyLenD=$(expr $leb_sizeD - $data_offsetD)
			dataCopiedD=0
			echo maxCopyLenD=$maxCopyLenD, dataCopiedD=$dataCopiedD
			while [ $(expr $component_sizeD + 0) -gt 0 ]
			do
				echo component_sizeD=$component_sizeD, maxCopyLenD=$maxCopyLenD, \
					dataCopiedD=$dataCopiedD, data_locationD=$data_locationD
				if [ $(expr $component_sizeD + 0) -lt $(expr $maxCopyLenD + 0) ]; then
					maxCopyLenD=$component_sizeD
				fi
				dd if=$1 of=$temp_kernel_path bs=1 count=$maxCopyLenD \
					skip=$data_locationD seek=$dataCopiedD
				dataCopiedD=$(expr $dataCopiedD + $maxCopyLenD)
				component_sizeD=$(expr $component_sizeD - $maxCopyLenD)
				data_locationD=$(expr $data_locationD + $leb_sizeD)
			done
			break
		fi
	done
	return 1
}

# is_image_version_higher() iterates through each component and check
# failsafe & non-failsafe versions against locally installed version.
# If newer component version is lower than locally insatlled image,
# abort the FW upgrade process.
# In case of NAND image, Kernel is ubinized, first extract the kernel
# image out of it & then check against failsafe version.
is_image_version_higher() {
	local img=$1
	decode_local_image_version

	for sec in $(print_sections ${img}); do
		local fullname=$(get_full_section_name ${img} ${sec})
		sw_id=0

	case "${fullname}" in
		hlos* | u-boot* )
			get_sw_id_from_component_bin /tmp/${fullname}.bin && { \
				echo "Error while extracting version information from \
					/tmp/${fullname}.bin"
				return 0
			}
			if [ "$failsafe_version" -gt "$sw_id" ]; then
				echo "Version of Fail safe image ${fullname}($sw_id) is lower than \
					minimal supported version($failsafe_version)"
				return 0
			fi
			if [ "$sw_id" -gt "$max_failsafe_version" ]; then
				echo "Version of Fail safe image ${fullname}($sw_id) is higher \
					than maximum allowed version($max_failsafe_version)"
				return 0
			fi
			failsafe_sw_id=$sw_id
			is_sw_version_present=`expr $is_sw_version_present + 1`
			echo is_sw_version_present=$is_sw_version_present
			;;
		sbl2* | sbl3* | tz* | rpm* )
			get_sw_id_from_component_bin /tmp/${fullname}.bin && { \
				echo "Error while extracting version information from \
					/tmp/${fullname}.bin"
				return 0
			}
			if [ "$nonfailsafe_version" -gt "$sw_id" ]; then
				echo "Version of NON-Fail safe image ${fullname}($sw_id) is lower \
					than minimal supported version($nonfailsafe_version)"
				return 0
			fi
			nonfailsafe_sw_id=$sw_id
			is_sw_version_present=`expr $is_sw_version_present + 1`
			echo is_sw_version_present=$is_sw_version_present
			;;
		ubi* )
			#Extract Kernel image from UBI first
			extract_kernel_binary  /tmp/${fullname}.bin && { \
				echo "Unable to extract Kernel image from /tmp/${fullname}.bin"
				return 0
			}
			get_sw_id_from_component_bin $temp_kernel_path && { \
				echo "Error while extracting version information from \
			       		/tmp/${fullname}.bin"
				return 0
			}
			if [ "$failsafe_version" -gt "$sw_id" ]; then
				echo "Version of Fail safe image ${fullname}($sw_id) is lower \
					than minimal supported version($failsafe_version)"
				return 0
			fi
			if [ "$sw_id" -gt "$max_failsafe_version" ]; then
				echo "Version of Fail safe image ${fullname}($sw_id) is higher \
					than maximum allowed version($max_failsafe_version)"
				return 0
			fi
			failsafe_sw_id=$sw_id
			is_sw_version_present=`expr $is_sw_version_present + 1`
			echo is_sw_version_present=$is_sw_version_present
			;;
		*) #echo "Component ${fullname} ignored"
			;;
	esac
	done
	echo "Returning SUCCESS(1) from is_image_version_higher"
	return 1
}

check_image_version() {
	is_version_check_enabled && {\
		echo "Version check is not enabled, upgrade to continue !!!"
		return 1
	}
	echo "Local image is SECURE image, check individual image version"
	is_image_version_higher $1 && {\
		echo "New image versions are lower than existing image, upgrade to STOP !!!"
		return 0
	}
	update_tmp_version
	echo "New image versions are upgradeable, upgrade to proceed !!!"
	return 1
}

# Update the version information file based on currently SW_ID being installed.
# Write version information temporarily to /etc/config/version file before going
# for reboot. After reboot, once Kernel loads, update the actual version
# information file inside preini RC script.
update_tmp_version() {
	echo nonfailsafe_version=$nonfailsafe_sw_id, failsafe_version=$failsafe_sw_id
	if [ `expr $is_sw_version_present + 0` -eq 0 ]; then
		echo "Image doesn't have valid SW Versions. Do not update the version file"
		return 0
	fi
	new_versionD=0
	i=1
	while [ $i -le $max_failsafe_version ]
	do
		if [ $(expr $max_failsafe_version - $i) -lt $(expr $failsafe_sw_id + 0) ]; then
			new_versionD=$(expr $new_versionD + 1)
		fi
		new_versionD=$(expr $new_versionD \* 2)
		i=`expr $i + 1`
	done
	i=1
	while [ $i -le $max_nonfailsafe_version ]
	do
		if [ $(expr $max_nonfailsafe_version - $i) -lt $(expr $nonfailsafe_sw_id + 0) ]; then
			new_versionD=$(expr $new_versionD + 1)
		fi
		new_versionD=$(expr $new_versionD \* 2)
		i=`expr $i + 1`
	done
	new_versionD=`expr $new_versionD + 1`
	echo new_version=`printf "0x%x" $new_versionD`
	echo `printf "0x%x" $new_versionD` > "$TMP_VERSION_FILE"
	cat "$PRIMARY_BOOT_FILE" > "$TMP_PRIMARY_BOOT_FILE"
	return 1
}

# split_code_signature_cert_from_component_bin splits the component
# binary by splitting into code(including MBN header), signature file &
# attenstation certificate. Save them into $2 $3 & $4.
split_code_signature_cert_from_component_bin() {
	component_size=`hexdump -n 4 -s 16 $1 | awk '{print $3 $2}'`
	code_size=`hexdump -n 4 -s 20 $1 | awk '{print $3 $2}'`
	echo "Total size=" $component_size
	echo "Actual code size=" $code_size
	if [ "$component_size" != "$code_size" ]; then
		echo "Image with version information"
		dd if=$1 of=$2 bs=1 count=`printf "%d\n" $((0x$code_size + 0x28))`
		dd if=$1 of=$3 bs=1 count=256 skip=`printf "%d\n" \
			$((0x$code_size + 0x28))`
		dd if=$1 of=$4 bs=1 count=2048 skip=`printf "%d\n" \
			$((0x$code_size + 0x128))`
		return 1
	else
		echo "Error: Image without version information"
		return 0
	fi
}

#being used to calculate the image hash
generate_swid_ipad() {
	swid_xor_ipad=${swid_xor_ipad}`printf "%x" $(($1 ^ $2))`
}

#being used to calculate the image hash
generate_hwid_opad() {
	hwid_xor_opad=${hwid_xor_opad}`printf "%x" $(($1 ^ $2))`
}

SW_MASK=3636363636363636
HW_MASK=5c5c5c5c

# is_component_authenticated() usage the code, signature & public key retrieved
# for each component.
is_component_authenticated() {
	openssl x509 -in $3 -pubkey -inform DER  -noout > /tmp/pub_key
	sw_id=`openssl x509 -in $3 -noout -text -inform der | grep "SW_ID" | awk -v \
		FS="(OU=01 |SW_ID)" '{print $2}' | cut -c-16`
	hw_id=`openssl x509 -in $3 -noout -text -inform der | grep "SW_ID" | awk -v \
		FS="(OU=02 |HW_ID)" '{print $2}' | cut -c-8`
	oem_id=`openssl x509 -in $3 -noout -text -inform der | grep "SW_ID" | awk -v \
		FS="(OU=04 |OEM_ID)" '{print $2}' | cut -c-4`
	oem_model_id=`openssl x509 -in $3 -noout -text -inform der | grep "SW_ID" | awk -v \
		FS="(OU=06 |MODEL_ID)" '{print $2}' | cut -c-4`
	echo "SW=$sw_id, HW=$hw_id, OEM=$oem_id, model_id=$oem_model_id"

	swid_xor_ipad=""
	len=${#sw_id}
	for i in $(seq 0 $(expr $len - 1)); do
		generate_swid_ipad `printf "%d" 0x${sw_id:$i:1}` `printf "%d" 0x${SW_MASK:$i:1}`
	done
	rm -f /tmp/swid_xor_ipad
	len=${#swid_xor_ipad}
	for i in $(seq 0 2 $(expr $len - 1)); do
		echo -n -e \\x${swid_xor_ipad:$i:2} >> /tmp/swid_xor_ipad
	done

	hwid_xor_opad=""
	len=${#hw_id}
	for i in $(seq 0 $(expr $len - 1)); do
		generate_hwid_opad `printf "%d" 0x${hw_id:$i:1}` `printf "%d" 0x${HW_MASK:$i:1}`
	done

	len=${#oem_id}
	for i in $(seq 0 $(expr $len - 1)); do
		generate_hwid_opad `printf "%d" 0x${oem_id:$i:1}` `printf "%d" 0x${HW_MASK:$i:1}`
	done

	len=${#oem_model_id}
	for i in $(seq 0 $(expr $len - 1)); do
		generate_hwid_opad `printf "%d" 0x${oem_model_id:$i:1}` `printf "%d" 0x${HW_MASK:$i:1}`
	done
	rm -f /tmp/hwid_xor_opad
	len=${#hwid_xor_opad}
	for i in $(seq 0 2 $(expr $len - 1)); do
		echo -n -e \\x${hwid_xor_opad:$i:2} >> /tmp/hwid_xor_opad
	done

	openssl dgst -sha256 -binary -out code_hash $1

	cat /tmp/swid_xor_ipad code_hash | openssl dgst -sha256 -binary -out tmp_hash
	cat /tmp/hwid_xor_opad tmp_hash | openssl dgst -sha256 -binary -out computed_hash
	openssl rsautl -in $2 -pubin -inkey /tmp/pub_key -verify > reference_hash

	cmp computed_hash reference_hash
	return $?
}

# is_image_authenticated() iterates through each component and check
# whether individual component is authenticated. If not, abort the FW
# upgrade process. In case of NAND image, Kernel is ubinized, first
# extract the kernel image out of it & then verify
is_image_authenticated() {
	local img=$1

	for sec in $(print_sections ${img}); do
		local fullname=$(get_full_section_name ${img} ${sec})
		sw_id=0

	case "${fullname}" in
		hlos* | u-boot* | sbl2* | sbl3* | tz* | rpm* )
			split_code_signature_cert_from_component_bin /tmp/${fullname}.bin \
					src sig cert && { \
				echo "Error while splitting code/signature/Certificate from \
					/tmp/${fullname}.bin"
				return 0
			}
			is_component_authenticated src sig cert || { \
				echo "Error while authenticating /tmp/${fullname}.bin"
				return 0
			}
			;;
		ubi* )
			#Extract Kernel image from UBI first
			extract_kernel_binary  /tmp/${fullname}.bin \
					src sig cert && { \
				echo "Unable to extract Kernel image from /tmp/${fullname}.bin"
				return 0
			}
			split_code_signature_cert_from_component_bin $temp_kernel_path \
					src sig cert && { \
				echo "Error while splitting code/signature/Certificate from \
					/tmp/tmp_kernel.bin"
				return 0
			}
			is_component_authenticated src sig cert || { \
				echo "Error while authenticating $temp_kernel_path"
				return 0
			}
			;;
		*) #echo "Component ${fullname} ignored"
			;;
	esac
	done
	echo "Returning SUCCESS(1) from is_image_authenticated"
	return 1
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
	local bin=$1
	local mtdname=$2

	local mtdpart=$(grep "\"${mtdname}\"" /proc/mtd | awk -F: '{print $1}')
	local pgsz=$(cat /sys/class/mtd/${mtdpart}/writesize)

	dd if=/tmp/${bin}.bin bs=${pgsz} conv=sync | mtd write - -e ${mtdname} ${mtdname}
}

do_flash_emmc() {
	local bin=$1
	local emmcblock=$2

	dd if=/dev/zero of=${emmcblock}
	dd if=/tmp/${bin}.bin of=${emmcblock}
}

do_flash_partition() {
	local bin=$1
	local mtdname=$2
	local emmcblock="$(find_mmc_part "0:$mtdname")"

	if [ -e "$emmcblock" ]; then
		do_flash_emmc $bin $emmcblock
	else
		do_flash_mtd $bin $mtdname
	fi
}

do_flash_bootconfig() {
	local bin=$1
	local mtdname=$2

	# Fail safe upgrade
	if [ -f /proc/boot_info/getbinary_${bin} ]; then
		cat /proc/boot_info/getbinary_${bin} > /tmp/${bin}.bin
		do_flash_partition $bin $mtdname
	fi
}

do_flash_failsafe_partition() {
	local bin=$1
	local mtdname=$2
	local emmcblock

	# Fail safe upgrade
	[ -f /proc/boot_info/$mtdname/upgradepartition ] && {
		mtdname=$(cat /proc/boot_info/$mtdname/upgradepartition)
	}

	emmcblock="$(find_mmc_part "$mtdname")"

	if [ -e "$emmcblock" ]; then
		do_flash_emmc $bin $emmcblock
	else
		do_flash_mtd $bin $mtdname
	fi

}

do_flash_ubi() {
	local bin=$1
	local mtdname=$2
	local mtdpart

	mtdpart=$(grep "\"${mtdname}\"" /proc/mtd | awk -F: '{print $1}')
	ubidetach -f -p /dev/${mtdpart}

	# Fail safe upgrade
	[ -f /proc/boot_info/$mtdname/upgradepartition ] && {
		mtdname=$(cat /proc/boot_info/$mtdname/upgradepartition)
	}

	mtdpart=$(grep "\"${mtdname}\"" /proc/mtd | awk -F: '{print $1}')

	ubiformat /dev/${mtdpart} -y -f /tmp/${bin}.bin
}

do_flash_hlos() {
	local sec=$1
	local mtdpart=$(find_mmc_part "0:HLOS")

	if [ -n "$mtdpart" ]; then
		do_flash_failsafe_partition ${sec} "0:HLOS"
	else
		do_flash_failsafe_partition ${sec} "HLOS"
	fi
}

flash_section() {
	local sec=$1

	local board=$(ipq806x_board_name)
	case "${sec}" in
		hlos*) switch_layout linux; do_flash_hlos ${sec};;
		fs*) switch_layout linux; do_flash_failsafe_partition ${sec} "rootfs";;
		ubi*) switch_layout linux; do_flash_ubi ${sec} "rootfs";;
		sbl1*) switch_layout boot; do_flash_partition ${sec} "SBL1";;
		sbl2*) switch_layout boot; do_flash_failsafe_partition ${sec} "SBL2";;
		sbl3*) switch_layout boot; do_flash_failsafe_partition ${sec} "SBL3";;
		u-boot*) switch_layout boot; do_flash_failsafe_partition ${sec} "APPSBL";;
		ddr-${board}-*) switch_layout boot; do_flash_failsafe_partition ${sec} "DDRCONFIG";;
		ssd*) switch_layout boot; do_flash_partition ${sec} "SSD";;
		tz*) switch_layout boot; do_flash_failsafe_partition ${sec} "TZ";;
		rpm*) switch_layout boot; do_flash_failsafe_partition ${sec} "RPM";;
		*) echo "Section ${sec} ignored"; return 1;;
	esac

	echo "Flashed ${sec}"
}

platform_check_image() {
	local board=$(ipq806x_board_name)

	local mandatory_nand="ubi"
	local mandatory_nor_emmc="hlos fs"
	local mandatory_section_found=0
	local optional="sbl1 sbl2 sbl3 u-boot ddr-${board} ssd tz rpm"
	local ignored="mibib bootconfig"

	image_is_FIT $1 || return 1

	image_has_mandatory_section $1 ${mandatory_nand} && {\
		mandatory_section_found=1
	}

	image_has_mandatory_section $1 ${mandatory_nor_emmc} && {\
		mandatory_section_found=1
	}

	if [ $mandatory_section_found -eq 0 ]; then
		echo "Error: mandatory section(s) missing from \"$1\". Abort..."
		return 1
	fi

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

	is_authentication_check_enabled || {
		is_image_authenticated $1 && {\
			echo "Error: \"$1\" couldn't be authenticated. Abort..."
			return 1
		}
		check_image_version $1 && {\
			echo "Error: \"$1\" couldn't be upgraded. Abort..."
			return 1
		}
	}
	return 0
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
	db149 | ap148 | ap145 | ap148_1xx | db149_1xx | db149_2xx | ap145_1xx | ap160 | ap160_2xx | ap161 | ak01_1xx)
		for sec in $(print_sections $1); do
			flash_section ${sec}
		done

		switch_layout linux
		# update bootconfig to register that fw upgrade has been done
		do_flash_bootconfig bootconfig "BOOTCONFIG"
		do_flash_bootconfig bootconfig1 "BOOTCONFIG1"

		return 0;
		;;
	esac

	echo "Upgrade failed!"
	return 1;
}

platform_copy_config() {
	local nand_part="$(find_mtd_part "ubi_rootfs")"
	local emmcblock="$(find_mmc_part "rootfs_data")"

	if [ -e "$nand_part" ]; then
		local mtdname=rootfs
		local mtdpart

		[ -f /proc/boot_info/$mtdname/upgradepartition ] && {
			mtdname=$(cat /proc/boot_info/$mtdname/upgradepartition)
		}

		mtdpart=$(grep "\"${mtdname}\"" /proc/mtd | awk -F: '{print $1}')
		ubiattach -p /dev/${mtdpart}
		mount -t ubifs ubi0:ubi_rootfs_data /tmp/overlay || {
			mount -t ubifs ubi0:rootfs_data /tmp/overlay
		}
		tar zxvf /tmp/sysupgrade.tgz -C /tmp/overlay/
	elif [ -e "$emmcblock" ]; then
		mount -t ext4 "$emmcblock" /tmp/overlay
		tar zxvf /tmp/sysupgrade.tgz -C /tmp/overlay/
	else
		jffs2_copy_config
	fi
}
