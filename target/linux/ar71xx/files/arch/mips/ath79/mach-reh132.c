/*
 * Atheros REH132 reference board support
 *
 * Copyright (c) 2013 Qualcomm Atheros
 * Copyright (c) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
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

#include <linux/pci.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>
#include <linux/ar8216_platform.h>
#include <linux/of_platform.h>

#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-spi.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define REH132_MAC0_OFFSET		0
#define REH132_MAC1_OFFSET		6
#define REH132_WMAC_CALDATA_OFFSET	0x1000

static struct of_device_id __initdata reh132_common_ids[] = {
	{ .compatible = "simple-bus", },
	{},
};

struct of_dev_auxdata reh132_auxdata_lookup[] __initdata = {
	OF_DEV_AUXDATA("qcom,reh132-wmac", AR934X_WMAC_BASE, "ar934x_wmac", &ath79_wmac_data),
	OF_DEV_AUXDATA("qcom,reh132-spi",  AR71XX_SPI_BASE,  "ath79-spi",   &ath79_spi_data),
	OF_DEV_AUXDATA("qcom,reh132-mdio", AR71XX_GE1_BASE,  "ag71xx-mdio", &ath79_mdio1_data),
	OF_DEV_AUXDATA("qcom,ag71xx-eth",  AR71XX_GE0_BASE,  "ag71xx.0",    &ath79_eth0_data),
	OF_DEV_AUXDATA("qcom,ag71xx-eth",  AR71XX_GE1_BASE,  "ag71xx.1",    &ath79_eth1_data),
	{}
};

static void __init reh132_init(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	ath79_init_m25p80_pdata(NULL);
	ath79_init_mdio_pdata(1, 0);
	ath79_init_wmac_pdata(art + REH132_WMAC_CALDATA_OFFSET, NULL);

	ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_SW_PHY_SWAP);

	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.speed = SPEED_100;
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.fifo_cfg2 = 0x03ff0155;
	ath79_init_mac(ath79_eth0_data.mac_addr, art + REH132_MAC0_OFFSET, 0);
	ath79_init_eth_pdata(0);
	ath79_eth0_data.mii_bus_dev = NULL;

	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.speed = SPEED_1000;
	ath79_eth1_data.duplex = DUPLEX_FULL;
	ath79_eth1_data.fifo_cfg2 = 0x03ff0155;
	ath79_init_mac(ath79_eth1_data.mac_addr, art + REH132_MAC1_OFFSET, 0);
	ath79_init_eth_pdata(1);
	ath79_eth1_data.mii_bus_dev = NULL;
}

static void __init reh132_setup(void)
{
	reh132_init();
	of_platform_populate(NULL, reh132_common_ids, reh132_auxdata_lookup, NULL);
}

MIPS_MACHINE(ATH79_MACH_REH132, "REH132", "Atheros REH132 reference board",
		 reh132_setup);
