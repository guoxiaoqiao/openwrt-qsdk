/*
 *  Atheros AR71XX/AR724X/AR913X Calibration data in NAND flash fixup
 *
 *  Copyright (C) 2012 Qualcomm Atheros Inc.
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* 0 is a valid value for an MTD offset. So we'll use -1 as an undefined value
 * That means that all the fields in the structure below have to be initialized
 * in the platform code when the fixup is used */
#define FIXUP_UNDEFINED		-1LL

struct ath79_caldata_fixup {
	char *name;		/* Name of the MTD dev to read from */
	loff_t pcie_caldata_addr[2];	/* Addr (in flash) of radios caldata */
	loff_t wmac_caldata_addr;
	loff_t mac_addr[2];	/* Addr (in flash) of mac addresses */
};

void __init ath79_mtd_caldata_fixup(struct ath79_caldata_fixup *);
