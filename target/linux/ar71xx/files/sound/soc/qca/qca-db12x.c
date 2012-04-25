/*
 * ath-db12x.c -- ALSA machine code for DB12x board ref design (and relatives)
 *
 * Author:	Mathieu Olivari
 * Created:	Jan 25, 2011
 * Copyright:	(C) 2011 Qualcomm Atheros, Inc.
 *
 */

#include <asm/delay.h>
#include <linux/types.h>
#include <sound/core.h>
#include <sound/soc.h>
#include <linux/module.h>

/* Kernel includes */
#if 0
#include "atheros.h"
#include "934x.h"
#endif

/* Driver include */
#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>
#include "wasp-i2s.h"
#include "wasp-pcm.h"

/* GPIO Assignation */
#define DB120_GPIO_I2S_MCLK	4
#define DB120_GPIO_I2S_SD	11
#define DB120_GPIO_I2S_WS	12
#define DB120_GPIO_I2S_CLK	13
#define DB120_GPIO_SPDIF_OUT	15

static struct platform_device *db12x_snd_device;

static struct snd_soc_dai_link db12x_dai = {
	.name = "I2S SPKR Out",
	.stream_name = "I2S SPKR Out",
	.cpu_dai_name = "wasp-i2s",
	.codec_dai_name = "dit-hifi",
	.platform_name = "wasp-pcm-audio",
	.codec_name = "spdif-dit",
	/* use ops to check startup state */
};

static struct snd_soc_card snd_soc_db12x = {
	.name = "Qualcomm-Atheros DB12x ref. design",
	.dai_link = &db12x_dai,
	.num_links = 1,
};

static int __init db12x_init(void)
{
	int ret;

	printk(KERN_CRIT "%s called\n", __FUNCTION__);

	db12x_snd_device = platform_device_alloc("soc-audio", -1);
	if(!db12x_snd_device)
		return -ENOMEM;

	platform_set_drvdata(db12x_snd_device, &snd_soc_db12x);
	ret = platform_device_add(db12x_snd_device);

	if (ret) {
		platform_device_put(db12x_snd_device);
	}

	return ret;
}

static void __exit db12x_exit(void)
{
	printk(KERN_CRIT "%s called\n", __FUNCTION__);

	platform_device_unregister(db12x_snd_device);
}

module_init(db12x_init);
module_exit(db12x_exit);

MODULE_AUTHOR("Qualcomm-Atheros");
MODULE_DESCRIPTION("QCA Audio Machine module");
MODULE_LICENSE("Dual BSD/GPL");
