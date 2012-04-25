/*
 * qca-soundlib.h -- ALSA PCM interface for the QCA Wasp based audio interface
 *
 * Author:	Mathieu Olivari
 * Created:	Jan 22, 2011
 * Copyright:	(C) 2011 Qualcomm Atheros, Inc.
 *
 */

int wasp_set_sample_format (struct snd_pcm_format_t format);

int wasp_set_sample_rate (unsigned int rate);
