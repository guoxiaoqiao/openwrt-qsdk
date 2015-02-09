#!/bin/sh
#
# Copyright (c) 2013 The Linux Foundation. All rights reserved.
# Copyright (C) 2011 OpenWrt.org
#

IPQ806X_BOARD_NAME=
IPQ806X_MODEL=

ipq806x_board_detect() {
	local machine
	local name

	machine=$(awk 'BEGIN{FS="[ \t]+:[ \t]"} /Hardware/ {print $2}' /proc/cpuinfo)

	case "$machine" in
	*"DB149 reference board")
		name="db149"
		;;
	*"AP148 reference board")
		name="ap148"
		;;
	*"AP145 reference board")
		name="ap145"
		;;
	*"AP145-1XX reference board")
		name="ap145_1xx"
		;;
	*"DB149-1XX reference board")
		name="db149_1xx"
		;;
	*"DB149-2XX reference board")
		name="db149_2xx"
		;;
	*"AP148-1XX reference board")
		name="ap148_1xx"
		;;
	*"AP160 reference board")
		name="ap160"
		;;
	*"AP160-2XX reference board")
		name="ap160_2xx"
		;;
	*"AP161 reference board")
		name="ap161"
		;;
	esac

	[ -z "$name" ] && name="unknown"

	[ -z "$IPQ806X_BOARD_NAME" ] && IPQ806X_BOARD_NAME="$name"
	[ -z "$IPQ806X_MODEL" ] && IPQ806X_MODEL="$machine"

	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"

	echo "$IPQ806X_BOARD_NAME" > /tmp/sysinfo/board_name
	echo "$IPQ806X_MODEL" > /tmp/sysinfo/model
}

ipq806x_board_name() {
	local name

	[ -f /tmp/sysinfo/board_name ] && name=$(cat /tmp/sysinfo/board_name)
	[ -z "$name" ] && name="unknown"

	echo "$name"
}
