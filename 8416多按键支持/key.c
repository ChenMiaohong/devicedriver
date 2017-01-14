#include<linux/module.h>
#include<linux/init.h>
#include<linux/miscdevice.h>
#include<linux/fs.h>
#include<linux/interrupt.h>
#include<linux/io.h>
#include<linux/irq.h>
#include<linux/slab.h>
#include<linux/vmalloc.h>
#include<linux/uaccess.h>
MODULE_LICENSE("GPL");
#define GPX1CON 0x11000C20
#define GPX1DAT 0x11000C24
unsigned int *gpx1con;
unsigned int *gpx1dat;
unsigned int key_num;

struct work_struct *work1;

struct work_struct *work1;
struct timer_list key_timer;
void work1_fun(struct work_struct *work)
{
    mod_timer(&key_timer,jiffies+HZ/10);
	//	printk("press the key\n");
}
void key_timer_func(unsigned long data)
{	unsigned int key_val;
	key_val = readl(gpx1dat)&(0x1<<1);
	if(key_val==0)
	{
		key_num = 1;
	}
		//printk("\nhome key down\n");
	


	
	key_val = readl(gpx1dat)&(0x1<<2);
	if(key_val==0)
	{
		key_num = 2;
	}		
//printk("\n back key down\n");



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
	
	writel(readl(gpx1con)&~(0xff<<4)|(0xff<<4),gpx1con);
	gpx1dat = ioremap(GPX1DAT,4);


}
ssize_t key_read(struct file *filp, char __user *buf, size_t size, loff_t *pos)
{
    printk("in kernel :key num is %d\n",key_num);	
    copy_to_user(buf, &key_num, 4);
    
    return 4;
}
int key_open(struct inode *node,struct file *filp)
{
    return 0;	
}

struct file_operations key_fops = 
{
    .open = key_open,
	.read = key_read,	
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
	
	request_irq(IRQ_EINT(10),key_int,IRQF_TRIGGER_FALLING,"mykey",0);
	
    //创建工作
	work1 = kmalloc(sizeof(struct work_struct),GFP_KERNEL);
	INIT_WORK(work1,work1_fun);
	
	init_timer(&key_timer);
	key_timer.function = key_timer_func;    
	add_timer(&key_timer);
	return 0;
}

static void key_exit()
{
	misc_deregister(&key_miscdev);
	free_irq(IRQ_EINT(9),0);
	free_irq(IRQ_EINT(10),0);
}

module_init(key_init);
module_exit(key_exit);                              
