#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/slab.h>
#include<linux/stat.h>
#include<linux/kdev_t.h>
#include<linux/string.h>
#include<linux/gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/gpio-exynos4.h>
#include <linux/mm.h>
/*dev_t等的定义*/
#include <linux/types.h>
/*包含copy_to_user和copy_from_user的头文件*/
#include <asm/uaccess.h>
/*包含寄存器操作函数的头文件*/
#include <asm/io.h>
/*延时定义*/
#include <linux/delay.h>
/*包含函数device_create 结构体class等头文件*/
#include <linux/device.h>
/*错误诊断和输出需要的头文件*/
#include <linux/errno.h>

#define GPA0CON 0x1140_0000
//GPA0CON寄存器的物理地址
#define GPA0DAT 0x1140_0004
//GPA0DAT寄存器的物理地址
#define ds18b20_io (EXYNOS4_GPA0(7))
static int *gpa0con;
static int *gpa0dat;
struct cdev cdev;
dev_t devno;
void ds18b20_reset(void)
{
    int ret;

    s3c_gpio_cfgpin(ds18b20_io,S3C_GPIO_SFN(1));/*配置成输出*/
    gpio_set_value(ds18b20_io,1);/* 向18B20发送一个上升沿，并保持高电平状态约100微秒*/
    udelay(100);
    gpio_set_value(ds18b20_io,0);/*向18B20发送一个下降沿，并保持低电平状态约600微秒*/
    udelay(600);
    gpio_set_value(ds18b20_io,1);/* 向18B20发送一个上升沿，此时可释放DS18B20总线*/
    udelay(100);
    /*以上操作是给DS18B20一个复位脉冲*/

    s3c_gpio_cfgpin(ds18b20_io,S3C_GPIO_SFN(0));/*配置为输入,可以检测到DS18B20是否复位成功*/
    /*如果低电平出现说明总线上有器件已做出应答*/
    ret = gpio_get_value(ds18b20_io);/*读取io上的电平.若总线在释放后总线状态为高电平，则复位失败*/
    /*if(!ret){
        printk(KERN_EMERG "ds18b20 init is success!\n");
    }
    else{
        printk(KERN_EMERG "ds18b20 init is failed!\n");
    }*/
}

/*写一个字节*/
/*写“1”时隙：
   保持总线在低电平1微秒到15微秒之间
   然后再保持总线在高电平15微秒到60微秒之间
   理想状态: 1微秒的低电平然后跳变再保持60微秒的高电平

写“0”时隙：
    保持总线在低电平15微秒到60微秒之间
    然后再保持总线在高电平1微秒到15微秒之间
    理想状态: 60微秒的低电平然后跳变再保持1微秒的高电平
    */
void write_data (unsigned char dat)
{
    unsigned char i;

    s3c_gpio_cfgpin(ds18b20_io,S3C_GPIO_SFN(1));/*端口设置为输出*/
    for(i=0;i<8;i++){
/*主机想写1，在一开始拉低总线电平1微秒后就释放总线为高电平，一直到写周期结束。*/
        gpio_set_value(ds18b20_io,0);
        udelay(1);

        /*此处为写"1"*/
        /*若byte变量的D0位是1，则需向总线上写“1”
         根据写“1”时隙规则，电平在此处翻转为高*/
        if(((dat)&(0x01))==1)
            gpio_set_value(ds18b20_io,1);
        udelay(80);
        gpio_set_value(ds18b20_io,1);
        udelay(15);
        dat = dat>>1;
    }
    gpio_set_value(ds18b20_io,1);
}

/*读一个字节*/
/* 读“1”时隙：
    若总线状态保持在低电平状态1微秒到15微秒之间
    然后跳变到高电平状态且保持在15微秒到60微秒之间
    就认为从DS18B20读到一个“1”信号
    理想情况: 1微秒的低电平然后跳变再保持60微秒的高电平

读“0”时隙：
    若总线状态保持在低电平状态15微秒到30微秒之间
    然后跳变到高电平状态且保持在15微秒到60微秒之间
    就认为从DS18B20读到一个“0”信号
    理想情况: 15微秒的低电平然后跳变再保持46微秒的高电平
    */
unsigned char read_data(void)
{
    unsigned char i;
    unsigned char val = 0;

    for(i=0;i<8;i++){
        s3c_gpio_cfgpin(ds18b20_io,S3C_GPIO_SFN(1));
        gpio_set_value(ds18b20_io,0);
        udelay(1);
        val >>=1;
        gpio_set_value(ds18b20_io,1);
        s3c_gpio_cfgpin(ds18b20_io,S3C_GPIO_SFN(0));
        udelay(1);

        /*若总线在我们设它为低电平之后若1微秒之内变为高
          则认为从DS18B20处收到一个“1”信号
          因此把byte的D7为置“1”   */
        if(gpio_get_value(ds18b20_io))
            val = val | 0x80;
        udelay(60);
    }
    return val;
}

/*打开文件函数*/
static int ds18b20_open(struct inode *inide,struct file *flip)
{
    int ret;
    /*申请gpio*/
    ret = gpio_request(ds18b20_io,"DS18B20");
    if(ret<0){
        printk(KERN_EMERG "gpio_request is failed!\n");
        return 1;
    }
    printk(KERN_EMERG "open DS18B20\n");

    return 0;
}

/*读文件函数*/
static ssize_t ds18b20_read(struct file *flip,char __user *buff,size_t count,loff_t *f_ops)
{
    unsigned char buf[2];/*DS18B20将产生的温度数据以两个字节的形式存储到高速暂存器的温度寄存器中*/

    ds18b20_reset();
    udelay(420);
    write_data(0xcc);/*跳过序列号命令*/
    write_data(0x44);/*发送转换命令44H，完成温度测量和AD转换*/
    mdelay(800);
    ds18b20_reset();
    udelay(400);
    write_data(0xcc);
    write_data(0xbe);/*发送读取命令,从0位到第9位*/
    buf[0] = read_data();/*读取低位温度*/
    buf[1] = read_data();/*读取高位温度*/
    copy_to_user(buff,buf,sizeof(buf));/*传输数据到用户空间*/

    return 0;
}

static int ds18b20_release(struct inode *inode, struct file *filp)
{
    printk(KERN_EMERG "ds18b20_release is success!\n");

    return 0;
}

static struct file_operations ds18b20_ops={
    .owner = THIS_MODULE,
    .open = ds18b20_open,
    .read = ds18b20_read,
};
static int __init first_init(void)
{
    cdev_init(&cdev,&ds18b20_ops);/*初始化cdev结构*/

    alloc_chrdev_region(&devno, 0 , 1 , "ds18b20");
    cdev_add(&cdev, devno, 1);

    return 0;
}
static void __exit first_exit()
{
    cdev_del(&cdev); /*注销设备*/
    unregister_chrdev_region(devno,1);
}
module_init(first_init);
module_exit(first_exit);
MODULE_LICENSE("Dual BSD/GPL");








