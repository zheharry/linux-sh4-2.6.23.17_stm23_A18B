/*
 * arch/sh/boards/st/custom001035/setup.c
 *
 * Copyright (C) 2008 STMicroelectronics Limited
 * Author: Stuart Menefy (stuart.menefy@st.com)
 *
 * May be copied or modified under the terms of the GNU General Public
 * License.  See linux/COPYING for more information.
 *
 * STMicroelectronics STx7141 Mboard support.
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/stm/pio.h>
#include <linux/stm/soc.h>
#include <linux/workqueue.h>
#include <linux/stm/emi.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_bitbang.h>
#include <linux/spi/flash.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/physmap.h>
#include <linux/mtd/partitions.h>
#include <linux/phy.h>
#include <linux/lirc.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <asm/irq-ilc.h>
#include <asm/irl.h>
#include <sound/stm.h>
#include "../common/common.h"

#define FLASH_NOR


static int ascs[] __initdata = {
	1 | (ASC1_PIO10  << 8)	/* PIO6 muxed with TS(NIM) and OOB_??? */
};


static void __init custom001035_setup(char **cmdline_p)
{
	u8 test;

	printk(KERN_INFO "STMicroelectronics STx7141 Mboard initialisation\n");

	stx7141_early_device_init();
	stx7141_configure_asc(ascs, ARRAY_SIZE(ascs), 0);

}

static struct plat_stm_pwm_data pwm_private_info = {
	.flags		= PLAT_STM_PWM_OUT1,
};

static struct plat_ssc_data ssc_private_info = {
	.capability  =
		ssc0_has(SSC_UNCONFIGURED)	/* SSC1 */	|
		ssc1_has(SSC_UNCONFIGURED)	/* SSC2 */	|
		ssc2_has(SSC_UNCONFIGURED)	/* SSC3 */	|
		ssc3_has(SSC_I2C_CAPABILITY)	/* SSC4 */	|
		ssc4_has(SSC_I2C_CAPABILITY)	/* SSC5 */	|
		ssc5_has(SSC_I2C_CAPABILITY)	/* SSC6 */	|
		ssc6_has(SSC_I2C_CAPABILITY),	/* SSC7 */
};

#ifdef FLASH_NOR
/* J69 must be in position 2-3 to enable the on-board Flash devices (both
 * NOR and NAND) rather than STEM). */
/* J89 and J84 must be both in position 1-2 to avoid shorting A15 */
/* J70 must be in the 2-3 position to enable NOR Flash */

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

#else

/* J70 must be in the 1-2 position to enable NAND Flash */
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

static struct nand_config_data custom001035_nand_config = {
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

		.wait_active_low = 0,
	},

	.chip_delay		= 40,		/* time in us */
	.mtd_parts		= nand_partitions,
	.nr_parts		= ARRAY_SIZE(nand_partitions),
};
#endif

static int custom001035_phy_reset(void *bus)
{
	return 1;
}

static struct plat_stmmacphy_data phy_private_data = {
	.bus_id = 0,
	.phy_addr = 31,
	.phy_mask = 0,
	.interface = PHY_INTERFACE_MODE_MII,
	.phy_reset = custom001035_phy_reset,
};

static struct platform_device smsc8700_phy_device = {
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


static struct platform_device *custom001035_devices[] __initdata = {
	&physmap_flash,
	&smsc8700_phy_device,
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
	/*
	 * Can't enable PWM output without conflicting with either
	 * SSC6 (audio) or USB1A OC (which is disabled because it is broken,
	 * but would still result in contention).
	 *
	 * stx7141_configure_pwm(&pwm_private_info);
	 */
	stx7141_configure_ssc(&ssc_private_info);
	stx7141_configure_usb(0);

	/* This requires fitting jumpers J52A 1-2 and J52B 4-5 */
	/* STSDK - FAE/STAPIREF_COMPAT - Remove this usb to have QAM working */
	/* stx7141_configure_usb(1); */

	if (cpu_data->cut_major > 1) {

		/* Moved this under cut2 or more since on cut1 OC protection was disabled */
		/* and it was causing tuner failure problem on cut1,                      */
		/* therefore, we used to comment this cut1 in IP partition patch          */
		/* This requires fitting jumpers J52A 1-2 and J52B 4-5                    */
		stx7141_configure_usb(1); 
		
		/*As per Sti7141 S/W Arc doc v0.5.0==>both USB1.1 given to eCM*/
		/* stx7141_configure_usb(2); */
		/* stx7141_configure_usb(3); */

		stx7141_configure_sata();
	}

    {
	struct stpio_pin *pin; 
#if 0 /* GMAC 0 dedicated to eCM */
	pin=stpio_request_set_pin(9,4, "PHY_RESET_94",STPIO_OUT, 0);
	stpio_free_pin(pin);
	pin=stpio_request_set_pin(9,5, "PHY_RESET_95",STPIO_OUT, 0);
	stpio_free_pin(pin);
	pin=stpio_request_set_pin(10,7, "PHY_RESET_107",STPIO_OUT, 0);
	stpio_free_pin(pin);
#endif
	pin=stpio_request_set_pin(11,6,"PHY_RESET_116",STPIO_OUT, 0);
	stpio_free_pin(pin);
	pin=stpio_request_set_pin(13,1,"PHY_RESET_131",STPIO_OUT, 0);
	stpio_free_pin(pin);
	pin=stpio_request_set_pin(13,2,"PHY_RESET_132",STPIO_OUT, 1);
	stpio_free_pin(pin);
	pin=stpio_request_set_pin(13,3,"PHY_RESET_133",STPIO_OUT, 1);
	stpio_free_pin(pin);
	pin=stpio_request_set_pin(13,4,"PHY_RESET_134",STPIO_OUT, 1);
	stpio_free_pin(pin);
	pin=stpio_request_set_pin(13,5,"PHY_RESET_135",STPIO_OUT, 1);
	stpio_free_pin(pin);
	pin=stpio_request_set_pin(14,2,"PHY_RESET_142",STPIO_OUT, 1);
	stpio_free_pin(pin);
	pin=stpio_request_set_pin(14,3,"PHY_RESET_143",STPIO_OUT, 0);
	stpio_free_pin(pin);
	pin=stpio_request_set_pin(14,4,"PHY_RESET_144",STPIO_OUT, 1);
	stpio_free_pin(pin);
	pin=stpio_request_set_pin(14,5,"PHY_RESET_145",STPIO_OUT, 1);
	stpio_free_pin(pin);
	pin=stpio_request_set_pin(16,6,"PHY_RESET_166",STPIO_OUT, 1);
	stpio_free_pin(pin);
	pin=stpio_request_set_pin(11,3,"PHY_RESET_113",STPIO_OUT, 0);
	stpio_free_pin(pin);
	mdelay(100);
	pin=stpio_request_set_pin(11,3,"PHY_RESET_113",STPIO_OUT, 1);
	stpio_free_pin(pin);
	mdelay(100);
	}

	stx7141_configure_ethernet(1, 0, 0, 0);
#if 0
	stx7141_configure_lirc(&lirc_scd);
#endif

#ifndef FLASH_NOR
	stx7141_configure_nand(&custom001035_nand_config);
	/* The MTD NAND code doesn't understand the concept of VPP,
	 * (or hardware write protect) so permanently enable it.
	 */
#endif

	return platform_add_devices(custom001035_devices, ARRAY_SIZE(custom001035_devices));
}
arch_initcall(device_init);

static void __iomem *custom001035_ioport_map(unsigned long port, unsigned int size)
{
	/*
	 * No IO ports on this device, but to allow safe probing pick
	 * somewhere safe to redirect all reads and writes.
	 */
	return (void __iomem *)CCN_PVR;
}

static void __init custom001035_init_irq(void)
{
}

struct sh_machine_vector mv_custom001035 __initmv = {
	.mv_name		= "custom001035",
	.mv_setup		= custom001035_setup,
	.mv_nr_irqs		= NR_IRQS,
	.mv_init_irq		= custom001035_init_irq,
	.mv_ioport_map		= custom001035_ioport_map,
};



