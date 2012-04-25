/*
 * qca-soundlib.c -- QCA Wasp specific audio function
 *
 * Author:	Mathieu Olivari
 * Created:	Jan 22, 2011
 * Copyright:	(C) 2011 Qualcomm Atheros, Inc.
 *
 */

#include <sound/core.h>
#include <sound/soc.h>

int wasp_set_sample_format (struct snd_pcm_format_t format)
{
	switch (format) {
	case SNDRV_PCM_FORMAT_S8:
	case SNDRV_PCM_FORMAT_S16_LE:
	case SNDRV_PCM_FORMAT_S16_BE:
	case SNDRV_PCM_FORMAT_S24_LE:
	case SNDRV_PCM_FORMAT_S24_BE:
	case SNDRV_PCM_FORMAT_S32_LE:
	case SNDRV_PCM_FORMAT_S32_BE:
	default:
		return -EOPNOTSUPP;
	return 0;
}

int wasp_set_sample_rate (unsigned int rate)
{
	switch (rate) {
	case 32000:
		break;
	case 44100:
		break;
	case 48000:
		break;
	case 96000:
		break;
	default:
		return -EOPNOTSUPP;

	return 0;
}
