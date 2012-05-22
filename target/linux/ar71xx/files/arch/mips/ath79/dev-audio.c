/*
 *  Atheros AR71xx Audio driver code
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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/delay.h>

#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>

#include "dev-audio.h"
#include "audio-lib.h"

void __iomem *ath79_dma_base;
EXPORT_SYMBOL_GPL(ath79_dma_base);

void __iomem *ath79_stereo_base;
EXPORT_SYMBOL_GPL(ath79_stereo_base);

void __iomem *ath79_audio_dpll_base;
EXPORT_SYMBOL_GPL(ath79_audio_dpll_base);

static struct platform_device ath79_i2s_device = {
	.name		= "ath79-i2s",
	.id		= -1,
};

static struct platform_device ath79_pcm_device = {
	.name		= "ath79-pcm-audio",
	.id		= -1,
};

void __init ath79_audio_device_register(void)
{
	platform_device_register(&ath79_i2s_device);
	platform_device_register(&ath79_pcm_device);
}

void __init ath79_audio_init(void)
{
	ath79_dma_base = ioremap_nocache(AR934X_DMA_BASE,
		AR934X_DMA_SIZE);
	ath79_stereo_base = ioremap_nocache(AR934X_STEREO_BASE,
		AR934X_STEREO_SIZE);
	ath79_audio_dpll_base = ioremap_nocache(AR934X_AUD_DPLL_BASE,
		AR934X_AUD_DPLL_SIZE);

	ath79_stereo_wr(AR934X_STEREO_REG_CONFIG,
		AR934X_STEREO_CONFIG_SPDIF_ENABLE |
		AR934X_STEREO_CONFIG_I2S_ENABLE |
		AR934X_STEREO_CONFIG_DATA_WORD_16 << AR934X_STEREO_CONFIG_DATA_WORD_SIZE_SHIFT |
		AR934X_STEREO_CONFIG_SAMPLE_CNT_CLEAR_TYPE |
		AR934X_STEREO_CONFIG_MASTER |
		0x2 << AR934X_STEREO_CONFIG_POSEDGE_SHIFT);
	ath79_stereo_reset_set();
	udelay(100);
	ath79_stereo_reset_clear();

	ath79_dma_wr(AR934X_DMA_REG_MBOX_DMA_POLICY,
			0x6 << AR934X_DMA_MBOX_DMA_POLICY_TX_FIFO_THRESH_SHIFT |
			AR934X_DMA_MBOX_DMA_POLICY_RX_QUANTUM);
}
EXPORT_SYMBOL(ath79_audio_init);

int ath79_audio_set_freq(int freq)
{
	struct clk *clk;
	const struct ath79_pll_config *cfg;

	clk = clk_get(NULL, "ref");

	/* PLL settings can have 2 different values depending
	 * on the clock rate */
	switch(clk_get_rate(clk)) {
	case 25*1000*1000:
		cfg = &pll_cfg_25MHz[0];
		break;
	case 40*1000*1000:
		cfg = &pll_cfg_40MHz[0];
		break;
	default:
		printk(KERN_ERR "%s: Clk speed %lu.%03lu not supported\n", __FUNCTION__,
			clk_get_rate(clk)/1000000,(clk_get_rate(clk)/1000) % 1000);
		return -EIO;
	}

	/* Search the frequency in the pll table */
	do {
		if(cfg->rate == freq)
			break;
		cfg++;
	} while(cfg->rate != 0);
	if (cfg->rate == 0) {
		printk(KERN_ERR "%s: Freq %d not supported\n",
			__FUNCTION__, freq);
		return -EIO;
	}

	/* Loop until we converged to an acceptable value */
	do {
		ath79_audiodpll_do_meas_clear();
		ath79_pll_powerdown();
		udelay(100);

		ath79_load_pll_regs(cfg);

		ath79_pll_powerup();
		ath79_audiodpll_do_meas_clear();
		ath79_audiodpll_do_meas_set();

		while ( ! ath79_audiodpll_meas_done_is_set()) {
			udelay(10);
		}

	} while (ath79_audiodpll_sqsum_dvc_get() >= 0x40000);

	return 0;
}
EXPORT_SYMBOL(ath79_audio_set_freq);
