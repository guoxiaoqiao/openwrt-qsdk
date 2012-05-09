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
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/delay.h>

#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>

#include "dev-audio.h"

void __iomem *ath79_dma_base;
EXPORT_SYMBOL_GPL(ath79_dma_base);

void __iomem *ath79_stereo_base;
EXPORT_SYMBOL_GPL(ath79_stereo_base);

void __iomem *ath79_audio_dpll_base;
EXPORT_SYMBOL_GPL(ath79_audio_dpll_base);

static DEFINE_SPINLOCK(ath79_audio_lock);

static struct platform_device ath79_i2s_device = {
	.name		= "wasp-i2s",
	.id		= -1,
};

static struct platform_device ath79_pcm_device = {
	.name		= "wasp-pcm-audio",
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
		AR934X_STEREO_CONFIG_PCM_SWAP |
		AR934X_STEREO_CONFIG_DATA_WORD_16 << AR934X_STEREO_CONFIG_DATA_WORD_SIZE_SHIFT |
		AR934X_STEREO_CONFIG_SAMPLE_CNT_CLEAR_TYPE |
		AR934X_STEREO_CONFIG_MASTER |
		0x2 << AR934X_STEREO_CONFIG_POSEDGE_SHIFT);
	ath79_stereo_reset_set();
	udelay(100);
	ath79_stereo_reset_clear();

	ath79_audio_set_freq(44100);
}
EXPORT_SYMBOL(ath79_audio_init);

static void ath79_pll_powerup(void)
{
	unsigned long flags;
	u32 t;

	spin_lock_irqsave(&ath79_audio_lock, flags);

	t = ath79_pll_rr(AR934X_PLL_AUDIO_CONFIG_REG);
	t &= ~AR934X_PLL_AUDIO_CONFIG_PLLPWD;
	ath79_pll_wr(AR934X_PLL_AUDIO_CONFIG_REG, t);

	spin_unlock_irqrestore(&ath79_audio_lock, flags);
}

static void ath79_pll_powerdown(void)
{
	unsigned long flags;
	u32 t;

	spin_lock_irqsave(&ath79_audio_lock, flags);

	t = ath79_pll_rr(AR934X_PLL_AUDIO_CONFIG_REG);
	t |= AR934X_PLL_AUDIO_CONFIG_PLLPWD;
	ath79_pll_wr(AR934X_PLL_AUDIO_CONFIG_REG, t);

	spin_unlock_irqrestore(&ath79_audio_lock, flags);
}

static bool ath79_pll_ispowered(void)
{
	u32 status;

	status = ath79_pll_rr(AR934X_PLL_AUDIO_CONFIG_REG)
			& AR934X_PLL_AUDIO_CONFIG_PLLPWD;
	return ( !status ? true : false);
}

static void ath79_pll_set_target_div(u32 div_int, u32 div_frac)
{
	unsigned long flags;
	u32 t;

	spin_lock_irqsave(&ath79_audio_lock, flags);

	t = ath79_pll_rr(AR934X_PLL_AUDIO_MOD_REG);
	t &= ~(AR934X_PLL_AUDIO_MOD_TGT_DIV_INT_MASK
		<< AR934X_PLL_AUDIO_MOD_TGT_DIV_INT_SHIFT);
	t &= ~(AR934X_PLL_AUDIO_MOD_TGT_DIV_FRAC_MASK
		<< AR934X_PLL_AUDIO_MOD_TGT_DIV_FRAC_SHIFT);
	t |= (div_int & AR934X_PLL_AUDIO_MOD_TGT_DIV_INT_MASK)
		<< AR934X_PLL_AUDIO_MOD_TGT_DIV_INT_SHIFT;
	t |= (div_frac & AR934X_PLL_AUDIO_MOD_TGT_DIV_FRAC_MASK)
		<< AR934X_PLL_AUDIO_MOD_TGT_DIV_FRAC_SHIFT;
	ath79_pll_wr(AR934X_PLL_AUDIO_MOD_REG, t);

	spin_unlock_irqrestore(&ath79_audio_lock, flags);
}

static void ath79_pll_set_refdiv(u32 refdiv)
{
	unsigned long flags;
	u32 t;

	spin_lock_irqsave(&ath79_audio_lock, flags);

	t = ath79_pll_rr(AR934X_PLL_AUDIO_CONFIG_REG);
	t &= ~(AR934X_PLL_AUDIO_CONFIG_REFDIV_MASK
		<< AR934X_PLL_AUDIO_CONFIG_REFDIV_SHIFT);
	t |= (refdiv & AR934X_PLL_AUDIO_CONFIG_REFDIV_MASK)
		<< AR934X_PLL_AUDIO_CONFIG_REFDIV_SHIFT;
	ath79_pll_wr(AR934X_PLL_AUDIO_CONFIG_REG, t);

	spin_unlock_irqrestore(&ath79_audio_lock, flags);
}

static void ath79_pll_set_ext_div(u32 ext_div)
{
	unsigned long flags;
	u32 t;

	spin_lock_irqsave(&ath79_audio_lock, flags);

	t = ath79_pll_rr(AR934X_PLL_AUDIO_CONFIG_REG);
	t &= ~(AR934X_PLL_AUDIO_CONFIG_EXT_DIV_MASK
		<< AR934X_PLL_AUDIO_CONFIG_EXT_DIV_SHIFT);
	t |= (ext_div & AR934X_PLL_AUDIO_CONFIG_EXT_DIV_MASK)
		<< AR934X_PLL_AUDIO_CONFIG_EXT_DIV_SHIFT;
	ath79_pll_wr(AR934X_PLL_AUDIO_CONFIG_REG, t);

	spin_unlock_irqrestore(&ath79_audio_lock, flags);
}

static void ath79_pll_set_postpllpwd(u32 postpllpwd)
{
	unsigned long flags;
	u32 t;

	spin_lock_irqsave(&ath79_audio_lock, flags);

	t = ath79_pll_rr(AR934X_PLL_AUDIO_CONFIG_REG);
	t &= ~(AR934X_PLL_AUDIO_CONFIG_POSTPLLPWD_MASK
		<< AR934X_PLL_AUDIO_CONFIG_POSTPLLPWD_SHIFT);
	t |= (postpllpwd & AR934X_PLL_AUDIO_CONFIG_POSTPLLPWD_MASK)
		<< AR934X_PLL_AUDIO_CONFIG_POSTPLLPWD_SHIFT;
	ath79_pll_wr(AR934X_PLL_AUDIO_CONFIG_REG, t);

	spin_unlock_irqrestore(&ath79_audio_lock, flags);
}

static void ath79_pll_clear_bypass(void)
{
	unsigned long flags;
	u32 t;

	spin_lock_irqsave(&ath79_audio_lock, flags);

	t = ath79_pll_rr(AR934X_PLL_AUDIO_CONFIG_REG);
	t &= ~(AR934X_PLL_AUDIO_CONFIG_BYPASS);
	ath79_pll_wr(AR934X_PLL_AUDIO_CONFIG_REG, t);

	spin_unlock_irqrestore(&ath79_audio_lock, flags);
}

static void ath79_audiodpll_do_meas_set(void)
{
	unsigned long flags;
	u32 t;

	spin_lock_irqsave(&ath79_audio_lock, flags);

	t = ath79_audio_dpll_rr(AR934X_DPLL_REG_3);
	t |= AR934X_DPLL_3_DO_MEAS;
	ath79_audio_dpll_wr(AR934X_DPLL_REG_3, t);

	spin_unlock_irqrestore(&ath79_audio_lock, flags);
}

static void ath79_audiodpll_do_meas_clear(void)
{
	unsigned long flags;
	u32 t;

	spin_lock_irqsave(&ath79_audio_lock, flags);

	t = ath79_audio_dpll_rr(AR934X_DPLL_REG_3);
	t &= ~(AR934X_DPLL_3_DO_MEAS);
	ath79_audio_dpll_wr(AR934X_DPLL_REG_3, t);

	spin_unlock_irqrestore(&ath79_audio_lock, flags);
}

static bool ath79_audiodpll_meas_done_is_set(void)
{
	u32 status;

	status = ath79_audio_dpll_rr(AR934X_DPLL_REG_4) & AR934X_DPLL_4_MEAS_DONE;
	return ( status ? true : false);
}

static void ath79_audiodpll_set_gains(u32 kd, u32 ki)
{
	unsigned long flags;
	u32 t;

	spin_lock_irqsave(&ath79_audio_lock, flags);

	if(ath79_pll_ispowered())
		BUG();

	t = ath79_audio_dpll_rr(AR934X_DPLL_REG_2);
	t &= ~(AR934X_DPLL_2_KD_MASK << AR934X_DPLL_2_KD_SHIFT);
	t &= ~(AR934X_DPLL_2_KI_MASK << AR934X_DPLL_2_KI_SHIFT);
	t |= (kd & AR934X_DPLL_2_KD_MASK) << AR934X_DPLL_2_KD_SHIFT;
	t |= (ki & AR934X_DPLL_2_KI_MASK) << AR934X_DPLL_2_KI_SHIFT;
	ath79_audio_dpll_wr(AR934X_DPLL_REG_2, t);

	spin_unlock_irqrestore(&ath79_audio_lock, flags);
}

static void ath79_audiodpll_phase_shift_set(u32 phase)
{
	unsigned long flags;
	u32 t;

	spin_lock_irqsave(&ath79_audio_lock, flags);

	if(ath79_pll_ispowered())
		BUG();

	t = ath79_audio_dpll_rr(AR934X_DPLL_REG_3);
	t &= ~(AR934X_DPLL_3_PHASESH_MASK << AR934X_DPLL_3_PHASESH_SHIFT);
	t |= (phase & AR934X_DPLL_3_PHASESH_MASK)
		<< AR934X_DPLL_3_PHASESH_SHIFT;
	ath79_audio_dpll_wr(AR934X_DPLL_REG_3, t);

	spin_unlock_irqrestore(&ath79_audio_lock, flags);
}

static void ath79_audiodpll_range_set(void)
{
	unsigned long flags;
	u32 t;

	spin_lock_irqsave(&ath79_audio_lock, flags);

	t = ath79_audio_dpll_rr(AR934X_DPLL_REG_2);
	t |= AR934X_DPLL_2_RANGE;
	ath79_audio_dpll_wr(AR934X_DPLL_REG_2, t);

	spin_unlock_irqrestore(&ath79_audio_lock, flags);
}

u32 ath79_audiodpll_sqsum_dvc_get(void)
{
	u32 t;

	t = ath79_audio_dpll_rr(AR934X_DPLL_REG_3) >> AR934X_DPLL_3_SQSUM_DVC_SHIFT;
	t &= AR934X_DPLL_3_SQSUM_DVC_MASK;
	return t;
}

static void ath79_stereo_set_posedge(u32 posedge)
{
	unsigned long flags;
	u32 t;

	spin_lock_irqsave(&ath79_audio_lock, flags);

	t = ath79_audio_dpll_rr(AR934X_STEREO_REG_CONFIG);
	t &= ~(AR934X_STEREO_CONFIG_POSEDGE_MASK
		<< AR934X_STEREO_CONFIG_POSEDGE_SHIFT);
	t |= (posedge & AR934X_STEREO_CONFIG_POSEDGE_MASK)
		<< AR934X_STEREO_CONFIG_POSEDGE_SHIFT;
	ath79_audio_dpll_wr(AR934X_STEREO_REG_CONFIG, t);

	spin_unlock_irqrestore(&ath79_audio_lock, flags);
}

void ath79_stereo_reset_set(void)
{
	unsigned long flags;
	u32 t;

	spin_lock_irqsave(&ath79_audio_lock, flags);

	t = ath79_stereo_rr(AR934X_STEREO_REG_CONFIG);
	t |= AR934X_STEREO_CONFIG_RESET;
	ath79_stereo_wr(AR934X_STEREO_CONFIG_RESET, t);

	spin_unlock_irqrestore(&ath79_audio_lock, flags);
}
EXPORT_SYMBOL(ath79_stereo_reset_set);

void ath79_stereo_reset_clear(void)
{
	unsigned long flags;
	u32 t;

	spin_lock_irqsave(&ath79_audio_lock, flags);

	t = ath79_stereo_rr(AR934X_STEREO_REG_CONFIG);
	t &= ~(AR934X_STEREO_CONFIG_RESET);
	ath79_stereo_wr(AR934X_STEREO_REG_CONFIG, t);

	spin_unlock_irqrestore(&ath79_audio_lock, flags);
}
EXPORT_SYMBOL(ath79_stereo_reset_clear);

int ath79_audio_set_freq(int freq)
{
	struct clk *clk;

	clk = clk_get(NULL, "ref");

	do {
		ath79_audiodpll_do_meas_clear();
		ath79_pll_powerdown();
		udelay(100);

		/* PLL settings can have 2 different values depending
		 * on the clock rate
		 */
		ath79_audiodpll_phase_shift_set(0x6);
		ath79_pll_set_postpllpwd(0x3);
		ath79_pll_clear_bypass();
		ath79_pll_set_ext_div(0x6);
		ath79_audiodpll_range_set();
		if (clk_get_rate(clk) == 25 * 1000 * 1000) {
			ath79_audiodpll_set_gains(0x3d, 0x4);
			ath79_pll_set_refdiv(1);
			switch(freq) {
			case 22050:
				ath79_pll_set_target_div(0x15, 0x2B442);
				ath79_stereo_set_posedge(3);
				break;
			case 32000:
				ath79_pll_set_target_div(0x17, 0x24F76);
				ath79_stereo_set_posedge(3);
				break;
			case 44100:
				ath79_pll_set_target_div(0x15, 0x2B442);
				ath79_stereo_set_posedge(2);
				break;
			case 48000:
				ath79_pll_set_target_div(0x17, 0x24F76);
				ath79_stereo_set_posedge(2);
				break;
			case 88200:
				ath79_pll_set_target_div(0x15, 0x2B442);
				ath79_stereo_set_posedge(1);
				break;
			case 96000:
				ath79_pll_set_target_div(0x17, 0x24F76);
				ath79_stereo_set_posedge(1);
				break;
			default:
				printk(KERN_ERR "%s: Frequency %d unsupported\n",
					__FUNCTION__, freq);
				break;
			}
		}
		if (clk_get_rate(clk) == 40 * 1000 * 1000) {
			ath79_audiodpll_set_gains(0x32, 0x4);
			ath79_pll_set_refdiv(2);
			switch(freq) {
			case 22050:
				ath79_pll_set_target_div(0x1B, 0x6152);
				ath79_stereo_set_posedge(3);
				break;
			case 32000:
				ath79_pll_set_target_div(0x1D, 0x1F6FD);
				ath79_stereo_set_posedge(3);
				break;
			case 44100:
				ath79_pll_set_target_div(0x1B, 0x6152);
				ath79_stereo_set_posedge(2);
				break;
			case 48000:
				ath79_pll_set_target_div(0x1D, 0x1F6FD);
				ath79_stereo_set_posedge(2);
				break;
			case 88200:
				ath79_pll_set_target_div(0x1B, 0x6152);
				ath79_stereo_set_posedge(1);
				break;
			case 96000:
				ath79_pll_set_target_div(0x1D, 0x1F6FD);
				ath79_stereo_set_posedge(1);
				break;
			default:
				printk(KERN_ERR "%s: Frequency %d unsupported\n",
					__FUNCTION__, freq);
				return -EOPNOTSUPP;
				break;
			}
		}
		ath79_pll_powerup();
		ath79_audiodpll_do_meas_clear();
		ath79_audiodpll_do_meas_set();

		while ( ! ath79_audiodpll_meas_done_is_set()) {
			udelay(10);
		}

	} while (ath79_audiodpll_sqsum_dvc_get() >= 0x40000);

	return 0;
}

