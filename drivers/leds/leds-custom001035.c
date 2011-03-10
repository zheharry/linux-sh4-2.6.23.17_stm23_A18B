/*
 * linux/drivers/leds/leds-custom001035.c
 *
 * Copyright (C) 2008 STMicroelectronics Limited
 * Author: Stuart Menefy <stuart.menefy@st.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/leds.h>
#include <linux/io.h>
#include "../../arch/sh/boards/st/common/common.h"

static void custom001035_led_set(struct led_classdev *led_cdev,
			  enum led_brightness brightness)
{
}

static struct led_classdev custom001035_led = {
	.name = "custom001035-led",
	.brightness_set = custom001035_led_set,
	.default_trigger = "heartbeat"
};

static int __init custom001035_led_init(void)
{
	led_classdev_register(NULL, &custom001035_led);
}

static void __exit custom001035_led_exit(void)
{
	led_classdev_unregister(&custom001035_led);
}

module_init(custom001035_led_init);
module_exit(custom001035_led_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("LED support for STMicroelectronics custom001035");
MODULE_AUTHOR("Stuart Menefy <stuart.menefy@st.com>");
