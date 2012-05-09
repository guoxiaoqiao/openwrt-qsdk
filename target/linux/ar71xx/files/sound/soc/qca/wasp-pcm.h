/*
 * ath-pcm.h -- ALSA PCM interface for the QCA Wasp based audio interface
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

#ifndef _ATH_PCM_H_
#define _ATH_PCM_H_

/* Renamed struct ath_mbox_dma_desc */
struct wasp_pcm_desc {
	unsigned int OWN		:  1,    /* bit 00 */
	             EOM		:  1,    /* bit 01 */
	             rsvd1	    :  6,    /* bit 07-02 */
	             size	    : 12,    /* bit 19-08 */
	             length	    : 12,    /* bit 31-20 */
	             rsvd2	    :  4,    /* bit 00 */
	             BufPtr	    : 28,    /* bit 00 */
	             rsvd3	    :  4,    /* bit 00 */
	             NextPtr	: 28;    /* bit 00 */

    unsigned int Va[6];
    unsigned int Ua[6];
    unsigned int Ca[6];
    unsigned int Vb[6];
    unsigned int Ub[6];
    unsigned int Cb[6];
} wasp_pcm_desc;

/* Replaces struct i2s_dma_buf */
struct wasp_pcm_data {
	struct wasp_pcm_desc *dma_desc_array;
	dma_addr_t dma_desc_array_phys;
};

/* Replaces struct ath_i2s_softc */
struct wasp_pcm_priv {
	struct snd_pcm_substream *playback;
	struct snd_pcm_substream *capture;
};

/* platform data */
extern struct snd_soc_platform_driver wasp_soc_platform;

#endif
