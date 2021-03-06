/*
*  TP-LINK TL-WR802N
*
*  Copyright (C) 2015 Rick Pannen <pannen@gmail.com>
*
*  This program is free software; you can redistribute it and/or modify it
*  under the terms of the GNU General Public License version 2 as published
*  by the Free Software Foundation.
*/

#include <linux/gpio.h>
#include <linux/platform_device.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define TL_WR802N_GPIO_LED_SYSTEM	13
#define TL_WR802N_GPIO_BTN_RESET	11

#define TL_WR802N_KEYS_POLL_INTERVAL	20	/* msecs */
#define TL_WR802N_KEYS_DEBOUNCE_INTERVAL	(3 * TL_WR802N_KEYS_POLL_INTERVAL)

static const char *tl_wr802n_part_probes[] = {
	"tp-link",
	NULL,
};

static struct flash_platform_data tl_wr802n_flash_data = {
	.part_probes	= tl_wr802n_part_probes,
};

static struct gpio_led tl_wr802n_leds_gpio[] __initdata = {
	{
		.name		= "tp-link:blue:system",
		.gpio		= TL_WR802N_GPIO_LED_SYSTEM,
		.active_low	= 1,
	},
};

static struct gpio_keys_button tl_wr802n_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = TL_WR802N_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TL_WR802N_GPIO_BTN_RESET,
		.active_low	= 0,
	}
};

static void __init tl_ap143_setup(void)
{
       u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);
       u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);
       u8 tmpmac[ETH_ALEN];

       ath79_register_m25p80(&tl_wr802n_flash_data);

       ath79_setup_ar933x_phy4_switch(false, false);

       ath79_register_mdio(0, 0x0);

       /* LAN */
       ath79_switch_data.phy4_mii_en = 1;
       ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
       ath79_eth0_data.duplex = DUPLEX_FULL;
       ath79_eth0_data.speed = SPEED_100;
       ath79_eth0_data.phy_mask = BIT(4);
       ath79_init_mac(ath79_eth0_data.mac_addr, mac, 1);
       ath79_register_eth(0);

       ath79_init_mac(tmpmac, mac, 0);
       ath79_register_wmac(ee, tmpmac);
};

static void __init tl_wr802n_setup(void)
{
	tl_ap143_setup();

	ath79_register_leds_gpio(-1, ARRAY_SIZE(tl_wr802n_leds_gpio),
				 tl_wr802n_leds_gpio);

	ath79_register_gpio_keys_polled(1, TL_WR802N_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(tl_wr802n_gpio_keys),
					tl_wr802n_gpio_keys);
}

MIPS_MACHINE(ATH79_MACH_TL_WR802N, "TL-WR802N", "TP-LINK TL-WR802N", tl_wr802n_setup);
