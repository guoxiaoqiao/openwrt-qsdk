/*
 * ath-pcm.c -- ALSA PCM interface for the QCA Wasp based audio interface
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

#include "ath79-pcm.h"

#define ATH_I2S_NUM_DESC 160
#define ATH_I2S_BUFF_SIZE 24

static struct dma_pool *ath79_pcm_cache;
static spinlock_t ath79_pcm_lock;

static struct snd_pcm_hardware ath79_pcm_hardware = {
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
	.periods_max = PAGE_SIZE/sizeof(struct ath79_pcm_desc),
	.fifo_size = 0,
};

static inline struct ath79_pcm_desc *ath79_pcm_get_last_played(struct ath79_pcm_rt_priv *rtpriv)
{
	struct ath79_pcm_desc *desc, *prev;

	spin_lock(&ath79_pcm_lock);
	prev = list_entry(rtpriv->dma_head.prev, struct ath79_pcm_desc, list);
	list_for_each_entry(desc, &rtpriv->dma_head, list) {
		if (desc->OWN == 1 && prev->OWN == 0) {
			return desc;
		}
		prev = desc;
	}
	spin_unlock(&ath79_pcm_lock);

	/* If we didn't find the last played buffer, return NULL */
	return NULL;
}

static inline void ath79_pcm_set_own_bit(struct ath79_pcm_rt_priv *rtpriv)
{
	struct ath79_pcm_desc *desc;

	spin_lock(&ath79_pcm_lock);
	list_for_each_entry(desc, &rtpriv->dma_head, list) {
		if (desc->OWN == 0) {
			desc->OWN = 1;
		}
	}
	spin_unlock(&ath79_pcm_lock);
}

static inline void ath79_pcm_clear_own_bit(struct ath79_pcm_rt_priv *rtpriv)
{
	struct ath79_pcm_desc *desc;

	spin_lock(&ath79_pcm_lock);
	list_for_each_entry(desc, &rtpriv->dma_head, list) {
		if (desc->OWN == 1) {
			desc->OWN = 0;
		}
	}
	spin_unlock(&ath79_pcm_lock);
}

static irqreturn_t ath79_pcm_interrupt(int irq, void *dev_id)
{
	uint32_t status;
	struct ath79_pcm_pltfm_priv *prdata = dev_id;
	struct ath79_pcm_rt_priv *rtpriv;

	status = ath79_dma_rr(AR934X_DMA_REG_MBOX_INT_STATUS);

	/* Notify the corresponding ALSA substream */
	if(status & AR934X_DMA_MBOX_INT_STATUS_RX_DMA_COMPLETE) {
		rtpriv = prdata->playback->runtime->private_data;
		/* Store the last played buffer in the runtime priv struct */
		rtpriv->last_played = ath79_pcm_get_last_played(rtpriv);
		ath79_pcm_set_own_bit(rtpriv);
		if (rtpriv->last_played == NULL) {
			snd_printd("BUG: ISR called but no played buf found\n");
			goto ack;
		}
		snd_pcm_period_elapsed(prdata->playback);
	}
	if(status & AR934X_DMA_MBOX_INT_STATUS_TX_DMA_COMPLETE) {
		rtpriv = prdata->capture->runtime->private_data;
		/* Store the last played buffer in the runtime priv struct */
		rtpriv->last_played = ath79_pcm_get_last_played(rtpriv);
		ath79_pcm_set_own_bit(rtpriv);
		if (rtpriv->last_played == NULL) {
			snd_printd("BUG: ISR called but no rec buf found\n");
			goto ack;
		}
		snd_pcm_period_elapsed(prdata->capture);
	}

ack:
	/* Ack the interrupt */
	ath79_dma_wr(AR934X_DMA_REG_MBOX_INT_STATUS, status);

	return IRQ_HANDLED;
}

static int ath79_pcm_open(struct snd_pcm_substream *ss)
{
	struct snd_soc_pcm_runtime *runtime = ss->private_data;
	struct snd_soc_platform *platform = runtime->platform;
	struct ath79_pcm_pltfm_priv *prdata = snd_soc_platform_get_drvdata(platform);
	struct ath79_pcm_rt_priv *rtpriv;
	int err;

	if (prdata == NULL) {
		prdata = kzalloc(sizeof(struct ath79_pcm_pltfm_priv), GFP_KERNEL);
		if (prdata == NULL)
			return -ENOMEM;

		err = request_irq(ATH79_MISC_IRQ_DMA, ath79_pcm_interrupt, 0,
				  "ath79-pcm", prdata);
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
	snd_printd("%s: 0x%xB allocated at 0x%08x\n",
	       __FUNCTION__, sizeof(*rtpriv), (u32) rtpriv);

	ss->runtime->private_data = rtpriv;
	rtpriv->last_played = NULL;
	INIT_LIST_HEAD(&rtpriv->dma_head);

	snd_soc_set_runtime_hwparams(ss, &ath79_pcm_hardware);

	return 0;
}

static int ath79_pcm_close(struct snd_pcm_substream *ss)
{
	struct snd_soc_pcm_runtime *runtime = ss->private_data;
	struct snd_soc_platform *platform = runtime->platform;
	struct ath79_pcm_pltfm_priv *prdata = snd_soc_platform_get_drvdata(platform);
	struct ath79_pcm_rt_priv *rtpriv;

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

static int ath79_pcm_init_desc(struct list_head *head, dma_addr_t baseaddr,
			      int period_bytes,int bufsize)
{
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
		/* OWN=0 --> For now, set ownership to CPU. The ownership will
		 * be given to DMA controller when ready for xfer */
		desc->OWN = desc->EOM = 0;
		desc->rsvd1 = desc->rsvd2 = desc->rsvd3 = 0;

		/* The manual says the buffer size is not necessarily a multiple
		 * of the period size. We handle this case though I'm not sure
		 * how often it will happen in real life */
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

	spin_unlock(&ath79_pcm_lock);

	return 0;
}

static int ath79_pcm_hw_params(struct snd_pcm_substream *ss,
			      struct snd_pcm_hw_params *hw_params)
{
	struct snd_pcm_runtime *runtime = ss->runtime;
	struct ath79_pcm_rt_priv *rtpriv;
	int ret;

	rtpriv = runtime->private_data;

	ret = ath79_pcm_init_desc(&rtpriv->dma_head, ss->dma_buffer.addr,
		params_period_bytes(hw_params), params_buffer_bytes(hw_params));

	snd_pcm_set_runtime_buffer(ss, &ss->dma_buffer);
	runtime->dma_bytes = params_buffer_bytes(hw_params);

	return 1;
}

static int ath79_pcm_hw_free(struct snd_pcm_substream *ss)
{
	struct snd_pcm_runtime *runtime = ss->runtime;
	struct ath79_pcm_desc *desc, *n;
	struct ath79_pcm_rt_priv *rtpriv;

	rtpriv = runtime->private_data;

	spin_lock(&ath79_pcm_lock);
	list_for_each_entry_safe(desc, n, &rtpriv->dma_head, list) {
		list_del(&desc->list);
		dma_pool_free(ath79_pcm_cache, desc, desc->phys);
	}
	spin_unlock(&ath79_pcm_lock);

	snd_pcm_set_runtime_buffer(ss, NULL);
	return 0;
}

static int ath79_pcm_prepare(struct snd_pcm_substream *ss)
{
	struct snd_pcm_runtime *runtime = ss->runtime;
	struct ath79_pcm_rt_priv *rtpriv;
	struct ath79_pcm_desc *desc;

	rtpriv = runtime->private_data;

	/* Request the DMA channel to the controller */
	ath79_dma_wr(AR934X_DMA_REG_MBOX_DMA_POLICY,
		     AR934X_DMA_MBOX_DMA_POLICY_RX_QUANTUM |
		     (6 << AR934X_DMA_MBOX_DMA_POLICY_TX_FIFO_THRESH_SHIFT));

	/* Give ownership of the descriptors to the DMA engine */
	ath79_pcm_set_own_bit(rtpriv);

	/* Setup the PLLs for the requested frequencies */
	ath79_audio_set_freq(runtime->rate);

	/* The direction is indicated from the DMA engine perspective
	 * i.e. we'll be using the RX registers for Playback and
	 * the TX registers for capture */
	desc = list_first_entry(&rtpriv->dma_head, struct ath79_pcm_desc, list);
	ath79_dma_wr(AR934X_DMA_REG_MBOX0_DMA_RX_DESCRIPTOR_BASE,
		     (u32) desc->phys);
	ath79_mbox_set_interrupt(AR934X_DMA_MBOX0_INT_RX_COMPLETE);

	/* Reset the DMA MBOX controller */
	ath79_dma_wr(AR934X_DMA_REG_MBOX_FIFO_RESET,0xff);

	return 0;
}

static int ath79_pcm_trigger(struct snd_pcm_substream *ss, int cmd)
{
	struct ath79_pcm_rt_priv *rtpriv = ss->runtime->private_data;
	struct ath79_pcm_desc *desc;

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
		ath79_pcm_clear_own_bit(rtpriv);
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

static snd_pcm_uframes_t ath79_pcm_pointer(struct snd_pcm_substream *ss)
{
	struct snd_pcm_runtime *runtime = ss->runtime;
	struct ath79_pcm_rt_priv *rtpriv;
	snd_pcm_uframes_t ret = 0;

	rtpriv = runtime->private_data;

	if(rtpriv->last_played == NULL)
		ret = 0;
	else
		ret = rtpriv->last_played->BufPtr - runtime->dma_addr;

	return bytes_to_frames(runtime, ret);
}

static int ath79_pcm_mmap(struct snd_pcm_substream *ss, struct vm_area_struct *vma)
{
	return remap_pfn_range(vma, vma->vm_start,
			ss->dma_buffer.addr >> PAGE_SHIFT,
			vma->vm_end - vma->vm_start, vma->vm_page_prot);
}

static struct snd_pcm_ops ath79_pcm_ops = {
	.open		= ath79_pcm_open,
	.close		= ath79_pcm_close,
	.ioctl		= snd_pcm_lib_ioctl,
	.hw_params	= ath79_pcm_hw_params,
	.hw_free	= ath79_pcm_hw_free,
	.prepare	= ath79_pcm_prepare,
	.trigger	= ath79_pcm_trigger,
	.pointer	= ath79_pcm_pointer,
	.mmap		= ath79_pcm_mmap,
};

static void ath79_pcm_free_dma_buffers(struct snd_pcm *pcm)
{
	struct snd_pcm_substream *ss;
	struct snd_dma_buffer *buf;
	int stream;

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

	dma_pool_destroy(ath79_pcm_cache);
}

static int ath79_pcm_preallocate_dma_buffer(struct snd_pcm *pcm, int stream)
{
	struct snd_pcm_substream *ss = pcm->streams[stream].substream;
	struct snd_dma_buffer *buf = &ss->dma_buffer;

	printk(KERN_NOTICE "%s: allocate %8s stream\n", __FUNCTION__,
		stream == SNDRV_PCM_STREAM_CAPTURE ? "capture" : "playback" );

	buf->dev.type = SNDRV_DMA_TYPE_DEV;
	buf->dev.dev = pcm->card->dev;
	buf->private_data = NULL;
	buf->bytes = ath79_pcm_hardware.buffer_bytes_max;

	buf->area = dma_alloc_coherent(NULL, buf->bytes,
					   &buf->addr, GFP_DMA);
	if (!buf->area)
		return -ENOMEM;

	printk(KERN_NOTICE "%s: 0x%xB allocated at 0x%08x\n",
		__FUNCTION__, buf->bytes, (u32) buf->area);

	return 0;
}

static u64 ath79_pcm_dmamask = 0xffffffff;

static int ath79_soc_pcm_new(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_card *card = rtd->card->snd_card;
	struct snd_pcm *pcm = rtd->pcm;
	int ret = 0;

	if (!card->dev->dma_mask)
		card->dev->dma_mask = &ath79_pcm_dmamask;
	if (!card->dev->coherent_dma_mask)
		card->dev->coherent_dma_mask = DMA_BIT_MASK(32);

	if (pcm->streams[SNDRV_PCM_STREAM_PLAYBACK].substream) {
		ret = ath79_pcm_preallocate_dma_buffer(pcm,
			SNDRV_PCM_STREAM_PLAYBACK);
		if (ret)
			goto out;
	}

	if (pcm->streams[SNDRV_PCM_STREAM_CAPTURE].substream) {
		ret = ath79_pcm_preallocate_dma_buffer(pcm,
			SNDRV_PCM_STREAM_CAPTURE);
		if (ret)
			goto out;
	}

	/* Allocate a DMA pool to store the MBOX descriptor */
	ath79_pcm_cache = dma_pool_create("ath79_pcm_pool", rtd->platform->dev,
					 sizeof(struct ath79_pcm_desc), 4, 0);
	if (!ath79_pcm_cache)
		ret = -ENOMEM;

out:
	return ret;
}

struct snd_soc_platform_driver ath79_soc_platform = {
	.ops		= &ath79_pcm_ops,
	.pcm_new	= ath79_soc_pcm_new,
	.pcm_free	= ath79_pcm_free_dma_buffers,
};
EXPORT_SYMBOL_GPL(ath79_soc_platform);

static int __devinit ath79_soc_platform_probe(struct platform_device *pdev)
{
	return snd_soc_register_platform(&pdev->dev, &ath79_soc_platform);
}

static int __devexit ath79_soc_platform_remove(struct platform_device *pdev)
{
	snd_soc_unregister_platform(&pdev->dev);
	return 0;
}

static struct platform_driver ath79_pcm_driver = {
	.driver = {
			.name = "ath79-pcm-audio",
			.owner = THIS_MODULE,
	},

	.probe = ath79_soc_platform_probe,
	.remove = __devexit_p(ath79_soc_platform_remove),
};

static int __init ath79_soc_platform_init(void)
{
	return platform_driver_register(&ath79_pcm_driver);
}
module_init(ath79_soc_platform_init);

static void __exit ath79_soc_platform_exit(void)
{
	platform_driver_unregister(&ath79_pcm_driver);
}
module_exit(ath79_soc_platform_exit);

MODULE_AUTHOR("Qualcomm-Atheros");
MODULE_DESCRIPTION("QCA Audio PCM DMA module");
MODULE_LICENSE("Dual BSD/GPL");
