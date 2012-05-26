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
#include <linux/platform_device.h>
#include <linux/module.h>

#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>

#include "dev-audio.h"

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

void __init ath79_audio_setup(void)
{
	ath79_dma_base = ioremap_nocache(AR934X_DMA_BASE,
		AR934X_DMA_SIZE);
	ath79_stereo_base = ioremap_nocache(AR934X_STEREO_BASE,
		AR934X_STEREO_SIZE);
	ath79_audio_dpll_base = ioremap_nocache(AR934X_AUD_DPLL_BASE,
		AR934X_AUD_DPLL_SIZE);
}
