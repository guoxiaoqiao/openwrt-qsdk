#!/bin/sh
#
# Copyright (c) 2013 Qualcomm Atheros, Inc.
#

do_ipq806x() {
	. /lib/ipq806x.sh

	ipq806x_board_detect
}

boot_hook_add preinit_main do_ipq806x
