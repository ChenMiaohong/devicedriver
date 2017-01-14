#include<linux/module.h>
#include<linux/init.h>
#include<linux/miscdevice.h>
#include<linux/fs.h>
#include<linux/interrupt.h>
#include<linux/io.h>
#include<linux/irq.h>
#include<linux/slab.h>
#include<linux/vmalloc.h>
MODULE_LICENSE("GPL");
#define GPX1CON 0x11000C20
unsigned int *gpx1con;
struct work_struct *work1;

void work1_fun(struct work_struct *work)
{
    printk("press the key\n");
}
irqreturn_t key_int(int irq,void *dev_id)
{
		//检测是否发生了按键中断
	


		//清除按键中断




		//提交下半部分	
    	schedule_work(work1);
		//printk("press the key\n");

		return 0;
}
void keyinit()
{
	gpx1con = ioremap(GPX1CON,4);
	writel(readl(gpx1con)&~(0xf<<4)|(0xf<<4),gpx1con);
	

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
	 keyinit();

	request_irq(IRQ_EINT(9),key_int,IRQF_TRIGGER_FALLING,"mykey",0);

	
   work1 = kmalloc(sizeof(struct work_struct),GFP_KERNEL);

   INIT_WORK(work1,work1_fun);
	return 0;
}

static void key_exit()
{
	misc_deregister(&key_miscdev);
	free_irq(IRQ_EINT(9),0);
}

module_init(key_init);
module_exit(key_exit);                              
