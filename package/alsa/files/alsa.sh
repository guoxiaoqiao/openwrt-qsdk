#!/bin/sh
# Copyright (C) 2012 Qualcomm-Atheros Inc.

. /lib/functions.sh

append SND_DRIVERS "alsa"

detect_alsa() {
	devidx=0
	config_load sound

	[ -d /proc/asound ] || exit 0
	[ `cat /proc/asound/cards|grep '^ *[0-9]'|wc -l` -ne 0 ] || exit 0

	cd /proc/asound
	for card in $(ls -d card[0-9]* 2>&-); do
		devname="$(cat $card/id)"
		cat << EOF
config sound-device card$devidx
	option type	'alsa'
	option name	'$devname'

EOF
		for stream in $(ls -d $card/pcm*); do
			strname="$(cat $stream/info|grep '^id'|sed 's,.*: \(.*\),\1,')"
			strtype="$(cat $stream/info|grep '^stream'|sed 's,.*: \(.*\),\1,')"
			cat << EOF
config sound-stream
	option card	'card$devidx'
	option type	'$strtype'
	option name	'$strname'

EOF
		done
	done
}
