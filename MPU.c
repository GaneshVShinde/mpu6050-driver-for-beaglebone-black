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
#define BUF_SIZE 20

dev_t deviceno;							//device number			
struct class *MPU6050_class;					//device class
struct device *MPU6050_device;					//device
char kbuff[BUF_SIZE];						//KERNEL buffer

//device specific structure//
struct MPU6050_dev
{
	struct i2c_client *client;
	struct cdev cdev;
	struct kobject *kobj_conf_dir;
};

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CDAC");
MODULE_DESCRIPTION("I2C DRIVER FOR MPU6050");

// open function
int MPU6050_open(struct inode *inode, struct file *filep)
{
	struct MPU6050_dev *dev = filep->private_data = container_of(inode->i_cdev,struct MPU6050_dev,cdev);
	struct i2c_client *client = dev->client;
	printk("open function\n");
	return 0;
}


// close function
int MPU6050_close(struct inode *inode, struct file *filep)
{
	printk("Cloes function called\n");
	struct MPU6050_dev *dev = filep->private_data;
	struct i2c_client *client = dev->client;
	dev_info(&client->dev, "Close function called");
	return 0;
}


//write function
ssize_t MPU6050_write (struct file *filep, const char __user *ubuff, size_t cnt, loff_t *off)
{
	return 0;
}


//read function
ssize_t MPU6050_read (struct file *filep, char __user *ubuff, size_t cnt, loff_t *off)
{
	u8 j=0,addr;
	u8 command=0x3B; 			//reading from ACCEL_XOUT_H 

	printk("In read function\n");
	
	struct MPU6050_dev *dev = filep->private_data;
   	struct i2c_client *client = dev->client;

	i2c_smbus_write_byte_data (client,0x6B,0x00);			//configuring PWR_MGMT_1 register

	addr=i2c_smbus_read_byte_data (client,0x75);			//reading WHO_AM_I register
	printk("who am I register slave address=%x\n",addr);
	memset(kbuff,'\0',BUF_SIZE);
	while(j<15)
	{
	kbuff[j]=i2c_smbus_read_byte_data (client,command++);		//SMBUS read command
        printk("data at %d-%d\n",j,kbuff[j]);
        j++;
	}
        
	if(copy_to_user(ubuff,kbuff,BUF_SIZE))				//copying data to user
		return -EFAULT;

	return 0;
}


/*  ID table */
static struct i2c_device_id MPU6050_id[] = {
		{"MPU6050", 0}
};


static const struct file_operations MPU6050_fops = {
                .open                   = MPU6050_open,
         //      .write                  = MPU6050_write,
                .read                   = MPU6050_read,
                .release                = MPU6050_close,

};

//probe function
int MPU6050_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
        u8 retval;
	struct MPU6050_dev *dev;
//      u8 command=0x3B;

	printk("In probe function\n");
	dev_info(&client->dev, "MPU6050 device probed");

	dev = devm_kzalloc(&client->dev, sizeof(*dev), GFP_KERNEL);				//allocating memory for struct dev
	if (!dev)
	{
		dev_err(&client->dev, "Failed to allocate memory for private data\n");
		return -ENOMEM;
	}

	dev_info(&client->dev, "Allocated memory for private data\n");

	i2c_set_clientdata(client, dev);
	dev->client = client;

	cdev_init(&dev->cdev,&MPU6050_fops);

        retval=cdev_add(&dev->cdev,deviceno,1);
	if (retval)
	{
		dev_err(&client->dev, "Cdev_Add Failed");
		goto cdev_init_fail;
	}

	dev_info(&client->dev, "Added cdev Structure\n");

	MPU6050_device= device_create(MPU6050_class,NULL,deviceno,NULL,"MPU6050");		//creating a device
	if (MPU6050_device==NULL)
	{
		dev_err(&client->dev, "Failed to create ddvice");
		goto device_register_fail;
	}

	dev_info(&client->dev, "Device created\n");

	printk("WHO M I=%x\n",i2c_smbus_read_byte_data(client,0x75));				//Slave address=0x68
      
	i2c_smbus_write_byte_data(client,0x6B,0x02);
	printk("power management register=%d\n",i2c_smbus_read_byte_data(client,0x6B));
	i2c_smbus_write_byte_data(client,0x19,0x07);
	printk("SMPLRT_DIV register=%d\n",i2c_smbus_read_byte_data(client,0x19));
	i2c_smbus_write_byte_data(client,0x1B,0x08);
	printk("GYRO_CONFIG register=%d\n",i2c_smbus_read_byte_data(client,0x1B));
	i2c_smbus_write_byte_data(client,0x1C,0x00);
	printk("ACCEL_CONFIG register=%d\n",i2c_smbus_read_byte_data(client,0x1C));

	return 0;

	device_register_fail:
		cdev_del(&dev->cdev);
	cdev_init_fail:
		return retval;

	return 0;
}


//remove function
int MPU6050_remove(struct i2c_client *client)
{
	struct MPU6050_dev *dev;

	dev = i2c_get_clientdata(client);
	dev_info(&client->dev, "Remove function called\n");

	device_destroy(MPU6050_class,deviceno);
	dev_info(&client->dev, "Device destroyed\n");

	cdev_del(&dev->cdev);
	dev_info(&client->dev, "Cdev deleted\n");
	dev_info(&client->dev, "MPU6050 device removed");
	return 0;
}

//driver functions
static struct i2c_driver MPU6050_driver = {
                .driver         = {
                                .name   = "MPU6050",
                },
                .probe          = MPU6050_probe,
                .remove         = MPU6050_remove,
                .id_table       = MPU6050_id,

};


//init function
static int __init MPU6050_Final_init(void)
{
	int ret;
	printk("Init Fuction\n");

	alloc_chrdev_region(&deviceno,0,1,"MPU6050");

	MPU6050_class= class_create(THIS_MODULE,"MPU6050");
	ret = i2c_add_driver(&MPU6050_driver);
       	if (ret)
	{
		printk("Failed to add the i2c driver\n");
		return ret;
	}
	printk("I2C driver added\n");
	return 0;

}


//exit function
static void  __exit MPU6050_Final_exit(void)
{
	printk("Exit\n");
	i2c_del_driver(&MPU6050_driver);
	printk("I2C driver deleted\n");
	class_destroy(MPU6050_class);
	printk("Class destroyed\n");
	unregister_chrdev_region(deviceno,1);
	printk("Unregistered Char Device\n");
}

module_init(MPU6050_Final_init);
module_exit(MPU6050_Final_exit);
