/*
===============================================================================
Driver Name		:		I2Cpcf8574t_Final
Author			:		CDAC-BANGALORE-DESD
License			:		GPL
Description		:		LINUX DEVICE DRIVER PROJECT
===============================================================================
*/

#include"I2Cpcf8574t_Final.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CDAC-BANGALORE-DESD");

/* Device specific structure */
struct pcf8574t_dev
{
	struct i2c_client *client;
	struct cdev cdev;
	struct kobject *kobj_conf_dir;
};
static unsigned char busy_byte;
int pcf8574t_dev_init(struct pcf8574t_dev *dev);

dev_t deviceno;
struct class *pcf8574t_class;

/*  ID table */
static struct i2c_device_id pcf8574t_id[] = {
		{"pcf8574t", 0}
};


int LCD_En_toggle(struct i2c_client *client, unsigned char data)
{
	unsigned char temp;
	int retval;
	temp = data;
	temp |= (1<<2); //E=1

	retval=i2c_smbus_write_byte(client, temp);
	if (retval)
	{
		dev_err(&client->dev, "LCD_En_toggle failed\n");
		return retval;
	}
	temp &= ~(1<<2); //E=0

	retval=i2c_smbus_write_byte(client, temp);
	if (retval)
	{
		dev_err(&client->dev, "LCD_En_toggle failed\n");
		return retval;
	}
	return 0;
}

int LCD_busy(struct i2c_client *client)
{
	int retval;
	retval=i2c_smbus_write_byte(client, BUSY_READ);
	if (retval)
	{
		dev_err(&client->dev, "LCD_busy_check failed\n");
		return retval;
	}
	LCD_En_toggle(client,BUSY_READ);

	busy_byte=i2c_smbus_read_byte(client);

	while(busy_byte & LCD_BUSY)
	{
		printk("Waiting for LCD\n");
		busy_byte=i2c_smbus_read_byte(client);
	}
	return 0;
}

int LCD_data(struct i2c_client *client,unsigned char data)
{
	int retval;
	unsigned char temp;
	temp = data&0xF0;
	temp |= (1<<0); // RS=1
	temp |= (1<<3); // LIGHT=1
	retval=LCD_busy(client);
	if (retval)
	{
		dev_err(&client->dev, "LCD_busy failed\n");
		return retval;
	}

	retval=i2c_smbus_write_byte(client, temp);
	if (retval)
	{
		dev_err(&client->dev, "LCD_data write failed\n");
		return retval;
	}

	retval=LCD_En_toggle(client,temp); // Toggle E bit
	if (retval)
	{
		dev_err(&client->dev, "LCD_toggle failed\n");
		return retval;
	}
	data = data<<4;
	temp = data&0xF0;
	temp |= (1<<0); // RS=1
	temp |= (1<<3); // LIGHT=1

	retval=LCD_busy(client);
	if (retval)
	{
		dev_err(&client->dev, "LCD_busy failed\n");
		return retval;
	}
	retval=i2c_smbus_write_byte(client, temp);
	if (retval)
	{
		dev_err(&client->dev, "LCD_data write failed\n");
		return retval;
	}
	retval=LCD_En_toggle(client,temp); // Toggle E bit
	if (retval)
	{
		dev_err(&client->dev, "LCD_toggle failed\n");
		return retval;
	}
	return 0;
}

int LCD_com(struct i2c_client *client,unsigned char data)
{
	unsigned char temp;
	int retval;
	temp = data&0xF0;
	temp &= ~(1<<0); // RS=0
	temp |= (1<<3); // LIGHT=1
	retval=LCD_busy(client);
	if (retval)
	{
		dev_err(&client->dev, "LCD_toggle failed\n");
		return retval;
	}

	retval=i2c_smbus_write_byte(client, temp);
	if (retval)
	{
		dev_err(&client->dev, "LCD_com write failed\n");
		return retval;
	}

	retval=LCD_En_toggle(client,temp); // Toggle E bit
	if (retval)
	{
		dev_err(&client->dev, "LCD_toggle failed\n");
		return retval;
	}
	data = data<<4;
	temp = data&0xF0;
	temp &= ~(1<<0); // RS=0
	temp |= (1<<3); // LIGHT=1

	retval=LCD_busy(client);
	if (retval)
	{
		dev_err(&client->dev, "LCD_busy failed\n");
		return retval;
	}
	retval=i2c_smbus_write_byte(client, temp);
	if (retval)
	{
		dev_err(&client->dev, "LCD_com write failed\n");
		return retval;
	}

	retval=LCD_En_toggle(client,temp); // Toggle E bit
	if (retval)
	{
		dev_err(&client->dev, "LCD_toggle failed\n");
		return retval;
	}
	return 0;
}

int LCD_com_init(struct i2c_client *client,unsigned char data) //For writing four bits (DB7 - DB4) to lcd for initialisation
{
	unsigned char temp;
	int retval;
	temp = data&0xF0;
	temp &= ~(1<<0); // RS=0
	temp |= (1<<3); // LIGHT=1
	retval=LCD_busy(client);
	if (retval)
	{
		dev_err(&client->dev, "LCD_busy failed\n");
		return retval;
	}

	retval=i2c_smbus_write_byte(client, temp);
	if (retval)
	{
		dev_err(&client->dev, "LCD_com write failed\n");
		return retval;
	}

	retval=LCD_En_toggle(client,temp); // Toggle E bit
	if (retval)
	{
		dev_err(&client->dev, "LCD_toggle failed\n");
		return retval;
	}

	return 0;
}


struct pcf8574t_dev *kobj_to_pcf8574t_dev(struct kobject *kobj) //to get device structure address from kobject. When attribute is getting called only kobject address is available in the function. But we need dev structure address to communicate with device.
{
	struct device *i2cdev = kobj_to_dev(kobj->parent);//container_of(kobj, struct device, kobj);
	struct i2c_client *client = to_i2c_client(i2cdev);//container_of(d, struct i2c_client, dev)

	return i2c_get_clientdata(client);//Function is used to get the void *driver_data pointer that is part of the struct device, itself part of struct i2c_client.
}

//ssize_t (*show)(struct kobject *kobj, struct device_attribute *attr, char *buf);
//ssize_t (*store)(struct kobject *kobj, struct device_attribute *attr, const char *buf, size_t count);

ssize_t lcd_write_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	struct pcf8574t_dev *dev = kobj_to_pcf8574t_dev(kobj);//get dev structure address from kobject
	struct i2c_client *client = dev->client;
	int tcount,i=0;
	dev_info(&client->dev, "LCD write Attribute Called");
	if (count>17)
		return -1;
	pm_runtime_get_sync(&client->dev);//Turn on power. Calls resume function
	//LCD_com(client,0x01);
	//LCD_com(client,0x80);
	tcount=count;
	while(--tcount)
	{
		LCD_data(client,buf[i++]);
	}
	return count;
}

ssize_t lcd_backlight_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	struct pcf8574t_dev *dev = kobj_to_pcf8574t_dev(kobj);
	struct i2c_client *client = dev->client;
	int retval;
	dev_info(&client->dev, "LCD backlight Attribute Called");
	if((buf[0] & BACKLIGHT_CHECK)==0x00)
	{
		retval=i2c_smbus_write_byte(client,0x00);
		if (retval)
		{
			dev_err(&client->dev, "Backlight turn off failed\n");
			return retval;
		}
		dev_info(&client->dev, "Backlight turned off\n");
	}
	if((buf[0] & BACKLIGHT_CHECK)==0x01)
	{
		retval=i2c_smbus_write_byte(client,0x08);
		if (retval)
		{
			dev_err(&client->dev, "Backlight turn on failed\n");
			return retval;
		}
		dev_info(&client->dev, "Backlight turned on\n");
	}
	return count;
}

ssize_t lcd_commands_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	struct pcf8574t_dev *dev = kobj_to_pcf8574t_dev(kobj);
	struct i2c_client *client = dev->client;
	int retval;
	dev_info(&client->dev, "LCD commands Attribute Called");
	
	switch(buf[0])
	{
	case '0':
		retval = LCD_com(client,0x01);
		if (retval)
		{
			dev_err(&client->dev, "LCD display clear failed\n");
			return retval;
		}
		dev_info(&client->dev, "LCD display cleared\n");
		break;

	case '1':
		retval = LCD_com(client,0x80);
		if (retval)
		{
			dev_err(&client->dev, "LCD set to first line failed\n");
			return retval;
		}
		dev_info(&client->dev, "LCD set to first line\n");
		break;

	case '2':
		retval = LCD_com(client,0xC0);
		if (retval)
		{
			dev_err(&client->dev, "LCD set to second line failed\n");
			return retval;
		}
		dev_info(&client->dev, "LCD set to second line\n");
		break;

	//default:
		//return -1;
	}
	return count;
}


static struct kobj_attribute lcd_write_attr = __ATTR(lcd_write, S_IWUSR, NULL, lcd_write_store); //DEVICE_ATTR(name,mode,show,store)
static struct kobj_attribute lcd_backlight_attr = __ATTR(lcd_backlight, S_IWUSR, NULL, lcd_backlight_store);
static struct kobj_attribute lcd_commands_attr = __ATTR(lcd_commands, S_IWUSR, NULL, lcd_commands_store);

static struct attribute *pcf8574t_attrs[] = {
	&lcd_write_attr.attr,
	&lcd_backlight_attr.attr,
	&lcd_commands_attr.attr,
	NULL,
};

static struct attribute_group pcf8574t_attr_group = {
	.attrs = pcf8574t_attrs,
};

//static const struct attribute_group *pcf8574t_attr_groups[] = { //Need with device register()
//	&pcf8574t_attr_group,
//	NULL,
//};

int pcf8574t_open(struct inode *inode, struct file *filep)
{
	int retval;
	struct pcf8574t_dev *dev = filep->private_data = container_of(inode->i_cdev,struct pcf8574t_dev,cdev);
	struct i2c_client *client = dev->client;
	dev_info(&client->dev, "Open function called");
	retval = pm_runtime_get_sync(&client->dev); //power on the device
	if (retval < 0)
		return retval;

	return 0;
}

int pcf8574t_close(struct inode *inode, struct file *filep)
{
	struct pcf8574t_dev *dev = filep->private_data;
	struct i2c_client *client = dev->client;
	dev_info(&client->dev, "Close function called");
	return 0;
}

ssize_t pcf8574t_write (struct file *filep, const char __user *ubuff, size_t cnt, loff_t *off)
{
	char buff[128];
	int i=0;
	struct pcf8574t_dev *dev = filep->private_data;
	struct i2c_client *client = dev->client;
	dev_info(&client->dev, "Write function called");
	if (copy_from_user(buff, ubuff, cnt))
		return -EFAULT;
	while(cnt--)
	{
		LCD_data(client,buff[i++]);
	}

	return cnt;
}

long pcf8574t_ioctl (struct file *filep, unsigned int command, unsigned long arg)
{
	int retval;
	struct pcf8574t_dev *dev = filep->private_data;
	struct i2c_client *client = dev->client;
	dev_info(&client->dev, "IOCTL function called");

	switch(command)
	{
	case LCD_CLEAR:
		retval = LCD_com(client,0x01);
		if (retval)
		{
			dev_err(&client->dev, "LCD display clear failed\n");
			return retval;
		}
		dev_info(&client->dev, "LCD display cleared\n");
		break;

	case LCD_FIRSTLINE:
		retval = LCD_com(client,0x80);
		if (retval)
		{
			dev_err(&client->dev, "LCD set to first line failed\n");
			return retval;
		}
		dev_info(&client->dev, "LCD set to first line\n");
		break;
	case LCD_SECONDLINE:
		retval = LCD_com(client,0xC0);
		if (retval)
		{
			dev_err(&client->dev, "LCD set to second line failed\n");
			return retval;
		}
		dev_info(&client->dev, "LCD set to second line\n");
		break;
	default:
		return -1;
	}
	return 0;
}

static const struct file_operations pcf8574t_fops = {
		.open			= pcf8574t_open,
		.write			= pcf8574t_write,
		.release		= pcf8574t_close,
		.unlocked_ioctl	= pcf8574t_ioctl
};

/* Device Init function */
int pcf8574t_dev_init(struct pcf8574t_dev *dev)
{
	int retval;
	struct i2c_client *client = dev->client;
	dev_info(&client->dev, "Init function called");

	mdelay(15);
	retval = LCD_com_init(client,0x30);
	if (retval)
	{
		dev_err(&client->dev, "LCD function set failed\n");
		return retval;
	}
	mdelay(5);
	retval = LCD_com_init(client,0x30);
	if (retval)
	{
		dev_err(&client->dev, "LCD function set failed\n");
		return retval;
	}
	udelay(100);
	retval = LCD_com_init(client,0x30);
	if (retval)
	{
		dev_err(&client->dev, "LCD function set failed\n");
		return retval;
	}
	dev_info(&client->dev, "LCD function set successful\n");
	retval = LCD_com_init(client,0x20);
	if (retval)
	{
		dev_err(&client->dev, "LCD 4bit data line configuration failed\n");
		return retval;
	}
	retval = LCD_com(client,0x28);
	if (retval)
	{
		dev_err(&client->dev, "LCD 4bit data line configuration failed\n");
		return retval;
	}
	retval = LCD_com(client,0x08);
	if (retval)
	{
		dev_err(&client->dev, "LCD 4bit data line configuration failed\n");
		return retval;
	}
	dev_info(&client->dev, "LCD 4bit data line configuration successful\n");
	retval = LCD_com(client,0x0C);
	if (retval)
	{
		dev_err(&client->dev, "LCD Display on/cursor off failed\n");
		return retval;
	}
	dev_info(&client->dev, "LCD Display on/cursor off successful\n");
	retval = LCD_com(client,0x01);
	if (retval)
	{
		dev_err(&client->dev, "LCD display clear failed\n");
		return retval;
	}
	dev_info(&client->dev, "LCD display cleared\n");
	retval = LCD_com(client,0x06);
	if (retval)
	{
		dev_err(&client->dev, "LCD Entry mode set failed\n");
		return retval;
	}
	dev_info(&client->dev, "LCD Entry mode set\n");

	return 0;
}

/* Probe function */
int pcf8574t_probe(struct i2c_client *client, const struct i2c_device_id *id) //This probe function starts the per-device initialization: initializing hardware, allocating resources, and registering the device with the kernel as a block or network device or whatever it is.. Probe(*pdev) // is used by the kernel driver machine as needed to detect and install actual devices
{
	int retval;
	struct pcf8574t_dev *dev; // Our device specific structure pointer
	struct device *pcf8574t_device;
	dev_info(&client->dev, "PCF8574T device probed");

	dev = devm_kzalloc(&client->dev, sizeof(*dev), GFP_KERNEL); //devm_kzalloc() is resource-managed kzalloc(). The memory allocated with resource-managed functions is associated with the device. When the device is detached from the system or the driver for the device is unloaded, that memory is freed automatically. It is possible to free the memory with devm_kfree() if it's no longer needed.
	if (!dev) {
		dev_err(&client->dev, "Failed to allocate memory for private data\n");
		return -ENOMEM;
	}
	dev_info(&client->dev, "Allocated memory for private data\n");

	i2c_set_clientdata(client, dev);//Function is used to set the void *driver_data pointer that is part of the struct device, itself part of struct i2c_client. This is a void pointer that is for the driver to use. One would use this pointer mainly to pass driver related data around.  You should use this to keep device-specific data.
	dev->client = client; // device specific client is pointing to global client

	cdev_init(&dev->cdev,&pcf8574t_fops);
	retval=cdev_add(&dev->cdev,deviceno,1);
	if (retval)
	{
		dev_err(&client->dev, "Cdev_Add Failed");
		goto cdev_init_fail;
	}
	dev_info(&client->dev, "Added cdev Structure\n");

	/* Helps in device node creation */
	pcf8574t_device= device_create(pcf8574t_class,NULL,deviceno,NULL,"pcf8574t");//A “dev” file will be created, showing the dev_t for the device, if the dev_t is not 0,0.
	if (pcf8574t_device==NULL)
	{
		dev_err(&client->dev, "Failed to create ddvice");
		goto device_register_fail;
	}
	dev_info(&client->dev, "Device created\n");

	dev->kobj_conf_dir = kobject_create_and_add("pcf8574t_conf", &client->dev.kobj);//This function creates a kobject structure dynamically and registers it with sysfs. When you are finished with this structure, call kobject_put and the structure will be dynamically freed when it is no longer being used.
	retval = sysfs_create_group(dev->kobj_conf_dir, &pcf8574t_attr_group);//Creates sysfs attribute group
	if (retval)
	{
		dev_err(&client->dev, "Failed to create sysfs attribute group\n");
		goto sysfs_grp_fail;
	}
	dev_info(&client->dev, "Sysfs attribute group created\n");

	retval = pcf8574t_dev_init(dev);
	if (retval)
	{
		dev_err(&client->dev, "Failed to initialize LCD");
		goto dev_init_fail;
	}
	dev_info(&client->dev, "LCD Initialised\n");

	pm_runtime_set_active(&client->dev); //make device to power down mode (calls suspend function)
	pm_runtime_enable(&client->dev);

	/* After returning from here, the device's suspend function is called
	 * which pushes the device into low-power state.
	 * The device's resume will be called as and when the reference count
	 * for the device (in terms of pm) is incremented, which is achieved
	 * from the pm_runtime_get_(), called in the open() function.
	 * Conversely, device's suspend function will be called as and when the
	 * reference count of the device reaches 0, using pm_runtime_put_(),
	 * which is called in close/release functions.
	 */

	return 0;

	dev_init_fail:
		sysfs_remove_group(dev->kobj_conf_dir, &pcf8574t_attr_group);
		kobject_put(dev->kobj_conf_dir);
	sysfs_grp_fail:
		device_destroy(pcf8574t_class,deviceno);
	device_register_fail:
		cdev_del(&dev->cdev);
	cdev_init_fail:
		return retval;
}

/* Remove function */
int pcf8574t_remove(struct i2c_client *client)
{
	struct pcf8574t_dev *dev;
	dev = i2c_get_clientdata(client); //getting private client data
	dev_info(&client->dev, "Remove function called\n");

	pm_runtime_set_active(&client->dev); //make device to power down mode (calls suspend function)
	pm_runtime_disable(&client->dev);
	dev_info(&client->dev, "Power disabled\n");

	sysfs_remove_group(dev->kobj_conf_dir, &pcf8574t_attr_group);
	kobject_put(dev->kobj_conf_dir);
	dev_info(&client->dev, "Sysfs group removed\n");

	device_destroy(pcf8574t_class,deviceno);
	dev_info(&client->dev, "Device destroyed\n");
	cdev_del(&dev->cdev);
	dev_info(&client->dev, "Cdev deleted\n");

	dev_info(&client->dev, "PCF8574T device removed");
	return 0;
}

int pcf8574t_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	dev_info(&client->dev, "Suspend function called\n");
	return i2c_smbus_write_byte(client, 0x00);// LCD light OFF
}

int pcf8574t_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	dev_info(&client->dev, "Resume function called\n");
	return i2c_smbus_write_byte(client, 0x08); //LCD light ON
}

static UNIVERSAL_DEV_PM_OPS(pcf8574t_pm, pcf8574t_suspend, pcf8574t_resume, NULL);

/* I2C driver structure */
static struct i2c_driver pcf8574t_driver = {
		.driver		= {
				.name	= "pcf8574t",
				.pm	= &pcf8574t_pm
		},
		.probe		= pcf8574t_probe,
		.remove		= pcf8574t_remove,
		.id_table	= pcf8574t_id,
};

static int __init I2Cpcf8574t_Final_init(void)
{
	int retval;
	PINFO("INIT\n");

	alloc_chrdev_region(&deviceno,0,1,"pcf8574t");
	PINFO("Allocated Char Device\n");
	/* This would help you in creating device nodes automatically */
	pcf8574t_class= class_create(THIS_MODULE,"pcf8574t");
	PINFO("Created device class\n");
	retval = i2c_add_driver(&pcf8574t_driver); //add the driver to the list of i2c drivers in the driver core. then registration returns, the driver core will have called probe() for all matching-but-unbound devices.The driver's init function calls i2c_register_driver() which gives the kernel a list of devices it is able to service, along with a pointer to the probe() function. The kernel then calls the driver's probe() function once for each device.
	if (retval)
	{
		pr_err("Failed to add the i2c driver\n");
		return retval;
	}
	PINFO("I2C driver added\n");
	return 0;
}

static void __exit I2Cpcf8574t_Final_exit(void)
{	
	PINFO("EXIT\n");
	i2c_del_driver(&pcf8574t_driver);
	PINFO("I2C driver deleted\n");
	class_destroy(pcf8574t_class);
	PINFO("Class destroyed\n");
	unregister_chrdev_region(deviceno,1);
	PINFO("Unregistered Char Device\n");
}

module_init(I2Cpcf8574t_Final_init);
module_exit(I2Cpcf8574t_Final_exit);

/******** End of File *********/
