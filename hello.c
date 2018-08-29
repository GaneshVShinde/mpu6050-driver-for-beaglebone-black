#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>
static int init_hello(void)
{
printk(KERN_INFO"Hello World\n");
return 0;
}

static void exit_hello(void)
{

}


module_init(init_hello);
module_exit(exit_hello);
