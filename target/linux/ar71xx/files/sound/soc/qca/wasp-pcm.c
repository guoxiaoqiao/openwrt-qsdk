/*
 * ath-pcm.c -- ALSA PCM interface for the QCA Wasp based audio interface
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

#include <linux/dma-mapping.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/dmapool.h>
#include <linux/delay.h>
#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>

#include <linux/mm.h>

#include <sound/core.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>

#include "wasp-pcm.h"

#define ATH_I2S_NUM_DESC 160
#define ATH_I2S_BUFF_SIZE 24

static struct dma_pool *wasp_pcm_cache;
static spinlock_t wasp_pcm_lock;

static struct snd_pcm_hardware wasp_pcm_hardware = {
	.info = SNDRV_PCM_INFO_MMAP |
		SNDRV_PCM_INFO_MMAP_VALID |
		SNDRV_PCM_INFO_INTERLEAVED |
		SNDRV_PCM_INFO_NO_PERIOD_WAKEUP,
	.formats = SNDRV_PCM_FMTBIT_S16_LE,
	.rates = SNDRV_PCM_RATE_44100,
	.rate_min = 44100,
	.rate_max = 44100,
	.channels_min = 2,
	.channels_max = 2,
	/* These numbers are pretty random. As the DMA engine is descriptor base
	 * the only real limitation we have is the amount of RAM.
	 * Ideally, we'd need to find the best tradeoff between number of descs
	 * and CPU load */
	.buffer_bytes_max = 32768,
	.period_bytes_min = 64,
	.period_bytes_max = 4096,
	.periods_min = 4,
	.periods_max = 32,
	.fifo_size = 0,
};

static inline struct wasp_pcm_desc *wasp_pcm_get_last_played(struct wasp_pcm_rt_priv *rtpriv)
{
	struct wasp_pcm_desc *desc, *prev;

	prev = list_entry(rtpriv->dma_head.prev, struct wasp_pcm_desc, list);
	list_for_each_entry(desc, &rtpriv->dma_head, list) {
		if (desc->OWN == 1 && prev->OWN == 0) {
			return desc;
		}
		prev = desc;
	}

	/* If we didn't find the last played buffer, return NULL */
	return NULL;
}

static inline void wasp_pcm_set_own_bit(struct wasp_pcm_rt_priv *rtpriv)
{
	struct wasp_pcm_desc *desc;

	list_for_each_entry(desc, &rtpriv->dma_head, list) {
		if (desc->OWN == 0) {
			desc->OWN = 1;
		}
	}
}

static inline void wasp_pcm_clear_own_bit(struct wasp_pcm_rt_priv *rtpriv)
{
	struct wasp_pcm_desc *desc;

	list_for_each_entry(desc, &rtpriv->dma_head, list) {
		if (desc->OWN == 1) {
			desc->OWN = 0;
		}
	}
}

static irqreturn_t wasp_pcm_interrupt(int irq, void *dev_id)
{
	uint32_t status;
	struct wasp_pcm_pltfm_priv *prdata = dev_id;
	struct wasp_pcm_rt_priv *rtpriv;

	status = ath79_dma_rr(AR934X_DMA_REG_MBOX_INT_STATUS);

	/* Notify the corresponding ALSA substream */
	if(status & AR934X_DMA_MBOX_INT_STATUS_RX_DMA_COMPLETE) {
		rtpriv = prdata->playback->runtime->private_data;
		/* Store the last played buffer in the runtime priv struct */
		rtpriv->last_played = wasp_pcm_get_last_played(rtpriv);
		wasp_pcm_set_own_bit(rtpriv);
		if (rtpriv->last_played == NULL) {
			snd_printd(KERN_ERR "BUG: ISR called but no played buf\n");
			goto ack;
		}
		snd_pcm_period_elapsed(prdata->playback);
	}
	if(status & AR934X_DMA_MBOX_INT_STATUS_TX_DMA_COMPLETE) {
		rtpriv = prdata->capture->runtime->private_data;
		/* Store the last played buffer in the runtime priv struct */
		rtpriv->last_played = wasp_pcm_get_last_played(rtpriv);
		wasp_pcm_set_own_bit(rtpriv);
		if (rtpriv->last_played == NULL) {
			snd_printd(KERN_ERR "BUG: ISR called but no recorded buf\n");
			goto ack;
		}
		snd_pcm_period_elapsed(prdata->capture);
	}

ack:
	/* Ack the interrupt */
	ath79_dma_wr(AR934X_DMA_REG_MBOX_INT_STATUS, status);

	return IRQ_HANDLED;
}

static int wasp_pcm_open(struct snd_pcm_substream *ss)
{
	struct snd_soc_pcm_runtime *runtime = ss->private_data;
	struct snd_soc_platform *platform = runtime->platform;
	struct wasp_pcm_pltfm_priv *prdata = snd_soc_platform_get_drvdata(platform);
	struct wasp_pcm_rt_priv *rtpriv;
	int err;

	if (prdata == NULL) {
		prdata = kzalloc(sizeof(struct wasp_pcm_pltfm_priv), GFP_KERNEL);
		if (prdata == NULL)
			return -ENOMEM;

		err = request_irq(ATH79_MISC_IRQ_DMA, wasp_pcm_interrupt, 0,
				  "wasp-pcm", prdata);
		if (err) {
			kfree(prdata);
			return -EBUSY;
		}

		snd_soc_platform_set_drvdata(platform, prdata);
	}

	if (ss->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		prdata->playback = ss;
	} else {
		prdata->capture = ss;
	}

	/* Allocate/Initialize the buffer linked list head */
	rtpriv = kmalloc(sizeof(*rtpriv), GFP_KERNEL);
	if (!rtpriv) {
		return -ENOMEM;
	}
	printk(KERN_NOTICE "%s: 0x%xB allocated at 0x%08x\n",
	       __FUNCTION__, sizeof(*rtpriv), (u32) rtpriv);

	ss->runtime->private_data = rtpriv;
	rtpriv->last_played = NULL;
	INIT_LIST_HEAD(&rtpriv->dma_head);

	snd_soc_set_runtime_hwparams(ss, &wasp_pcm_hardware);

	return 0;
}

static int wasp_pcm_close(struct snd_pcm_substream *ss)
{
	struct snd_soc_pcm_runtime *runtime = ss->private_data;
	struct snd_soc_platform *platform = runtime->platform;
	struct wasp_pcm_pltfm_priv *prdata = snd_soc_platform_get_drvdata(platform);
	struct wasp_pcm_rt_priv *rtpriv;

	if (!prdata)
		return 0;

	if (ss->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		prdata->playback = NULL;
	} else {
		prdata->capture = NULL;
	}

	if (!prdata->playback && !prdata->capture) {
		free_irq(ATH79_MISC_IRQ_DMA, prdata);
		kfree(prdata);
		snd_soc_platform_set_drvdata(platform, NULL);
	}
	rtpriv = ss->runtime->private_data;
	kfree(rtpriv);

	return 0;
}

static int wasp_pcm_hw_params(struct snd_pcm_substream *ss,
			      struct snd_pcm_hw_params *hw_params)
{
	struct snd_pcm_runtime *runtime = ss->runtime;
	struct wasp_pcm_desc *desc, *prev;
	struct wasp_pcm_rt_priv *rtpriv;
	dma_addr_t desc_p;
	unsigned int offset = 0;

	rtpriv = runtime->private_data;

	spin_lock(&wasp_pcm_lock);

	/* We loop until we have enough buffers to map the whole DMA area */
	do {
		/* Allocate a descriptor and insert it into the DMA ring */
		desc = dma_pool_alloc(wasp_pcm_cache, GFP_KERNEL, &desc_p);
		desc->phys = desc_p;
		list_add_tail(&desc->list, &rtpriv->dma_head);
		/* OWN=0 --> For now, set ownership to CPU. The ownership will
		 * be given to DMA controller when ready for xfer */
		desc->OWN = desc->EOM = 0;
		desc->rsvd1 = desc->rsvd2 = desc->rsvd3 = 0;

		/* The manual says the buffer size is not necessarily a multiple
		 * of the period size. We handle this case though I'm not sure
		 * how often it will happen in real life */
		if (params_buffer_bytes(hw_params) >=
		    offset + params_period_bytes(hw_params)) {
			desc->size = params_period_bytes(hw_params);
		} else {
			desc->size = params_buffer_bytes(hw_params) - offset;
		}
		desc->BufPtr = ss->dma_buffer.addr + offset;

		/* For now, we assume the buffer is always full
		 * -->length == size */
		desc->length = desc->size;

		/* We need to make sure we are not the first descriptor.
		 * If we are, prev doesn't point to a struct wasp_pcm_desc */
		if (desc->list.prev != &rtpriv->dma_head) {
			prev =
			    list_entry(desc->list.prev, struct wasp_pcm_desc,
				       list);
			prev->NextPtr = desc->phys;
		}

		offset += desc->size;
	} while (offset < params_buffer_bytes(hw_params));

	/* Once all the descriptors have been created, we can close the ring
	 * by pointing from the last one to the first one */
	desc = list_first_entry(&rtpriv->dma_head, struct wasp_pcm_desc, list);
	prev = list_entry(rtpriv->dma_head.prev, struct wasp_pcm_desc, list);
	prev->NextPtr = desc->phys;

	spin_unlock(&wasp_pcm_lock);

	snd_pcm_set_runtime_buffer(ss, &ss->dma_buffer);
	runtime->dma_bytes = params_buffer_bytes(hw_params);

	return 1;
}

static int wasp_pcm_hw_free(struct snd_pcm_substream *ss)
{
	struct snd_pcm_runtime *runtime = ss->runtime;
	struct wasp_pcm_desc *desc, *n;
	struct wasp_pcm_rt_priv *rtpriv;

	rtpriv = runtime->private_data;

	spin_lock(&wasp_pcm_lock);
	list_for_each_entry_safe(desc, n, &rtpriv->dma_head, list) {
		list_del(&desc->list);
		printk(KERN_NOTICE "Freeing desc at @%08x\n", (u32) desc);
		dma_pool_free(wasp_pcm_cache, desc, desc->phys);
	}
	spin_unlock(&wasp_pcm_lock);

	snd_pcm_set_runtime_buffer(ss, NULL);
	return 0;
}

static int wasp_pcm_prepare(struct snd_pcm_substream *ss)
{
	struct snd_pcm_runtime *runtime = ss->runtime;
	struct wasp_pcm_rt_priv *rtpriv;
	struct wasp_pcm_desc *desc;

	rtpriv = runtime->private_data;

	printk(KERN_NOTICE "%s called\n", __FUNCTION__);
	printk(KERN_NOTICE "%s:Stream parameters\n", __FUNCTION__);
	printk(KERN_NOTICE "%s:  rate=     %d\n", __FUNCTION__, runtime->rate);
	printk(KERN_NOTICE "%s:  format=   %d\n", __FUNCTION__, (int)runtime->format);
	printk(KERN_NOTICE "%s:  channels= %d\n", __FUNCTION__, runtime->channels);
	printk(KERN_NOTICE "%s:  dma_area= %08x\n", __FUNCTION__, (u32)runtime->dma_area);
	printk(KERN_NOTICE "%s:  period_size= %08x\n", __FUNCTION__, (u32)runtime->period_size);
	printk(KERN_NOTICE "%s:  periods= %08x\n", __FUNCTION__, (u32)runtime->periods);

	/* Request the DMA channel to the controller */
	ath79_dma_wr(AR934X_DMA_REG_MBOX_DMA_POLICY,
		     AR934X_DMA_MBOX_DMA_POLICY_RX_QUANTUM |
		     (6 << AR934X_DMA_MBOX_DMA_POLICY_TX_FIFO_THRESH_SHIFT));

	/* Give ownership of the descriptors to the DMA engine */
	spin_lock(&wasp_pcm_lock);
	list_for_each_entry(desc, &rtpriv->dma_head, list) {
		desc->OWN = 1;
		printk(KERN_NOTICE
		       "Desc@=%08x OWN=%d EOM=%d Length=%d Size=%d\nBuf=%08x Next=%08x\n",
		       (u32) desc, (u32) desc->OWN, (u32) desc->EOM, (u32) desc->length, desc->size,
		       (u32) desc->BufPtr, (u32) desc->NextPtr);
	}
	spin_unlock(&wasp_pcm_lock);

	/* Setup the PLLs for the requested frequencies */
	ath79_audio_set_freq(runtime->rate);

	/* The direction is indicated from the DMA engine perspective
	 * i.e. we'll be using the RX registers for Playback and
	 * the TX registers for capture */
	desc = list_first_entry(&rtpriv->dma_head, struct wasp_pcm_desc, list);
	ath79_dma_wr(AR934X_DMA_REG_MBOX0_DMA_RX_DESCRIPTOR_BASE,
		     (u32) desc->phys);
	ath79_mbox_set_interrupt(AR934X_DMA_MBOX0_INT_RX_COMPLETE);

	/* Reset the DMA MBOX controller */
	ath79_dma_wr(AR934X_DMA_REG_MBOX_FIFO_RESET,0xff);

	return 0;
}

static int wasp_pcm_trigger(struct snd_pcm_substream *ss, int cmd)
{
	struct wasp_pcm_rt_priv *rtpriv = ss->runtime->private_data;
	struct wasp_pcm_desc *desc;

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		/* Here start the PCM engine */
		printk(KERN_NOTICE
		       "%s called - cmd=SNDRV_PCM_TRIGGER_START\n",
		       __FUNCTION__);
		/* As indicated above, RX is used for Playback here */
		ath79_dma_wr(AR934X_DMA_REG_MBOX0_DMA_RX_CONTROL,
			     AR934X_DMA_MBOX_DMA_CONTROL_START);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
		/* Here stop the PCM engine */
		list_for_each_entry(desc, &rtpriv->dma_head, list) {
			desc->EOM = 1;
		}
		wasp_pcm_clear_own_bit(rtpriv);
		ath79_dma_wr(AR934X_DMA_REG_MBOX0_DMA_RX_CONTROL,
			     AR934X_DMA_MBOX_DMA_CONTROL_STOP);
		mdelay(100);
		printk(KERN_NOTICE
		       "%s called - cmd=SNDRV_PCM_TRIGGER_STOP\n",
		       __FUNCTION__);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static snd_pcm_uframes_t wasp_pcm_pointer(struct snd_pcm_substream *ss)
{
	struct snd_pcm_runtime *runtime = ss->runtime;
	struct wasp_pcm_rt_priv *rtpriv;
	snd_pcm_uframes_t ret = 0;

	rtpriv = runtime->private_data;

	if(rtpriv->last_played == NULL)
		ret = 0;
	else
		ret = rtpriv->last_played->BufPtr - runtime->dma_addr;

	return bytes_to_frames(runtime, ret);
}

static int wasp_pcm_mmap(struct snd_pcm_substream *ss, struct vm_area_struct *vma)
{
	return remap_pfn_range(vma, vma->vm_start,
			ss->dma_buffer.addr >> PAGE_SHIFT,
			vma->vm_end - vma->vm_start, vma->vm_page_prot);
}

static struct snd_pcm_ops wasp_pcm_ops = {
	.open		= wasp_pcm_open,
	.close		= wasp_pcm_close,
	.ioctl		= snd_pcm_lib_ioctl,
	.hw_params	= wasp_pcm_hw_params,
	.hw_free	= wasp_pcm_hw_free,
	.prepare	= wasp_pcm_prepare,
	.trigger	= wasp_pcm_trigger,
	.pointer	= wasp_pcm_pointer,
	.mmap		= wasp_pcm_mmap,
};

static void wasp_pcm_free_dma_buffers(struct snd_pcm *pcm)
{
	struct snd_pcm_substream *ss;
	struct snd_dma_buffer *buf;
	int stream;
	int i;

	for (stream = 0; stream < 2; stream++) {
		ss = pcm->streams[stream].substream;
		if (!ss)
			continue;
		buf = &ss->dma_buffer;
		if (!buf->area)
			continue;
		dma_free_coherent(NULL, buf->bytes,
				      buf->area, buf->addr);
		buf->area = NULL;
	}

	dma_pool_destroy(wasp_pcm_cache);
}

static int wasp_pcm_preallocate_dma_buffer(struct snd_pcm *pcm, int stream)
{
	struct snd_pcm_substream *ss = pcm->streams[stream].substream;
	struct snd_dma_buffer *buf = &ss->dma_buffer;

	printk(KERN_NOTICE "%s: allocate %8s stream\n", __FUNCTION__,
		stream == SNDRV_PCM_STREAM_CAPTURE ? "capture" : "playback" );

	buf->dev.type = SNDRV_DMA_TYPE_DEV;
	buf->dev.dev = pcm->card->dev;
	buf->private_data = NULL;
	buf->bytes = wasp_pcm_hardware.buffer_bytes_max;

	buf->area = dma_alloc_coherent(NULL, buf->bytes,
					   &buf->addr, GFP_DMA);
	if (!buf->area)
		return -ENOMEM;

	printk(KERN_NOTICE "%s: 0x%xB allocated at 0x%08x\n",
		__FUNCTION__, buf->bytes, (u32) buf->area);

	return 0;
}

static u64 wasp_pcm_dmamask = 0xffffffff;

static int wasp_soc_pcm_new(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_card *card = rtd->card->snd_card;
	struct snd_pcm *pcm = rtd->pcm;
	int ret = 0;

	if (!card->dev->dma_mask)
		card->dev->dma_mask = &wasp_pcm_dmamask;
	if (!card->dev->coherent_dma_mask)
		card->dev->coherent_dma_mask = DMA_BIT_MASK(32);

	if (pcm->streams[SNDRV_PCM_STREAM_PLAYBACK].substream) {
		ret = wasp_pcm_preallocate_dma_buffer(pcm,
			SNDRV_PCM_STREAM_PLAYBACK);
		if (ret)
			goto out;
	}

	if (pcm->streams[SNDRV_PCM_STREAM_CAPTURE].substream) {
		ret = wasp_pcm_preallocate_dma_buffer(pcm,
			SNDRV_PCM_STREAM_CAPTURE);
		if (ret)
			goto out;
	}

	/* Allocate a DMA pool to store the MBOX descriptor */
	wasp_pcm_cache = dma_pool_create("wasp_pcm_pool", rtd->platform->dev,
					 sizeof(struct wasp_pcm_desc), 4, 0);
	if (!wasp_pcm_cache)
		ret = -ENOMEM;

out:
	return ret;
}

struct snd_soc_platform_driver wasp_soc_platform = {
	.ops		= &wasp_pcm_ops,
	.pcm_new	= wasp_soc_pcm_new,
	.pcm_free	= wasp_pcm_free_dma_buffers,
};
EXPORT_SYMBOL_GPL(wasp_soc_platform);

static int __devinit wasp_soc_platform_probe(struct platform_device *pdev)
{
	return snd_soc_register_platform(&pdev->dev, &wasp_soc_platform);
}

static int __devexit wasp_soc_platform_remove(struct platform_device *pdev)
{
	snd_soc_unregister_platform(&pdev->dev);
	return 0;
}

static struct platform_driver wasp_pcm_driver = {
	.driver = {
			.name = "wasp-pcm-audio",
			.owner = THIS_MODULE,
	},

	.probe = wasp_soc_platform_probe,
	.remove = __devexit_p(wasp_soc_platform_remove),
};

static int __init wasp_soc_platform_init(void)
{
	return platform_driver_register(&wasp_pcm_driver);
}
module_init(wasp_soc_platform_init);

static void __exit wasp_soc_platform_exit(void)
{
	platform_driver_unregister(&wasp_pcm_driver);
}
module_exit(wasp_soc_platform_exit);

MODULE_AUTHOR("Qualcomm-Atheros");
MODULE_DESCRIPTION("QCA Audio PCM DMA module");
MODULE_LICENSE("Dual BSD/GPL");
