/*
 *  Atheros AR71xx Audio device support
 *
 *  Copyright (C) 2012 Qualcomm-Atheros Inc.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _ATH79_DEV_AUDIO_H
#define _ATH79_DEV_AUDIO_H

void __init ath79_audio_device_register(void);

void ath79_audio_init(void);

void ath79_stereo_reset_set(void);
void ath79_stereo_reset_clear(void);

int ath79_audio_set_freq(int freq);

#endif /* _ATH79_DEV_DSA_H */
