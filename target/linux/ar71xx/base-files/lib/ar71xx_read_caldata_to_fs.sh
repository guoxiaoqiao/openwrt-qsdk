#!/bin/sh
#
# Copyright (c) 2015 The Linux Foundation. All rights reserved.
# Copyright (C) 2011 OpenWrt.org

. /lib/functions.sh
. /lib/ar71xx.sh

qcmbr_do_load_ar71xx_board_bin()
{
    local board=$(ar71xx_board_name)
    local mtdblock=$(find_mtd_part art)

    local cal_data_path="/tmp"

    [ -n "$mtdblock" ] || return

    # load board.bin
    case "$board" in
            ap147 | ap151 | ap135)
                    mkdir -p ${cal_data_path}
                    # As for Qcmdr ahbskip is given so the Radio chip connect to PCI will be wifi0 instead of wifi1
                    dd if=${mtdblock} of=${cal_data_path}/wifi0.caldata bs=32 count=377 skip=640
            ;;
    esac
}
