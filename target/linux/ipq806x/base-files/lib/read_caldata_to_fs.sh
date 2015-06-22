#!/bin/sh
#
# Copyright (c) 2015 The Linux Foundation. All rights reserved.
# Copyright (C) 2011 OpenWrt.org

. /lib/ipq806x.sh

read_caldata_to_filesystem()
{
    local board=$(ipq806x_board_name)
    local mtdblock=$(find_mtd_part 0:ART)

    local apdk041="/lib/firmware/IPQ4019/hw.1"

    [ -n "$mtdblock" ] || return

    # load board.bin
    case "$board" in
            ap-dk0*)
                    mkdir -p ${apdk041}
                    dd if=${mtdblock} of=${apdk041}/caldata_0.bin bs=32 count=377 skip=128
                    dd if=${mtdblock} of=${apdk041}/caldata_1.bin bs=32 count=377 skip=640
            ;;
    esac
}
