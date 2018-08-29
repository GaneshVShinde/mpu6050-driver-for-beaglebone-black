
#define DRIVER_NAME "I2Cpcf8574t_Final"
#define PDEBUG(fmt,args...) printk(KERN_DEBUG"%s:"fmt,DRIVER_NAME, ##args)
#define PERR(fmt,args...) printk(KERN_ERR"%s:"fmt,DRIVER_NAME,##args)
#define PINFO(fmt,args...) printk(KERN_INFO"%s:"fmt,DRIVER_NAME, ##args)
#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/uaccess.h>
#include <linux/pm.h>
#include <linux/input-polldev.h>
#include <linux/dcache.h>
#include <linux/device.h>
#include <linux/pm_runtime.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include "LCD_commands.h"

#define LCD_BUSY 		0x80
#define BUSY_READ 		0x0A
#define BACKLIGHT_OFF	0x30
#define BACKLIGHT_ON	0x31
#define BACKLIGHT_CHECK	0x01
