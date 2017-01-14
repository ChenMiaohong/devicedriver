#include<linux/module.h>
#include<linux/init.h>
#include<linux/miscdevice.h>

int key_open(struct inode *node,struct file *filp)
{
    return 0;	
}

struct file_operations key_fops = 
{
    .open = key_open,	
};
struct miscdevice key_miscdev = {

	.minor = 200,
	.name = "mykey",
	.fops = &key_fops,
};
static int key_init()
{

	misc_register(&key_miscdev);
	return 0;
}

static void key_exit()
{
	misc_deregister(&key_miscdev);
}

module_init(key_init);
module_exit(key_exit);                              
