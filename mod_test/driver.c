#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <asm/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");
#ifndef DRIVER_NAME
#define DRIVER_NAME "mod_test"	/* /proc/devices, /proc/modules */
#endif /* DRIVER_NAME */

static bool verbose = false;
module_param(verbose, bool, S_IRUGO);
MODULE_PARM_DESC(verbose, "(optional) verbose mode");

static unsigned long param0 = 1234;
module_param(param0, ulong, S_IRUGO);
MODULE_PARM_DESC(param0, "param 0");
static unsigned long param1 = 3;
module_param(param1, ulong, S_IRUGO);
MODULE_PARM_DESC(param1, "param 1");
static unsigned long param2 = 0;
module_param(param2, ulong, S_IRUGO);
MODULE_PARM_DESC(param2, "param 2");
static unsigned long param3 = 0;
module_param(param3, ulong, S_IRUGO);
MODULE_PARM_DESC(param3, "param 3");

static int __init mod_init(void);
static void __exit mod_exit(void);

static void print_settings(void) {
	printk(DRIVER_NAME ": verbose=%d param0=%lu param1=%lu param2=%lu param3=%lu\n"
			, verbose, param0, param1, param2, param3);
}


/* At load (insmod) */
static int __init mod_init(void) {
	printk(DRIVER_NAME ": mod_init\n");

	print_settings();

	printk(DRIVER_NAME ": insmod done.\n");
	return 0;
}

/* At unload (rmmod) */
static void __exit mod_exit(void) {
	printk(DRIVER_NAME ": mod_exit\n");
}

module_init(mod_init);
module_exit(mod_exit);
