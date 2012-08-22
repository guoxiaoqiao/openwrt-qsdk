#!/bin/sh
#
# Copyright (C) 2012 Qualcomm-Atheros
#

button_add_uci_config() {
	local action=$1
	local button=$2
	local handler=$3
	uci batch <<EOF
add system button
set system.@button[-1].action=$1
set system.@button[-1].button=$2
set system.@button[-1].handler="$3"
commit
EOF
	uci commit system
}

