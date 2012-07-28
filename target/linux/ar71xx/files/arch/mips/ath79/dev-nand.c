/*
 *  Atheros AR71XX/AR724X/AR913X NAND controller device
 *
 *  Copyright (C) 2012 Qualcomm Atheros Inc.
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <linux/platform_device.h>
#include <asm/mach-ath79/ar71xx_regs.h>

static struct resource ath79_nand_resources[] = {
	{
		.start	= AR71XX_NAND_CTRL_BASE,
		.end	= AR71XX_NAND_CTRL_BASE + AR71XX_NAND_CTRL_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device ath79_nand_device = {
	.name		= "ath79-nand",
	.id		= -1,
	.resource	= ath79_nand_resources,
	.num_resources	= ARRAY_SIZE(ath79_nand_resources),
};

void __init ath79_register_nand(void)
{
	ath79_nand_device.dev.platform_data = NULL;
	platform_device_register(&ath79_nand_device);
}
