/*
 * ar80xx.c: ar80xx(ar8031/ar8033/ar8035) PHY driver
 *
 * Copyright (C) 2013 Qualcomm Atheros
 * Copyright (C) 2009 Felix Fietkau <nbd@openwrt.org>
 * Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/if.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/if_ether.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/netlink.h>
#include <linux/bitops.h>
#include <net/genetlink.h>
#include <linux/delay.h>
#include <linux/phy.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/lockdep.h>
#include "ar80xx.h"
#include <asm/mach-ath79/ar71xx_regs.h>

static int
ar8033_config_init(struct phy_device *pdev)
{
	//struct mii_bus *bus = pdev->bus;
	//bus->write(bus, 0x5, 0x1f, 0x101);
	//bus->write(bus, 0x5, 0x0, 0x8000);
	return 0;
}

static int
ar8033_read_status(struct phy_device *phydev)
{
	void __iomem *base;
	u32 t;
	genphy_read_status(phydev);

	base = ioremap(QCA955X_GMAC_BASE, QCA955X_GMAC_SIZE);
	t = __raw_readl(base + SGMII_MAC_RX_CONFIG_ADDRESS_OFFSET);
	t &= LINK_STATUS_BIT;
	if( t != 0)
		phydev->link = 1;
	else
		phydev->link = 0;
	iounmap(base);
	return 0;
}

static int
ar8033_config_aneg(struct phy_device *pdev)
{
	return 0;
}

static int
ar8033_probe(struct phy_device *pdev)
{
	return 0;
}

static void
ar8033_remove(struct phy_device *pdev)
{
}

static struct phy_driver ar80xx_phy_drivers[] = {
    {
	.phy_id		= AR80XX_PHY_ID_AR8033,
	.name		= "Qualcomm Atheros AR8033 PHY",
	.phy_id_mask	= AR80XX_PHY_ID_MASK,
	.features	= PHY_GBIT_FEATURES,
	.probe		= ar8033_probe,
	.remove		= ar8033_remove,
	.config_init	= &ar8033_config_init,
	.config_aneg	= &ar8033_config_aneg,
	.read_status	= &ar8033_read_status,
	.driver		= { .owner = THIS_MODULE },
    },
};

int __init
ar80xx_phy_init(void)
{
	int ret;
	int i;

	for (i = 0; i < ARRAY_SIZE(ar80xx_phy_drivers); i++) {
		ret = phy_driver_register(&ar80xx_phy_drivers[i]);
		if (ret) {
			while (i-- > 0)
				phy_driver_unregister(&ar80xx_phy_drivers[i]);
			return ret;
		}
	}
	return 0;
}

void __exit
ar80xx_phy_exit(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ar80xx_phy_drivers); i++)
		phy_driver_unregister(&ar80xx_phy_drivers[i]);
}

module_init(ar80xx_phy_init);
module_exit(ar80xx_phy_exit);
MODULE_LICENSE("GPL");

