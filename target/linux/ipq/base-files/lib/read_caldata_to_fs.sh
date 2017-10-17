#!/bin/sh
#
# Copyright (c) 2015 The Linux Foundation. All rights reserved.
# Copyright (C) 2011 OpenWrt.org

. /lib/ipq806x.sh

do_load_ipq4019_board_bin()
{
    local board=$(ipq806x_board_name)
    local mtdblock=$(find_mtd_part 0:ART)

    local apdk="/tmp"

    HK_BD_FILENAME=/lib/firmware/IPQ8074/bdwlan.bin

    if [ -z "$mtdblock" ]; then
        # read from mmc
        mtdblock=$(find_mmc_part 0:ART)
    fi

    [ -n "$mtdblock" ] || return

    # load board.bin
    case "$board" in
            ap-dk0*)
                    mkdir -p ${apdk}
                    dd if=${mtdblock} of=${apdk}/wifi0.caldata bs=32 count=377 skip=128
                    dd if=${mtdblock} of=${apdk}/wifi1.caldata bs=32 count=377 skip=640
            ;;
            ap16* | ap148*)
                    mkdir -p ${apdk}
                    dd if=${mtdblock} of=${apdk}/wifi0.caldata bs=32 count=377 skip=128
                    dd if=${mtdblock} of=${apdk}/wifi1.caldata bs=32 count=377 skip=640
                    dd if=${mtdblock} of=${apdk}/wifi2.caldata bs=32 count=377 skip=1152
            ;;
             ap-hk01-*)
                    mkdir -p ${apdk}
                    FILESIZE=$(stat -c%s "$HK_BD_FILENAME")
                    dd if=${mtdblock} of=${apdk}/caldata.b10 bs=1 count=$FILESIZE skip=4096
                    [ -L /lib/firmware/IPQ8074/caldata.b10 ] || \
                    ln -s /tmp/caldata.b10 /lib/firmware/IPQ8074/caldata.b10
            ;;
             ap-hk02)
                    mkdir -p ${apdk}
                    FILESIZE=$(stat -c%s "$HK_BD_FILENAME")
                    dd if=${mtdblock} of=${apdk}/caldata.b20 bs=1 count=$FILESIZE skip=4096
                    [ -L /lib/firmware/IPQ8074/caldata.b20 ] || \
                    ln -s /tmp/caldata.b20 /lib/firmware/IPQ8074/caldata.b20
            ;;
             ap-hk05)
                    mkdir -p ${apdk}
                    FILESIZE=$(stat -c%s "$HK_BD_FILENAME")
                    dd if=${mtdblock} of=${apdk}/caldata.b50 bs=1 count=$FILESIZE skip=4096
                    [ -L /lib/firmware/IPQ8074/caldata.b50 ] || \
                    ln -s /tmp/caldata.b50 /lib/firmware/IPQ8074/caldata.b50
            ;;
             ap-hk06)
                    mkdir -p ${apdk}
                    FILESIZE=$(stat -c%s "$HK_BD_FILENAME")
                    dd if=${mtdblock} of=${apdk}/caldata.b60 bs=1 count=$FILESIZE skip=4096
                    [ -L /lib/firmware/IPQ8074/caldata.b60 ] || \
                    ln -s /tmp/caldata.b60 /lib/firmware/IPQ8074/caldata.b60
            ;;

    esac
}

