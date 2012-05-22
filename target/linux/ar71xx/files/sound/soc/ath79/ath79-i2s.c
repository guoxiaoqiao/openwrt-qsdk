/*
 * ath79-i2s.c -- ALSA DAI (i2s) interface for the QCA Wasp based audio interface
 *
 * Copyright (c) 2012 Qualcomm-Atheros Inc.
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
 */

#include <sound/core.h>
#include <sound/soc.h>
#include <linux/module.h>

#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>

static int ath79_i2s_startup(struct snd_pcm_substream *substream,
			      struct snd_soc_dai *dai)
{
	printk(KERN_CRIT "%s called\n", __FUNCTION__);
	return 0;
}

static void ath79_i2s_shutdown(struct snd_pcm_substream *substream,
				struct snd_soc_dai *dai)
{
	printk(KERN_CRIT "%s called\n", __FUNCTION__);
	return;
}

static int ath79_i2s_trigger(struct snd_pcm_substream *substream, int cmd,
			      struct snd_soc_dai *dai)
{
	int ret = 0;

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		printk(KERN_CRIT "%s called - cmd=SNDRV_PCM_TRIGGER_START\n", __FUNCTION__);
		break;
	case SNDRV_PCM_TRIGGER_RESUME:
		printk(KERN_CRIT "%s called - cmd=SNDRV_PCM_TRIGGER_RESUME\n", __FUNCTION__);
		break;
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		printk(KERN_CRIT "%s called - cmd=SNDRV_PCM_TRIGGER_PAUSE_RELEASE\n", __FUNCTION__);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
		printk(KERN_CRIT "%s called - cmd=SNDRV_PCM_TRIGGER_STOP\n", __FUNCTION__);
		break;
	case SNDRV_PCM_TRIGGER_SUSPEND:
		printk(KERN_CRIT "%s called - cmd=SNDRV_PCM_TRIGGER_SUSPEND\n", __FUNCTION__);
		break;
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		printk(KERN_CRIT "%s called - cmd=SNDRV_PCM_TRIGGER_PAUSE_PUSH\n", __FUNCTION__);
		break;
	default:
		ret = -EINVAL;
	}
	return ret;
}

static int ath79_i2s_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params,
				struct snd_soc_dai *dai)
{
	printk(KERN_CRIT "%s called\n", __FUNCTION__);

	return 0;
}

static int ath79_i2s_set_dai_fmt(struct snd_soc_dai *cpu_dai,
		unsigned int fmt)
{
	printk(KERN_CRIT "%s called - fmt=%04x\n", __FUNCTION__, fmt);
	return 0;
}

static int ath79_i2s_set_dai_sysclk(struct snd_soc_dai *cpu_dai,
		int clk_id, unsigned int freq, int dir)
{
	printk(KERN_CRIT "%s called - freq=%d dir=%d\n", __FUNCTION__, freq, dir);
	return 0;
}

static struct snd_soc_dai_ops ath79_i2s_dai_ops = {
	.startup	= ath79_i2s_startup,
	.shutdown	= ath79_i2s_shutdown,
	.trigger	= ath79_i2s_trigger,
	.hw_params	= ath79_i2s_hw_params,
	.set_fmt	= ath79_i2s_set_dai_fmt,
	.set_sysclk	= ath79_i2s_set_dai_sysclk,
};

static struct snd_soc_dai_driver ath79_i2s_dai = {
	.name = "ath79-i2s",
	.id = 0,
	.playback = {
		.channels_min = 2,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_44100,
		.formats = SNDRV_PCM_FMTBIT_S16_LE |
			   SNDRV_PCM_FMTBIT_S16_BE,
		},
	.ops = &ath79_i2s_dai_ops,
};

static int ath79_i2s_drv_probe(struct platform_device *pdev)
{
	printk(KERN_CRIT "%s called\n", __FUNCTION__);
	return snd_soc_register_dai(&pdev->dev, &ath79_i2s_dai);
}

static int __devexit ath79_i2s_drv_remove(struct platform_device *pdev)
{
	printk(KERN_CRIT "%s called\n", __FUNCTION__);
	snd_soc_unregister_dai(&pdev->dev);
	return 0;
}

static struct platform_driver ath79_i2s_driver = {
	.probe = ath79_i2s_drv_probe,
	.remove = __devexit_p(ath79_i2s_drv_remove),

	.driver = {
		.name = "ath79-i2s",
		.owner = THIS_MODULE,
	},
};

static int __init ath79_i2s_init(void)
{
	printk(KERN_CRIT "%s called\n", __FUNCTION__);
	return platform_driver_register(&ath79_i2s_driver);
}

static void __exit ath79_i2s_exit(void)
{
	printk(KERN_CRIT "%s called\n", __FUNCTION__);
	platform_driver_unregister(&ath79_i2s_driver);
}

module_init(ath79_i2s_init);
module_exit(ath79_i2s_exit);

MODULE_AUTHOR("Qualcomm-Atheros");
MODULE_DESCRIPTION("QCA Audio DAI (i2s) module");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_ALIAS("platform:ath79-i2s");
