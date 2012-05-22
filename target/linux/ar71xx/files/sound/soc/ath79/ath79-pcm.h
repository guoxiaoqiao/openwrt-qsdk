/*
 * ath-pcm.h -- ALSA PCM interface for the QCA Wasp based audio interface
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

#ifndef _ATH_PCM_H_
#define _ATH_PCM_H_

#include <linux/list.h>

/*
 * Renamed struct ath_mbox_dma_desc
 * MBOX Hardware descriptor
 * Note that a list is appended to this structure so that
 * we can parse descriptors from the CPU using virtual addresses
 */
struct ath79_pcm_desc {
	unsigned int	OWN	:  1,    /* bit 00 */
			EOM	:  1,    /* bit 01 */
			rsvd1	:  6,    /* bit 07-02 */
			size	: 12,    /* bit 19-08 */
			length	: 12,    /* bit 31-20 */
			rsvd2	:  4,    /* bit 00 */
			BufPtr	: 28,    /* bit 00 */
			rsvd3	:  4,    /* bit 00 */
			NextPtr	: 28;    /* bit 00 */

	unsigned int Va[6];
	unsigned int Ua[6];
	unsigned int Ca[6];
	unsigned int Vb[6];
	unsigned int Ub[6];
	unsigned int Cb[6];

	/* Software specific data
	 * These data are not taken into account by the HW */
	struct list_head list; /* List linking all the buffer in virt@ space */
	dma_addr_t phys; /* Physical address of the descriptor */
};

struct ath79_pcm_rt_priv {
	struct list_head dma_head;
	struct ath79_pcm_desc *last_played;
};

/* Replaces struct ath_i2s_softc */
struct ath79_pcm_pltfm_priv {
	struct snd_pcm_substream *playback;
	struct snd_pcm_substream *capture;
};

/* platform data */
extern struct snd_soc_platform_driver ath79_soc_platform;

#endif
