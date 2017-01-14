#include<linux/module.h>
#include<linux/init.h>
#include<linux/miscdevice.h>
#include<linux/fs.h>
#include<linux/interrupt.h>
#include<linux/irq.h>
irqreturn_t key_int(int irq,void *dev_id)
{
		//检测是否发生了按键中断
	


		//清除按键中断


		//打印键值
    	  printk("press the key\n");
		 return 0;
}

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
	
	request_irq(IRQ_EINT(9),key_int,IRQF_TRIGGER_FALLING,"mykey",0);

	return 0;
}

static void key_exit()
{
	misc_deregister(&key_miscdev);
	free_irq(IRQ_EINT(9),0);
}

module_init(key_init);
module_exit(key_exit);                              
