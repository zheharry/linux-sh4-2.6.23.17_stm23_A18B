/*
 * linux/drivers/leds/leds-custom001019.c
 *
 * Copyright (C) 2007 STMicroelectronics Limited
 * Author: Marc Chappellier <marc.chappellier@st.com>
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
#include <asm/io.h>
#include <linux/stm/pio.h>

static struct stpio_pin *ledpio;

static void custom001019_led_set(struct led_classdev *led_cdev, enum led_brightness brightness)
{
 if (brightness)
  stpio_set_pin(ledpio,1);
 else
  stpio_set_pin(ledpio,0);
}

static struct led_classdev custom001019_led = {
	.name = "custom001019-led",
	.brightness_set = custom001019_led_set,
	.default_trigger = "heartbeat"
};

static int __init custom001019_led_init(void)
{
    ledpio = stpio_request_pin(1,2, "LED", STPIO_OUT);
	led_classdev_register(NULL, &custom001019_led);
    return(0);
}

static void __exit custom001019_led_exit(void)
{
	stpio_free_pin(ledpio);
	led_classdev_unregister(&custom001019_led);
}

module_init(custom001019_led_init);
module_exit(custom001019_led_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("LED support for STMicroelectronics custom001019");
MODULE_AUTHOR("Marc Chappellier <marc.chappellier@st.com>");
