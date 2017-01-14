#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/io.h>
//#include <mach/gpio-bank-k.h>
#include "led.h"  
#define GPL2CON 0x11000100
#define GPL2DAT 0x11000104
#define GPK1CON 0x11000060
#define GPK1DAT 0x11000064

static int *pgpl2con;  
static int *pgpl2dat;  
  
static int *pgpk1con;  
static int *pgpk1dat;

struct cdev cdev;
dev_t devno;
int led_open(struct inode *node, struct file *filp)
{
	pgpl2con = ioremap(GPL2CON,4);  
    pgpl2dat = ioremap(GPL2DAT,4);  
  
    pgpk1con = ioremap(GPK1CON,4);  
    pgpk1dat = ioremap(GPK1DAT,4);  
  
    writel((readl(pgpl2con)& ~(0xf<<0)) |(0x1<<0),pgpl2con) ;     
    writel((readl(pgpk1con)& ~(0xf<<4)) |(0x1<<4),pgpk1con) ;   
	return 0;
}

long led_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch (cmd)
	{
	    case LED_ON:
	         writel(readl(pgpl2dat) |(0x1<<0), pgpl2dat);
    		 writel(readl(pgpk1dat) |(0x1<<1), pgpk1dat);
			printk("OPEN is success\n");
			break;
	    
	    case LED_OFF:
	        writel(readl(pgpl2dat) &(~(0x1<<0)), pgpl2dat);
   			writel(readl(pgpk1dat) &(~(0x1<<1)), pgpk1dat);
			printk("close is success\n");
			break;
	    default:
	    	return -EINVAL;
	}
}

static struct file_operations led_fops =
{
   	.open = led_open,
    .unlocked_ioctl = led_ioctl,
};

static int led_init()
{
    cdev_init(&cdev,&led_fops);
    
    alloc_chrdev_region(&devno, 0 , 1 , "myled");
    cdev_add(&cdev, devno, 1);
    
    return 0;	
}

static void led_exit()
{
	cdev_del(&cdev);
	unregister_chrdev_region(devno,1);
}


module_init(led_init);
module_exit(led_exit);
    
