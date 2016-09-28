/*
 * Atheros AP147 reference board support
 *
 * Copyright (c) 2013 The Linux Foundation. All rights reserved.
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
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-spi.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"
#include "pci.h"

#define AP147_GPIO_LED_WLAN		12
#define AP147_GPIO_LED_WLAN_V2		4
#define AP147_GPIO_LED_WPS		13
#define AP147_GPIO_LED_SYS_V2		1
#define AP147_GPIO_LED_STATUS		13

#define AP147_GPIO_LED_WAN		4
#define AP147_GPIO_LED_WAN_V2		2
#define AP147_GPIO_LED_LAN1		16
#define AP147_GPIO_LED_LAN1_V2		15
#define AP147_GPIO_LED_LAN2		15
#define AP147_GPIO_LED_LAN2_V2		16
#define AP147_GPIO_LED_LAN3		14
#define AP147_GPIO_LED_LAN3_V2		0
#define AP147_GPIO_LED_LAN4		11
#define AP147_GPIO_LED_LAN4_V2		3

#define AP147_GPIO_BTN_WPS		17
#define AP147_GPIO_BTN_WPS_V2		14

#define AP147_KEYS_POLL_INTERVAL	20	/* msecs */
#define AP147_KEYS_DEBOUNCE_INTERVAL	(3 * AP147_KEYS_POLL_INTERVAL)

#define AP147_MAC0_OFFSET		0
#define AP147_MAC1_OFFSET		6
#define AP147_WMAC_CALDATA_OFFSET	0x1000

#define AP147_MAX_LED_WPS_GPIOS         6
#define AP147_MAX_BOARD_VERSION         2

#define AP147_V2_ID			17
#define AP147_WMAC1_CALDATA_OFFSET	0x5000
#define BOARDID_OFFSET			0x20

#define BOARD_V1		0
#define BOARD_V2		1

enum GPIO {
	WAN,
	LAN1,
	LAN2,
	LAN3,
	LAN4
};

unsigned char ap147_gpios[AP147_MAX_BOARD_VERSION][AP147_MAX_LED_WPS_GPIOS] __initdata = {
	{AP147_GPIO_LED_WAN, AP147_GPIO_LED_LAN1, AP147_GPIO_LED_LAN2,
	AP147_GPIO_LED_LAN3, AP147_GPIO_LED_LAN4, AP147_GPIO_BTN_WPS},
	{AP147_GPIO_LED_WAN_V2, AP147_GPIO_LED_LAN1_V2, AP147_GPIO_LED_LAN2_V2,
	AP147_GPIO_LED_LAN3_V2, AP147_GPIO_LED_LAN4_V2, AP147_GPIO_BTN_WPS_V2}
};

static struct gpio_led ap147_leds_gpio[] __initdata = {
	{
		.name		= "ap147:green:status",
		.gpio		= AP147_GPIO_LED_WPS,
		.active_low	= 1,
	},
	{
		.name		= "ap147:green:wlan",
		.gpio		= AP147_GPIO_LED_WLAN,
		.active_low	= 1,
	}
};

static struct gpio_keys_button ap147_gpio_keys[] __initdata = {
	{
		.desc		= "WPS button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = AP147_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= AP147_GPIO_BTN_WPS,
		.active_low	= 1,
	},
};

static void __init ap147_gpio_led_setup(int board_version)
{
	ath79_gpio_direction_select(ap147_gpios[board_version][WAN], true);
	ath79_gpio_direction_select(ap147_gpios[board_version][LAN1], true);
	ath79_gpio_direction_select(ap147_gpios[board_version][LAN2], true);
	ath79_gpio_direction_select(ap147_gpios[board_version][LAN3], true);
	ath79_gpio_direction_select(ap147_gpios[board_version][LAN4], true);

	ath79_gpio_output_select(ap147_gpios[board_version][WAN],
			QCA953X_GPIO_OUT_MUX_LED_LINK5);
	ath79_gpio_output_select(ap147_gpios[board_version][LAN1],
			QCA953X_GPIO_OUT_MUX_LED_LINK1);
	ath79_gpio_output_select(ap147_gpios[board_version][LAN2],
			QCA953X_GPIO_OUT_MUX_LED_LINK2);
	ath79_gpio_output_select(ap147_gpios[board_version][LAN3],
			QCA953X_GPIO_OUT_MUX_LED_LINK3);
	ath79_gpio_output_select(ap147_gpios[board_version][LAN4],
			QCA953X_GPIO_OUT_MUX_LED_LINK4);

	if (board_version == BOARD_V2) {
		ap147_leds_gpio[0].active_low = 0;
		ap147_leds_gpio[0].gpio = AP147_GPIO_LED_SYS_V2;
		ap147_leds_gpio[1].gpio = AP147_GPIO_LED_WLAN_V2;
		ap147_gpio_keys[0].gpio = AP147_GPIO_BTN_WPS_V2;
	}
	ath79_register_leds_gpio(-1, ARRAY_SIZE(ap147_leds_gpio),
			ap147_leds_gpio);
	ath79_register_gpio_keys_polled(-1, AP147_KEYS_POLL_INTERVAL,
			ARRAY_SIZE(ap147_gpio_keys),
			ap147_gpio_keys);
}

static void __init ap147_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	u8 board_id = *(u8 *) (art + AP147_WMAC1_CALDATA_OFFSET + BOARDID_OFFSET);
	pr_info("AP147 Reference Board Id is %d\n",(u8)board_id);
	ath79_register_m25p80(NULL);

	if (board_id == AP147_V2_ID) {
		/* Disabling the JTAG due to conflicting GPIO's.
		 * Can be re-enabled dynamically by writing appropriate
		 * value to GPIO_FUNCTION_ADDRESS register
		 */
		ath79_gpio_function_enable(AR934X_GPIO_FUNC_JTAG_DISABLE);
		ap147_gpio_led_setup(BOARD_V2);
	} else {
		ap147_gpio_led_setup(BOARD_V1);
	}

	ath79_register_usb();
	ath79_register_pci();

	ath79_register_wmac(art + AP147_WMAC_CALDATA_OFFSET, NULL);

	ath79_register_mdio(0, 0x0);
	ath79_register_mdio(1, 0x0);

	ath79_init_mac(ath79_eth0_data.mac_addr, art + AP147_MAC0_OFFSET, 0);
	ath79_init_mac(ath79_eth1_data.mac_addr, art + AP147_MAC1_OFFSET, 0);

	/* WAN port */
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.speed = SPEED_100;
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_register_eth(0);

	/* LAN ports */
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.speed = SPEED_1000;
	ath79_eth1_data.duplex = DUPLEX_FULL;
	ath79_switch_data.phy_poll_mask |= BIT(4);
	ath79_switch_data.phy4_mii_en = 1;
	ath79_switch_data.phy_classab_en = 1;
	ath79_register_eth(1);
}

MIPS_MACHINE(ATH79_MACH_AP147, "AP147", "Qualcomm Atheros AP147 reference board",
	     ap147_setup);
