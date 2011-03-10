/*
 * arch/sh/boards/st/sat7111/setup.c
 *
 * Copyright (C) 2008 STMicroelectronics Limited
 * Author: Stuart Menefy (stuart.menefy@st.com)
 *
 * May be copied or modified under the terms of the GNU General Public
 * License.  See linux/COPYING for more information.
 *
 * STMicroelectronics STx7111 Mboard support.
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/stm/pio.h>
#include <linux/stm/soc.h>
#include <linux/stm/emi.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/physmap.h>
#include <linux/mtd/partitions.h>
#include <linux/phy.h>
#include <linux/delay.h>
#include <linux/lirc.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>
#include <asm/irq-ilc.h>
#include <asm/irl.h>
#include <asm/io.h>
#include "../common/common.h"

/* Whether the hardware supports NOR or NAND Flash depends on J34.
 * In position 1-2 CSA selects NAND, in position 2-3 is selects NOR.
 * Note that J30A must be in position 2-3 to select the on board Flash
 * (both NOR and NAND).
 */
#define FLASH_NOR

static int ascs[2] __initdata = { 2, 3 };

static void __init sat7111_setup(char** cmdline_p)
{
	printk("STMicroelectronics sat7111 initialisation\n");

	stx7111_early_device_init();
	stx7111_configure_asc(ascs, 2, 0);
}

static struct plat_stm_pwm_data pwm_private_info = {
	.flags		= PLAT_STM_PWM_OUT0,
};

static struct plat_ssc_data ssc_private_info = {
	.capability  =
		ssc0_has(SSC_SPI_CAPABILITY) |
		ssc1_has(SSC_I2C_CAPABILITY) |
		ssc2_has(SSC_I2C_CAPABILITY) |
		ssc3_has(SSC_I2C_CAPABILITY),
};

static struct platform_device sat7111_leds = {
	.name = "leds-gpio",
	.id = -1,
	.dev.platform_data = &(struct gpio_led_platform_data) {
		.num_leds = 2,
		.leds = (struct gpio_led[]) {
			{
				.name = "HB red",
				.default_trigger = "error",
				.gpio = stpio_to_gpio(3, 5),
			},
			{
				.name = "HB white",
				.default_trigger = "power",
				.gpio = stpio_to_gpio(3, 0),
			},
		},
	},
};

static struct gpio_keys_button sat7111_buttons[] = {
	{
		.code = BTN_0,
		.gpio = stpio_to_gpio(6, 2),
		.desc = "KD1",
	},
	{
		.code = BTN_1,
		.gpio = stpio_to_gpio(6, 3),
		.desc = "KD2",
	},
	{
		.code = BTN_2,
		.gpio = stpio_to_gpio(6, 4),
		.desc = "KD3",
	},
	{
		.code = BTN_3,
		.gpio = stpio_to_gpio(6, 5),
		.desc = "KD4",
	},
	{
		.code = BTN_4,
		.gpio = stpio_to_gpio(6, 2),
		.desc = "KD5",
	},
	{
		.code = BTN_5,
		.gpio = stpio_to_gpio(6, 2),
		.desc = "KD6",
	},
	{
		.code = BTN_6,
		.gpio = stpio_to_gpio(6, 2),
		.desc = "KD7",
	},
	{
		.code = BTN_7,
		.gpio = stpio_to_gpio(6, 2),
		.desc = "KD8",
	},
	{
		.code = BTN_8,
		.gpio = stpio_to_gpio(6, 2),
		.desc = "KD9",
	},
	{
		.code = BTN_9,
		.gpio = stpio_to_gpio(6, 2),
		.desc = "KD10",
	},
};

static struct gpio_keys_platform_data sat7111_button_data = {
	.buttons = sat7111_buttons,
	.nbuttons = ARRAY_SIZE(sat7111_buttons),
};

static struct platform_device sat7111_button_device = {
	.name = "gpio-keys",
	.id = -1,
	.num_resources = 0,
	.dev = {
		.platform_data = &sat7111_button_data,
	}
};

/* J34 must be in the 2-3 position to enable NOR Flash */
static struct stpio_pin *vpp_pio;
static struct stpio_pin *phy_reset_pio;

static void set_vpp(struct map_info * info, int enable)
{
	stpio_set_pin(vpp_pio, enable);
}

static struct mtd_partition mtd_parts_table[3] = {
	{
		.name = "Boot firmware",
		.size = 0x00040000,
		.offset = 0x00000000,
	}, {
		.name = "Kernel",
		.size = 0x00100000,
		.offset = 0x00040000,
	}, {
		.name = "Root FS",
		.size = MTDPART_SIZ_FULL,
		.offset = 0x00140000,
	}
};

static struct physmap_flash_data physmap_flash_data = {
	.width		= 2,
	.set_vpp	= set_vpp,
	.nr_parts	= ARRAY_SIZE(mtd_parts_table),
	.parts		= mtd_parts_table
};

static struct platform_device physmap_flash = {
	.name		= "physmap-flash",
	.id		= -1,
	.num_resources	= 1,
	.resource	= (struct resource[]) {
		{
			.start		= 0x00000000,
			.end		= 32*1024*1024 - 1,
			.flags		= IORESOURCE_MEM,
		}
	},
	.dev		= {
		.platform_data	= &physmap_flash_data,
	},
};

static int sat7111_phy_reset(void *bus)
{
	stpio_set_pin(phy_reset_pio, 1);
	udelay(1);
	stpio_set_pin(phy_reset_pio, 0);
	udelay(1);
	stpio_set_pin(phy_reset_pio, 1);
    
	return 1;
}

static struct plat_stmmacphy_data phy_private_data = {
	/* SMSC LAN 8700 */
	.bus_id = 0,
	.phy_addr = 0,
	.phy_mask = 0,
	.phy_reset = &sat7111_phy_reset,
	.interface = PHY_INTERFACE_MODE_MII,
};

static struct platform_device sat7111_phy_device = {
	.name		= "stmmacphy",
	.id		= 0,
	.num_resources	= 1,
	.resource	= (struct resource[]) {
		{
			.name	= "phyirq",
			.start	= -1,/*FIXME*/
			.end	= -1,
			.flags	= IORESOURCE_IRQ,
		},
	},
	.dev = {
		.platform_data = &phy_private_data,
	}
};


/* J34 must be in the 1-2 position to enable NOR Flash */
static struct mtd_partition nand_partitions[] = {
	{
		.name	= "NAND root",
		.offset	= 0,
		.size 	= 0x00800000
	}, {
		.name	= "NAND home",
		.offset	= MTDPART_OFS_APPEND,
		.size	= MTDPART_SIZ_FULL
	},
};

static struct nand_config_data sat7111_nand_config = {
	.emi_bank		= 0,
	.emi_withinbankoffset	= 0,

	/* Timings for NAND512W3A */
	.emi_timing_data = &(struct emi_timing_data) {
		.rd_cycle_time	= 40,		 /* times in ns */
		.rd_oee_start	= 0,
		.rd_oee_end	= 10,
		.rd_latchpoint	= 10,
		.busreleasetime = 0,

		.wr_cycle_time	= 40,
		.wr_oee_start	= 0,
		.wr_oee_end	= 10,
	},

	.chip_delay		= 40,		/* time in us */
	.mtd_parts		= nand_partitions,
	.nr_parts		= ARRAY_SIZE(nand_partitions),
};

static struct platform_device *sat7111_devices[] __initdata = {
	&sat7111_leds,
#ifdef FLASH_NOR
	&physmap_flash,
#endif
	&sat7111_phy_device,
	&sat7111_button_device,
};

/* Configuration based on Futarque-RC signals train. */
lirc_scd_t lirc_scd = {
	.code = 0x3FFFC028,
	.codelen = 0x1e,
	.alt_codelen = 0,
	.nomtime = 0x1f4,
	.noiserecov = 0,
};

static int __init device_init(void)
{
	stx7111_configure_pwm(&pwm_private_info);
	stx7111_configure_ssc(&ssc_private_info);
	stx7111_configure_usb(1); /* Enable inverter */
	stx7111_configure_ethernet(1, 0, 0, 0);
    stx7111_configure_lirc(&lirc_scd);

	vpp_pio = stpio_request_pin(3,4, "VPP", STPIO_OUT);
    phy_reset_pio = stpio_request_pin(2,4, "PHY", STPIO_OUT);

#ifndef FLASH_NOR
	stx7111_configure_nand(&sat7111_nand_config);
	/* The MTD NAND code doesn't understand the concept of VPP,
	 * (or hardware write protect) so permanently enable it.
	 */
	stpio_set_pin(vpp_pio, 1);
#endif

	return platform_add_devices(sat7111_devices, ARRAY_SIZE(sat7111_devices));
}
arch_initcall(device_init);

static void __iomem *sat7111_ioport_map(unsigned long port, unsigned int size)
{
	/* However picking somewhere safe isn't as easy as you might think.
	 * I used to use external ROM, but that can cause problems if you are
	 * in the middle of updating Flash. So I'm now using the processor core
	 * version register, which is guaranted to be available, and non-writable.
	 */
	return (void __iomem *)CCN_PVR;
}

static void __init sat7111_init_irq(void)
{
}

struct sh_machine_vector mv_sat7111 __initmv = {
	.mv_name		= "sat7111",
	.mv_setup		= sat7111_setup,
	.mv_nr_irqs		= NR_IRQS,
	.mv_init_irq		= sat7111_init_irq,
	.mv_ioport_map		= sat7111_ioport_map,
};
