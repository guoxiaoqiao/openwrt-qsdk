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
#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>

#include <sound/core.h>
#include <sound/soc.h>

#include "wasp-pcm.h"
#include "ath_i2s.h"
#if 0
#include "atheros.h"
#include "934x.h"
#endif

#define ATH_I2S_NUM_DESC 160
#define ATH_I2S_BUFF_SIZE 24

static struct snd_pcm_hardware wasp_pcm_hardware = {
	.info			= SNDRV_PCM_INFO_MMAP |
				  SNDRV_PCM_INFO_MMAP_VALID |
				  SNDRV_PCM_INFO_INTERLEAVED,
	.formats		= SNDRV_PCM_FMTBIT_S8 |
				  SNDRV_PCM_FMTBIT_S16_LE |
				  SNDRV_PCM_FMTBIT_S16_BE |
				  SNDRV_PCM_FMTBIT_S24_LE |
				  SNDRV_PCM_FMTBIT_S24_BE |
				  SNDRV_PCM_FMTBIT_S32_LE |
				  SNDRV_PCM_FMTBIT_S32_BE,
	.rates			= SNDRV_PCM_RATE_32000 |
				  SNDRV_PCM_RATE_44100 |
				  SNDRV_PCM_RATE_48000 |
				  SNDRV_PCM_RATE_96000,
	.rate_min		= 32000,
	.rate_max		= 96000,
	.channels_min		= 2,
	.channels_max		= 2,
	/* For now, let's just use the same number of buffer as in the original driver
	 * In the long run, we'll need to optimize these parameters to minimize the risk
	 * of underrun and find the best ISR frequency trade-off
	 */
	.buffer_bytes_max	= ATH_I2S_NUM_DESC * ATH_I2S_BUFF_SIZE,
	.period_bytes_min	= ATH_I2S_BUFF_SIZE,
	.period_bytes_max	= ATH_I2S_NUM_DESC * ATH_I2S_BUFF_SIZE,
	.periods_min		= 1,
	.periods_max		= PAGE_SIZE/sizeof(ath_mbox_dma_desc),
};

/* Non-standard function: wasp_pcm_init_desc()
 *   desc:	virt@ of the pointer to the descriptor to intialize
 *   databuff:	virt@ of the beginning of the audio data buffer area
 */
static int wasp_pcm_init_desc(ath_mbox_dma_desc *desc, unsigned char *databuff)
{
	/* OWN=0 --> For init, set ownership to CPU */
	desc->OWN	= 0;
	desc->EOM	= 0;
	desc->rsvd1	= 0;
	desc->size	= ATH_I2S_BUFF_SIZE;
	desc->length	= 0;
	desc->rsvd2	= 0;
	desc->BufPtr	= virt_to_phys(databuff);
	desc->rsvd3	= 0;
	desc->NextPtr	= virt_to_phys(desc) + sizeof(ath_mbox_dma_desc);

	printk(KERN_CRIT "New desc init:\n");
	printk(KERN_CRIT "BufPtr=%08x  NextPtr=%08x\n", desc->BufPtr, desc->NextPtr);

	return 0;
}

static int wasp_pcm_open (struct snd_pcm_substream *ss)
{
	struct snd_pcm_runtime *runtime = ss->runtime;

	int count;
	i2s_dma_buf_t *dmabuf;

	printk(KERN_CRIT "%s called\n", __FUNCTION__);

	snd_soc_set_runtime_hwparams(ss, &wasp_pcm_hardware);
	runtime->private_data = &sc_buf_var;

	dmabuf = &sc_buf_var.sc_pbuf;

	/* Allocate the MBOX DMA buffers */
	dmabuf->db_desc = (ath_mbox_dma_desc *)	dma_alloc_coherent(
				NULL,
				ATH_I2S_NUM_DESC * sizeof(ath_mbox_dma_desc),
				&dmabuf->db_desc_p, GFP_DMA);

	if (dmabuf->db_desc == NULL) {
		printk(KERN_CRIT "%s: DMA allocation failed\n", __FUNCTION__);
		return -ENOMEM;
	}

	for(count=0; count < ATH_I2S_NUM_DESC; count++) {
		/* Initializing the descriptor */
		wasp_pcm_init_desc(dmabuf->db_desc + (count), ss->dma_buffer.area + (count*ATH_I2S_BUFF_SIZE));
	}

	/* Reset the DMA MBOX controller */
	ath79_dma_wr(AR934X_DMA_MBOX_FIFO_RESET, 0xff);

	/* Request the DMA channel to the controller */
	ath79_dma_wr(AR934X_DMA_SLIC_MBOX_DMA_POLICY,
		AR934X_DMA_MBOX_DMA_POLICY_RX_QUANTUM |
		AR934X_DMA_MBOX_DMA_POLICY_TX_FIFO_THRESH(6)
	);
	/* Flush these regs by reading them */
	ath79_dma_rr(AR934X_DMA_MBOX_FIFO_RESET);
	ath79_dma_rr(AR934X_DMA_SLIC_MBOX_DMA_POLICY);

	return 0;
}

static int wasp_pcm_close (struct snd_pcm_substream *ss)
{
	struct snd_pcm_runtime *runtime = ss->runtime;
	ath_i2s_softc_t *sc = (ath_i2s_softc_t *) &runtime->private_data;
	i2s_dma_buf_t *dmabuf = &sc->sc_pbuf;

	printk(KERN_CRIT "%s called\n", __FUNCTION__);

	dma_free_coherent(NULL,
		ATH_I2S_NUM_DESC * sizeof(ath_mbox_dma_desc),
		dmabuf->db_desc,
		GFP_DMA);

	return 0;
}

static int wasp_pcm_hw_params (struct snd_pcm_substream *ss, struct snd_pcm_hw_params *hw_params)
{
	printk(KERN_CRIT "%s called\n", __FUNCTION__);

	return snd_pcm_lib_malloc_pages(ss, params_buffer_bytes(hw_params));
}

static int wasp_pcm_hw_free (struct snd_pcm_substream *ss)
{
	printk(KERN_CRIT "%s called\n", __FUNCTION__);

	return snd_pcm_lib_free_pages(ss);
}

static int wasp_pcm_prepare (struct snd_pcm_substream *ss)
{
	i2s_dma_buf_t *dmabuf;

	dmabuf = &sc_buf_var.sc_pbuf;

	printk(KERN_CRIT "%s called\n", __FUNCTION__);
	ath79_dma_wr(AR934X_DMA_MBOX0_DMA_TX_DESCRIPTOR_BASE, (u32) sc_buf_var.sc_pbuf.db_desc);

	return 0;
}

static int wasp_pcm_trigger (struct snd_pcm_substream *ss, int cmd)
{
	switch(cmd) {
	case SNDRV_PCM_TRIGGER_START:
		/* Here start the PCM engine */
		printk(KERN_CRIT "%s called - cmd=SNDRV_PCM_TRIGGER_START\n", __FUNCTION__);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
		/* Here stop the PCM engine */
		printk(KERN_CRIT "%s called - cmd=SNDRV_PCM_TRIGGER_STOP\n", __FUNCTION__);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static snd_pcm_uframes_t wasp_pcm_pointer (struct snd_pcm_substream *ss)
{
	printk(KERN_CRIT "%s called\n", __FUNCTION__);
	return 0;
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
};

static int wasp_pcm_preallocate_dma_buffer(struct snd_pcm *pcm, int stream)
{
	struct snd_pcm_substream *ss = pcm->streams[stream].substream;
	struct snd_dma_buffer *buf = &ss->dma_buffer;
	size_t size = wasp_pcm_hardware.buffer_bytes_max;
	printk(KERN_CRIT "%s called\n", __FUNCTION__);
	buf->dev.type = SNDRV_DMA_TYPE_DEV;
	buf->dev.dev = pcm->card->dev;
	buf->private_data = NULL;
	buf->area = dma_alloc_coherent(pcm->card->dev, size,
					   &buf->addr, GFP_KERNEL);
	if (!buf->area)
		return -ENOMEM;
	buf->bytes = size;
	return 0;
}

static void wasp_pcm_free_dma_buffers(struct snd_pcm *pcm)
{
	struct snd_pcm_substream *ss;
	struct snd_dma_buffer *buf;
	int stream;

	printk(KERN_CRIT "%s called\n", __FUNCTION__);
	for (stream = 0; stream < 2; stream++) {
		ss = pcm->streams[stream].substream;
		if (!ss)
			continue;
		buf = &ss->dma_buffer;
		if (!buf->area)
			continue;
		dma_free_coherent(pcm->card->dev, buf->bytes,
				      buf->area, buf->addr);
		buf->area = NULL;
	}
}

static u64 wasp_pcm_dmamask = 0xffffffff;

static int wasp_soc_pcm_new(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_card *card = rtd->card->snd_card;
	struct snd_pcm *pcm = rtd->pcm;
	int ret = 0;

	printk(KERN_CRIT "%s called\n", __FUNCTION__);
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
	printk(KERN_CRIT "%s called\n", __FUNCTION__);
	return snd_soc_register_platform(&pdev->dev, &wasp_soc_platform);
}

static int __devexit wasp_soc_platform_remove(struct platform_device *pdev)
{
	printk(KERN_CRIT "%s called\n", __FUNCTION__);
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
	printk(KERN_CRIT "%s called\n", __FUNCTION__);
	return platform_driver_register(&wasp_pcm_driver);
}
module_init(wasp_soc_platform_init);

static void __exit wasp_soc_platform_exit(void)
{
	printk(KERN_CRIT "%s called\n", __FUNCTION__);
	platform_driver_unregister(&wasp_pcm_driver);
}
module_exit(wasp_soc_platform_exit);

MODULE_AUTHOR("Qualcomm-Atheros");
MODULE_DESCRIPTION("QCA Audio PCM DMA module");
MODULE_LICENSE("Dual BSD/GPL");
