
/*
 * Atheros AP152 reference board support
 *
 * Copyright (c) 2014 The Linux Foundation. All rights reserved.
 * Copyright (c) 2012 Gabor Juhos <juhosg@openwrt.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>
#include <linux/ar8216_platform.h>

#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-m25p80.h"
#include "machtypes.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-spi.h"
#include "dev-usb.h"
#include "dev-wmac.h"

#define AP152_GPIO_LED_WLAN             12
#define AP152_GPIO_LED_WPS              13
#define AP152_GPIO_LED_STATUS           13

#define AP152_GPIO_LED_WAN              4
#define AP152_GPIO_LED_LAN1             16
#define AP152_GPIO_LED_LAN2             15
#define AP152_GPIO_LED_LAN3             14
#define AP152_GPIO_LED_LAN4             11

#define AP152_GPIO_BTN_WPS              17
#define AP152_KEYS_POLL_INTERVAL        20     /* msecs */
#define AP152_KEYS_DEBOUNCE_INTERVAL    (3 * AP152_KEYS_POLL_INTERVAL)

#define AP152_MAC0_OFFSET               0
#define AP152_MAC1_OFFSET               6
#define AP152_WMAC_CALDATA_OFFSET       0x1000

static void __init ap152_setup(void)
{
	ath79_register_m25p80(NULL);
}

MIPS_MACHINE(ATH79_MACH_AP152, "AP152", "Qualcomm Atheros AP152 reference board",
	     ap152_setup);
