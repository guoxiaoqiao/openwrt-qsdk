/*
 * Atheros CUS227 board support
 *
 * Copyright (c) 2012 Qualcomm-Atheros Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the
 * disclaimer below) provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *  * Neither the name of Qualcomm Atheros nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
 * GRANTED BY THIS LICENSE.  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
 * HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/pci.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>
#include <linux/ar8216_platform.h>
#include <linux/delay.h>
#include <linux/gpio.h>

#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-audio.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-spi.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define CUS227_GPIO_S1			12
#define CUS227_GPIO_S2			13
#define CUS227_GPIO_S3			14
#define CUS227_GPIO_S4			15

#define CUS227_KEYS_POLL_INTERVAL	20	/* msecs */
#define CUS227_KEYS_DEBOUNCE_INTERVAL	(3 * CUS227_KEYS_POLL_INTERVAL)


#define CUS227_GPIO_I2S_MCLK		22
#define CUS227_GPIO_I2S_SD		18
#define CUS227_GPIO_I2S_WS		20
#define CUS227_GPIO_I2S_CLK		21
#define CUS227_GPIO_I2S_MIC_SD		19
#define CUS227_GPIO_SPDIF_OUT		4

#define CUS227_GPIO_SPI_CS1		11

#define CUS227_MAC0_OFFSET		0
#define CUS227_WMAC_CALDATA_OFFSET	0x1000

static struct gpio_led cus227_leds_gpio[] __initdata = {
	{
		.name		= "cus227:green:s2",
		.gpio		= CUS227_GPIO_S2,
		.active_low	= 1,
	},
	{
		.name		= "cus227:green:s3",
		.gpio		= CUS227_GPIO_S3,
		.active_low	= 1,
	},
	{
		.name		= "cus227:green:s4",
		.gpio		= CUS227_GPIO_S4,
		.active_low	= 1,
	},
};

static struct gpio_keys_button cus227_gpio_keys[] __initdata = {
	{
		.desc		= "WPS button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = CUS227_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= CUS227_GPIO_S1,
		.active_low	= 1,
	},
};

/* Because CUS227 has a different device set on the SPI bus, we cannot
 * reuse the routines from dev-m25p80.c to instanciate the NOR flash */
static struct ath79_spi_controller_data ath79_spi0_cdata =
{
	.cs_type = ATH79_SPI_CS_TYPE_INTERNAL,
	.is_flash = true,
	.cs_line = 0,
};

static struct ath79_spi_controller_data ath79_spi1_cdata =
{
	.cs_type = ATH79_SPI_CS_TYPE_INTERNAL,
	.is_flash = false,
	.cs_line = 1,
};

static struct spi_board_info ath79_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 25000000,
		.modalias	= "m25p80",
		.controller_data = &ath79_spi0_cdata,
	},
	{
		.bus_num	= 0,
		.chip_select	= 1,
		.max_speed_hz   = 25000000,
		.modalias	= "wm8988",
		.controller_data = &ath79_spi1_cdata,
	}
};

static struct ath79_spi_platform_data ath79_spi_data;

static struct platform_device cus227_codec = {
	.name		= "wm8988",
	.id		= -1,
};

static void __init cus227_audio_setup(void)
{
	u32 t;

	/* Reset I2S internal controller */
	t = ath79_reset_rr(AR71XX_RESET_REG_RESET_MODULE);
	ath79_reset_wr(AR71XX_RESET_REG_RESET_MODULE, t | AR934X_RESET_I2S );
	udelay(1);

	/* GPIO configuration
	   Please note that the value in direction_output doesn't really matter
	   here as GPIOs are configured to relay internal data signal
	*/
	gpio_request(CUS227_GPIO_I2S_CLK, "I2S CLK");
	ath79_gpio_output_select(CUS227_GPIO_I2S_CLK, AR934X_GPIO_OUT_MUX_I2S_CLK);
	gpio_direction_output(CUS227_GPIO_I2S_CLK, 0);

	gpio_request(CUS227_GPIO_I2S_WS, "I2S WS");
	ath79_gpio_output_select(CUS227_GPIO_I2S_WS, AR934X_GPIO_OUT_MUX_I2S_WS);
	gpio_direction_output(CUS227_GPIO_I2S_WS, 0);

	gpio_request(CUS227_GPIO_I2S_SD, "I2S SD");
	ath79_gpio_output_select(CUS227_GPIO_I2S_SD, AR934X_GPIO_OUT_MUX_I2S_SD);
	gpio_direction_output(CUS227_GPIO_I2S_SD, 0);

	gpio_request(CUS227_GPIO_I2S_MCLK, "I2S MCLK");
	ath79_gpio_output_select(CUS227_GPIO_I2S_MCLK, AR934X_GPIO_OUT_MUX_I2S_MCK);
	gpio_direction_output(CUS227_GPIO_I2S_MCLK, 0);

	gpio_request(CUS227_GPIO_SPDIF_OUT, "SPDIF OUT");
	ath79_gpio_output_select(CUS227_GPIO_SPDIF_OUT, AR934X_GPIO_OUT_MUX_SPDIF_OUT);
	gpio_direction_output(CUS227_GPIO_SPDIF_OUT, 0);

	gpio_request(CUS227_GPIO_I2S_MIC_SD, "I2S MIC_SD");
	ath79_gpio_input_select(CUS227_GPIO_I2S_MIC_SD, AR934X_GPIO_IN_MUX_I2S_MIC_SD);
	gpio_direction_input(CUS227_GPIO_I2S_MIC_SD);

	/* Init stereo block registers in default configuration */
	ath79_audio_setup();
}

static void __init cus227_register_spi_devices(void)
{
	gpio_request(CUS227_GPIO_SPI_CS1, "SPI CS1");
	ath79_gpio_output_select(CUS227_GPIO_SPI_CS1, AR934X_GPIO_OUT_MUX_SPI_CS1);
	gpio_direction_output(CUS227_GPIO_SPI_CS1, 0);

	ath79_spi_data.bus_num = 0;
	ath79_spi_data.num_chipselect = 2;
	ath79_register_spi(&ath79_spi_data, ath79_spi_info, 2);
}

static void __init cus227_bam_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	cus227_register_spi_devices();

	ath79_register_leds_gpio(-1, ARRAY_SIZE(cus227_leds_gpio),
				 cus227_leds_gpio);
	ath79_register_gpio_keys_polled(-1, CUS227_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(cus227_gpio_keys),
					cus227_gpio_keys);
	ath79_register_usb();
	ath79_register_wmac(art + CUS227_WMAC_CALDATA_OFFSET, NULL);

	/* GMAC1 is connected to the internal switch */
	ath79_init_mac(ath79_eth1_data.mac_addr, art + CUS227_MAC0_OFFSET, 0);
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.speed = SPEED_1000;
	ath79_eth1_data.duplex = DUPLEX_FULL;
	ath79_register_mdio(1, 0x0);
	ath79_register_eth(1);

	/* Audio initialization: PCM/I2S and CODEC */
	cus227_audio_setup();
	platform_device_register(&cus227_codec);
	ath79_audio_device_register();
}

MIPS_MACHINE(ATH79_MACH_CUS227_BAM, "CUS227-BAM",
	     "Qualcomm Atheros CUS227-BAM",
	     cus227_bam_setup);
