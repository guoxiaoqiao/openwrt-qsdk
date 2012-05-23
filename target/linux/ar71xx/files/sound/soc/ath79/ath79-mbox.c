/*
 * ath79-mbox.c -- ALSA MBOX DMA management functions
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

#include <linux/dma-mapping.h>
#include <linux/types.h>
#include <linux/dmapool.h>
#include <linux/delay.h>
#include <sound/core.h>
#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>
#include "ath79-pcm.h"

spinlock_t ath79_pcm_lock;
static struct dma_pool *ath79_pcm_cache;

void ath79_mbox_dma_start(struct ath79_pcm_rt_priv *rtpriv)
{
	printk(KERN_NOTICE "%s: Starting DMA...\n", __FUNCTION__);
	ath79_pcm_set_own_bits(rtpriv);
	ath79_dma_wr(AR934X_DMA_REG_MBOX0_DMA_RX_CONTROL,
		     AR934X_DMA_MBOX_DMA_CONTROL_START);
}

void ath79_mbox_dma_stop(struct ath79_pcm_rt_priv *rtpriv)
{
	printk(KERN_NOTICE "%s: Stopping DMA...\n", __FUNCTION__);
	ath79_dma_wr(AR934X_DMA_REG_MBOX0_DMA_RX_CONTROL,
		     AR934X_DMA_MBOX_DMA_CONTROL_STOP);
	mdelay(100);
	ath79_pcm_clear_own_bits(rtpriv);
}

void ath79_mbox_dma_prepare(struct ath79_pcm_rt_priv *rtpriv)
{
	struct ath79_pcm_desc *desc;

	printk(KERN_NOTICE "%s: Prepare DMA transfer...\n", __FUNCTION__);
	/* Request the DMA channel to the controller */
	ath79_dma_wr(AR934X_DMA_REG_MBOX_DMA_POLICY,
		     AR934X_DMA_MBOX_DMA_POLICY_RX_QUANTUM |
		     (6 << AR934X_DMA_MBOX_DMA_POLICY_TX_FIFO_THRESH_SHIFT));

	/* The direction is indicated from the DMA engine perspective
	 * i.e. we'll be using the RX registers for Playback and
	 * the TX registers for capture */
	desc = list_first_entry(&rtpriv->dma_head, struct ath79_pcm_desc, list);
	ath79_dma_wr(AR934X_DMA_REG_MBOX0_DMA_RX_DESCRIPTOR_BASE,
		     (u32) desc->phys);
	ath79_mbox_set_interrupt(AR934X_DMA_MBOX0_INT_RX_COMPLETE);

	/* Reset the DMA MBOX controller */
	ath79_dma_wr(AR934X_DMA_REG_MBOX_FIFO_RESET,
			AR934X_DMA_MBOX0_FIFO_RESET_RX);
}

int ath79_mbox_dma_map(struct ath79_pcm_rt_priv *rtpriv, dma_addr_t baseaddr,
			      int period_bytes,int bufsize)
{
	struct list_head *head = &rtpriv->dma_head;
	struct ath79_pcm_desc *desc, *prev;
	dma_addr_t desc_p;
	unsigned int offset = 0;

	spin_lock(&ath79_pcm_lock);

	/* We loop until we have enough buffers to map the whole DMA area */
	do {
		/* Allocate a descriptor and insert it into the DMA ring */
		desc = dma_pool_alloc(ath79_pcm_cache, GFP_KERNEL, &desc_p);
		if(!desc) {
			return -ENOMEM;
		}
		desc->phys = desc_p;
		list_add_tail(&desc->list, head);

		desc->OWN = 1;
		desc->rsvd1 = desc->rsvd2 = desc->rsvd3 = desc->EOM = 0;

		/* buffer size may not be a multiple of period_bytes */
		if (bufsize >= offset + period_bytes) {
			desc->size = period_bytes;
		} else {
			desc->size = bufsize - offset;
		}
		desc->BufPtr = baseaddr + offset;

		/* For now, we assume the buffer is always full
		 * -->length == size */
		desc->length = desc->size;

		/* We need to make sure we are not the first descriptor.
		 * If we are, prev doesn't point to a struct ath79_pcm_desc */
		if (desc->list.prev != head) {
			prev =
			    list_entry(desc->list.prev, struct ath79_pcm_desc,
				       list);
			prev->NextPtr = desc->phys;
		}

		offset += desc->size;
	} while (offset < bufsize);

	/* Once all the descriptors have been created, we can close the loop
	 * by pointing from the last one to the first one */
	desc = list_first_entry(head, struct ath79_pcm_desc, list);
	prev = list_entry(head->prev, struct ath79_pcm_desc, list);
	prev->NextPtr = desc->phys;

	printk(KERN_NOTICE "%s - %s:\n", __FILE__, __FUNCTION__);
	list_for_each_entry(desc, &rtpriv->dma_head, list) {
	        printk(KERN_NOTICE
	               "Desc@=%08x OWN=%d EOM=%d Length=%d Size=%d\nBuf=%08x Next=%08x\n",
	               (u32) desc, (u32) desc->OWN, (u32) desc->EOM, (u32) desc->length, desc->size,
	               (u32) desc->BufPtr, (u32) desc->NextPtr);
	}

	spin_unlock(&ath79_pcm_lock);

	return 0;
}

void ath79_mbox_dma_unmap(struct ath79_pcm_rt_priv *rtpriv)
{
	struct list_head *head = &rtpriv->dma_head;
	struct ath79_pcm_desc *desc, *n;

	spin_lock(&ath79_pcm_lock);
	list_for_each_entry_safe(desc, n, head, list) {
		list_del(&desc->list);
		dma_pool_free(ath79_pcm_cache, desc, desc->phys);
	}
	spin_unlock(&ath79_pcm_lock);

	return;
}

int ath79_mbox_dma_init(struct device *dev)
{
	int ret = 0;

	/* Allocate a DMA pool to store the MBOX descriptor */
	ath79_pcm_cache = dma_pool_create("ath79_pcm_pool", dev,
					 sizeof(struct ath79_pcm_desc), 4, 0);
	if (!ath79_pcm_cache)
		ret = -ENOMEM;

	return ret;
}

void ath79_mbox_dma_exit(void)
{
	dma_pool_destroy(ath79_pcm_cache);
}
