/*
 * (C) Copyright 2001
 * Gerald Van Baren, Custom IDEAS, vanbaren@cideas.com.
 * (C) Copyright 2009 STMicroelectronics. Sean McGoogan <Sean.McGoogan@st.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _SPI_H_
#define _SPI_H_

#include <linux/platform_device.h>

#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 2, 18)
#error "**********************************************************"
#error " Sorry, this driver needs kernel version 2.2.18 or higher "
#error "**********************************************************"
#endif
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/smp_lock.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm/semaphore.h>
#include <asm/errno.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#include <linux/wrapper.h>
#endif
#define __KERNEL_SYSCALLS__
#include <linux/unistd.h>
/* DevFS header */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
#endif
/* SysFS header */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
#endif
#include <linux/device.h>
#include <linux/stm/soc.h>
#include <linux/init.h>    /* Initiliasation support */
#include <linux/module.h>  /* Module support */
#include <linux/kernel.h>  /* Kernel support */
#include <linux/version.h> /* Kernel version */
#include <linux/fs.h>      /* File operations (fops) defines */
#include <linux/cdev.h>    /* Charactor device support */
#include <linux/netdevice.h> /* ??? SET_MODULE_OWNER ??? */
#include <linux/ioport.h>  /* Memory/device locking macros   */
#include <linux/proc_fs.h>
#include <linux/interrupt.h>
#include <linux/errno.h>   /* Defines standard error codes */
#include <linux/sched.h>   /* Defines pointer (current) to current task */
#include <linux/stm/pio.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/seqlock.h>
#include <asm/timer.h>
#include <asm/rtc.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/clock.h>

//#define   NULL   0
//typedef unsigned int  __kernel_size_t;
//typedef __kernel_size_t		size_t;
//typedef  int		              ssize_t;

typedef unsigned char		uchar;

#define   CONFIG_SPI
#define  CONFIG_SOFT_SPI
#define  CONFIG_SPI_FLASH_ST

#define STX7105_COMMS_BASE 0xfd000000
#define ST40_SSC0_REGS_BASE (STX7105_COMMS_BASE + 0x00040000)

#define STPIO_NONPIO		0	/* Non-PIO function (ST40 defn) */
#define STPIO_BIDIR_Z1		0	/* Input weak pull-up (arch defn) */
#define STPIO_BIDIR		1	/* Bidirectonal open-drain */
#define STPIO_OUT		2	/* Output push-pull */
/*efine STPIO_BIDIR		3	* Bidirectional open drain */
#define STPIO_IN		4	/* Input Hi-Z */
/*efine STPIO_IN		5	* Input Hi-Z */
#define STPIO_ALT_OUT		6	/* Alt output push-pull (arch defn) */
#define STPIO_ALT_BIDIR		7	/* Alt bidir open drain (arch defn) */


#define STPIO_POUT_OFFSET	0x00
#define STPIO_PIN_OFFSET	0x10
#define STPIO_PC0_OFFSET	0x20
#define STPIO_PC1_OFFSET	0x30
#define STPIO_PC2_OFFSET	0x40
#define STPIO_PCOMP_OFFSET	0x50
#define STPIO_PMASK_OFFSET	0x60

#define STPIO_SET_OFFSET	0x4
#define STPIO_CLEAR_OFFSET	0x8

#define PIO_PORT_SIZE		0x1000	
#define PIO_BASE                     0xfe010000


#define smit_writel(b,addr)   (*(volatile unsigned int *)(addr) = (b)) 
#define smit_readl(addr)       (*(volatile unsigned int *)(addr))
#define MAX_DEVICES 1

#define PIO_PORT(n)		( ((n-7)*PIO_PORT_SIZE) + PIO_BASE)	

#define PIN_C0(PIN, DIR)	((((DIR) & 0x1)!=0) << (PIN))
#define PIN_C1(PIN, DIR)	((((DIR) & 0x2)!=0) << (PIN))
#define PIN_C2(PIN, DIR)	((((DIR) & 0x4)!=0) << (PIN))
#define CLEAR_PIN_C0(PIN, DIR)	((((DIR) & 0x1)==0) << (PIN))
#define CLEAR_PIN_C1(PIN, DIR)	((((DIR) & 0x2)==0) << (PIN))
#define CLEAR_PIN_C2(PIN, DIR)	((((DIR) & 0x4)==0) << (PIN))

static unsigned int major = 209;
static struct cdev         *spi25pxx_cdev  = NULL;


#define  STPIO_SET_PIN(PIO_ADDR, PIN, V) \
do { \
	smit_writel( 1<<(PIN),(PIO_ADDR) + STPIO_POUT_OFFSET +((V)? STPIO_SET_OFFSET : STPIO_CLEAR_OFFSET)); \
      } while (0)

#define STPIO_GET_PIN(PORT, PIN)       \
	((smit_readl(PIO_PORT(PORT)+STPIO_PIN_OFFSET)>>(PIN))&0x01)


#define SET_PIO_PIN(PIO_ADDR, PIN, DIR)			\
do {									\
	smit_writel(	PIN_C0((PIN),(DIR)),					\
		(PIO_ADDR)+STPIO_PC0_OFFSET+STPIO_SET_OFFSET);	\
	smit_writel(	PIN_C1((PIN),(DIR)),					\
		(PIO_ADDR)+STPIO_PC1_OFFSET+STPIO_SET_OFFSET);	\
	smit_writel(	PIN_C2((PIN),(DIR)),					\
		(PIO_ADDR)+STPIO_PC2_OFFSET+STPIO_SET_OFFSET);	\
	smit_writel(	CLEAR_PIN_C0((PIN),(DIR)),			\
		(PIO_ADDR)+STPIO_PC0_OFFSET+STPIO_CLEAR_OFFSET);	\
	smit_writel(	CLEAR_PIN_C1((PIN),(DIR)),			\
		(PIO_ADDR)+STPIO_PC1_OFFSET+STPIO_CLEAR_OFFSET);	\
	smit_writel(	CLEAR_PIN_C2((PIN),(DIR)),			\
		(PIO_ADDR)+STPIO_PC2_OFFSET+STPIO_CLEAR_OFFSET);	\
} while (0)



#if defined(CONFIG_SOFT_SPI)			/* Use "bit-banging" for SPI */
#	define SPI_SCL(val)	do { stx7105_spi_scl((val)); } while (0)
#	define SPI_SDA(val)	do { stx7105_spi_sda((val)); } while (0)
#	define SPI_DELAY	do { udelay(1); } while (0)	/* QQQ: only 500 kHz ??? */
#	define SPI_READ		stx7105_spi_read()
#endif	/* CONFIG_SOFT_SPI */




/*
 * The function call pointer type used to drive the chip select.
 */
typedef void (*spi_chipsel_type)(int cs);


/*-----------------------------------------------------------------------
 * Initialization, must be called once on start up.
 */
void spi_init(void);
extern ssize_t spi_read (uchar * const addr,const int alen,uchar * const buffer,const int len);
extern ssize_t spi_write (uchar * const addr,const int alen,uchar * buffer,const int len);

/*-----------------------------------------------------------------------
 * SPI transfer
 *
 * This writes "bitlen" bits out the SPI MOSI port and simultaneously clocks
 * "bitlen" bits in the SPI MISO port.  That's just the way SPI works.
 *
 * The source of the outgoing bits is the "dout" parameter and the
 * destination of the input bits is the "din" parameter.  Note that "dout"
 * and "din" can point to the same memory location, in which case the
 * input data overwrites the output data (since both are buffered by
 * temporary variables, this is OK).
 *
 * If the chipsel() function is not NULL, it is called with a parameter
 * of '1' (chip select active) at the start of the transfer and again with
 * a parameter of '0' at the end of the transfer.
 *
 * If the chipsel() function _is_ NULL, it the responsibility of the
 * caller to make the appropriate chip select active before calling
 * spi_xfer() and making it inactive after spi_xfer() returns.
 *
 * spi_xfer() interface:
 *   chipsel: Routine to call to set/clear the chip select:
 *              if chipsel is NULL, it is not used.
 *              if(cs),  make the chip select active (typically '0').
 *              if(!cs), make the chip select inactive (typically '1').
 *   dout:    Pointer to a string of bits to send out.  The bits are
 *              held in a byte array and are sent MSB first.
 *   din:     Pointer to a string of bits that will be filled in.
 *   bitlen:  How many bits to write and read.
 *
 *   Returns: 0 on success, not 0 on failure
 */
int  spi_xfer(spi_chipsel_type chipsel, int bitlen, uchar *dout, uchar *din);

/*
 * External table of chip select functions (see the appropriate board
 * support for the actual definition of the table).
 */
extern spi_chipsel_type spi_chipsel[];
extern int spi_chipsel_cnt;


#if defined(CONFIG_SPI)
extern void configSpi(void)
{
#if defined(CONFIG_SOFT_SPI)
	/* Configure SPI Serial Flash for PIO "bit-banging" */

#if 0
	/*
	 * On the PDK-7105 board, the following 4 pairs of PIO
	 * pins are connected together with a 3K3 resistor.
	 *
	 *	SPI_CLK  PIO15[0] <-> PIO2[5] COM_CLK
	 *	SPI_DOUT PIO15[1] <-> PIO2[6] COM_DOUT
	 *	SPI_NOCS PIO15[2] <-> PIO2[4] COM_NOTCS
	 *	SPI_DIN  PIO15[3] <-> PIO2[7] COM_DIN
	 *
	 * To minimise drive "contention", we may set
	 * associated pins on PIO2 to be simple inputs.
	 */
	SET_PIO_PIN(PIO_PORT(2),4,STPIO_IN);	/* COM_NOTCS */
	SET_PIO_PIN(PIO_PORT(2),5,STPIO_IN);	/* COM_CLK */
	SET_PIO_PIN(PIO_PORT(2),6,STPIO_IN);	/* COM_DOUT */
	SET_PIO_PIN(PIO_PORT(2),7,STPIO_IN);	/* COM_DIN */
#endif

	/* SPI is on PIO15:[3:0] */
	SET_PIO_PIN(PIO_PORT(15),3,STPIO_IN);	/* SPI_DIN */
	SET_PIO_PIN(PIO_PORT(15),0,STPIO_OUT);	/* SPI_CLK */
	SET_PIO_PIN(PIO_PORT(15),1,STPIO_OUT);	/* SPI_DOUT */
	SET_PIO_PIN(PIO_PORT(15),2,STPIO_OUT);	/* SPI_NOCS */

	/* drive outputs with sensible initial values */
	STPIO_SET_PIN(PIO_PORT(15), 2, 1);	/* deassert SPI_NOCS */
	STPIO_SET_PIN(PIO_PORT(15), 0, 1);	/* assert SPI_CLK */
	STPIO_SET_PIN(PIO_PORT(15), 1, 0);	/* deassert SPI_DOUT */
#endif	/* CONFIG_SOFT_SPI */
}
#endif	/* CONFIG_SPI */


#if defined(CONFIG_SOFT_SPI)			/* Use "bit-banging" for SPI */
extern void stx7105_spi_scl(const int val)
{
	const int pin = 0;	/* PIO15[0] = SPI_CLK */
	STPIO_SET_PIN(PIO_PORT(15), pin, val ? 1 : 0);
}

extern void stx7105_spi_sda(const int val)
{
	const int pin = 1;	/* PIO15[1] = SPI_DOUT */
	STPIO_SET_PIN(PIO_PORT(15), pin, val ? 1 : 0);
}

extern unsigned char stx7105_spi_read(void)
{
	const int pin = 3;	/* PIO15[3] = SPI_DIN */
	return STPIO_GET_PIN(15, pin);
}

/*
 * assert or de-assert the SPI Chip Select line.
 *
 *	input: cs == true, assert CS, else deassert CS
 */
static void spi_chip_select(const int cs)
{
	const int pin = 2;	/* PIO15[2] = SPI_NOTCS */

	if (cs)
	{	/* assert SPI CSn */
		STPIO_SET_PIN(PIO_PORT(15), pin, 0);
	}
	else
	{	/* DE-assert SPI CSn */
		STPIO_SET_PIN(PIO_PORT(15), pin, 1);
	}

	if (cs)
	{	/* wait 250ns for CSn assert to propagate  */
		udelay(1);	/* QQQ: can we make this shorter ? */
	}
}

/*
 * The SPI command uses this table of functions for controlling the SPI
 * chip selects: it calls the appropriate function to control the SPI
 * chip selects.
 */
spi_chipsel_type spi_chipsel[] =
{
	spi_chip_select
};
int spi_chipsel_cnt = sizeof(spi_chipsel) / sizeof(spi_chipsel[0]);
#endif	/* CONFIG_SOFT_SPI */

#endif	/* _SPI_H_ */
