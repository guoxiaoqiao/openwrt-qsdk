/*
 * wasp-pll.c -- Wasp specific lib to configure internal Audio PLL
 *
 * Copyright (c) 2012 Atheros Communications Inc.
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

#include <asm/mach-ar71xx/ar71xx.h>
#include "wasp-pll.h"
#include "ath_i2s.h"

void wasp_i2s_clk_set(u32 frac, u32 pll)
{
	u32 t;

	ar71xx_pll_wr(AR934X_PLL_AUDIO_MODULATION, frac);
	ar71xx_pll_wr(AR934X_PLL_AUDIO_CONFIG, pll);
	t = ar71xx_pll_rr(AR934X_PLL_AUDIO_CONFIG);
	ar71xx_pll_wr(AR934X_PLL_AUDIO_CONFIG, t & ~AR934X_PLL_AUDIO_CONFIG_PWR_DOWN);
}

void wasp_i2s_dpll(u32 kd, u32 ki)
{
	u32 t;
	uint32_t	i = 0;

	do {
		ath_reg_rmw_clear(ATH_AUD_DPLL3_ADDRESS, ATH_AUD_DPLL3_DO_MEAS_SET(1));
		ath_reg_rmw_set(ATH_AUDIO_PLL_CONFIG, ATH_AUDIO_PLL_CFG_PWR_DWN);
		udelay(100);
		// Configure AUDIO DPLL
		ath_reg_rmw_clear(ATH_AUD_DPLL2_ADDRESS, ATH_AUD_DPLL2_KI_MASK | ATH_AUD_DPLL2_KD_MASK);
		ath_reg_rmw_set(ATH_AUDIO_DPLL2, ATH_AUD_DPLL2_KI_SET(ki) | ATH_AUD_DPLL2_KD_SET(kd));
		ath_reg_rmw_clear(ATH_AUD_DPLL3_ADDRESS, ATH_AUD_DPLL3_PHASE_SHIFT_MASK);
		ath_reg_rmw_set(ATH_AUD_DPLL3_ADDRESS, ATH_AUD_DPLL3_PHASE_SHIFT_SET(0x6));
		if (!is_ar934x_10()) {
			ath_reg_rmw_clear(ATH_AUD_DPLL2_ADDRESS, ATH_AUD_DPLL2_RANGE_SET(1));
			ath_reg_rmw_set(ATH_AUD_DPLL2_ADDRESS, ATH_AUD_DPLL2_RANGE_SET(1));
		}
		ath_reg_rmw_clear(ATH_AUDIO_PLL_CONFIG, ATH_AUDIO_PLL_CFG_PWR_DWN);

		ath_reg_rmw_clear(ATH_AUD_DPLL3_ADDRESS, ATH_AUD_DPLL3_DO_MEAS_SET(1));
		udelay(100);
		ath_reg_rmw_set(ATH_AUD_DPLL3_ADDRESS, ATH_AUD_DPLL3_DO_MEAS_SET(1));
		udelay(100);

		while ((ath_reg_rd(ATH_AUD_DPLL4_ADDRESS) & ATH_AUD_DPLL4_MEAS_DONE_SET(1)) == 0) {
			udelay(10);
		}
		udelay(100);

		i ++;

	} while (ATH_AUD_DPLL3_SQSUM_DVC_GET(ath_reg_rd(ATH_AUD_DPLL3_ADDRESS)) >= 0x40000);

	printk("\tAud:	0x%x 0x%x\n", KSEG1ADDR(ATH_AUD_DPLL3_ADDRESS),
			ATH_AUD_DPLL3_SQSUM_DVC_GET(ath_reg_rd(ATH_AUD_DPLL3_ADDRESS)));
}
