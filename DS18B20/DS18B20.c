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
/*dev_t�ȵĶ���*/
#include <linux/types.h>
/*����copy_to_user��copy_from_user��ͷ�ļ�*/
#include <asm/uaccess.h>
/*�����Ĵ�������������ͷ�ļ�*/
#include <asm/io.h>
/*��ʱ����*/
#include <linux/delay.h>
/*��������device_create �ṹ��class��ͷ�ļ�*/
#include <linux/device.h>
/*������Ϻ������Ҫ��ͷ�ļ�*/
#include <linux/errno.h>

#define GPA0CON 0x1140_0000
//GPA0CON�Ĵ����������ַ
#define GPA0DAT 0x1140_0004
//GPA0DAT�Ĵ����������ַ
#define ds18b20_io (EXYNOS4_GPA0(7))
static int *gpa0con;
static int *gpa0dat;
struct cdev cdev;
dev_t devno;
void ds18b20_reset(void)
{
    int ret;

    s3c_gpio_cfgpin(ds18b20_io,S3C_GPIO_SFN(1));/*���ó����*/
    gpio_set_value(ds18b20_io,1);/* ��18B20����һ�������أ������ָߵ�ƽ״̬Լ100΢��*/
    udelay(100);
    gpio_set_value(ds18b20_io,0);/*��18B20����һ���½��أ������ֵ͵�ƽ״̬Լ600΢��*/
    udelay(600);
    gpio_set_value(ds18b20_io,1);/* ��18B20����һ�������أ���ʱ���ͷ�DS18B20����*/
    udelay(100);
    /*���ϲ����Ǹ�DS18B20һ����λ����*/

    s3c_gpio_cfgpin(ds18b20_io,S3C_GPIO_SFN(0));/*����Ϊ����,���Լ�⵽DS18B20�Ƿ�λ�ɹ�*/
    /*����͵�ƽ����˵��������������������Ӧ��*/
    ret = gpio_get_value(ds18b20_io);/*��ȡio�ϵĵ�ƽ.���������ͷź�����״̬Ϊ�ߵ�ƽ����λʧ��*/
    /*if(!ret){
        printk(KERN_EMERG "ds18b20 init is success!\n");
    }
    else{
        printk(KERN_EMERG "ds18b20 init is failed!\n");
    }*/
}

/*дһ���ֽ�*/
/*д��1��ʱ϶��
   ���������ڵ͵�ƽ1΢�뵽15΢��֮��
   Ȼ���ٱ��������ڸߵ�ƽ15΢�뵽60΢��֮��
   ����״̬: 1΢��ĵ͵�ƽȻ�������ٱ���60΢��ĸߵ�ƽ

д��0��ʱ϶��
    ���������ڵ͵�ƽ15΢�뵽60΢��֮��
    Ȼ���ٱ��������ڸߵ�ƽ1΢�뵽15΢��֮��
    ����״̬: 60΢��ĵ͵�ƽȻ�������ٱ���1΢��ĸߵ�ƽ
    */
void write_data (unsigned char dat)
{
    unsigned char i;

    s3c_gpio_cfgpin(ds18b20_io,S3C_GPIO_SFN(1));/*�˿�����Ϊ���*/
    for(i=0;i<8;i++){
/*������д1����һ��ʼ�������ߵ�ƽ1΢�����ͷ�����Ϊ�ߵ�ƽ��һֱ��д���ڽ�����*/
        gpio_set_value(ds18b20_io,0);
        udelay(1);

        /*�˴�Ϊд"1"*/
        /*��byte������D0λ��1��������������д��1��
         ����д��1��ʱ϶���򣬵�ƽ�ڴ˴���תΪ��*/
        if(((dat)&(0x01))==1)
            gpio_set_value(ds18b20_io,1);
        udelay(80);
        gpio_set_value(ds18b20_io,1);
        udelay(15);
        dat = dat>>1;
    }
    gpio_set_value(ds18b20_io,1);
}

/*��һ���ֽ�*/
/* ����1��ʱ϶��
    ������״̬�����ڵ͵�ƽ״̬1΢�뵽15΢��֮��
    Ȼ�����䵽�ߵ�ƽ״̬�ұ�����15΢�뵽60΢��֮��
    ����Ϊ��DS18B20����һ����1���ź�
    �������: 1΢��ĵ͵�ƽȻ�������ٱ���60΢��ĸߵ�ƽ

����0��ʱ϶��
    ������״̬�����ڵ͵�ƽ״̬15΢�뵽30΢��֮��
    Ȼ�����䵽�ߵ�ƽ״̬�ұ�����15΢�뵽60΢��֮��
    ����Ϊ��DS18B20����һ����0���ź�
    �������: 15΢��ĵ͵�ƽȻ�������ٱ���46΢��ĸߵ�ƽ
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

        /*����������������Ϊ�͵�ƽ֮����1΢��֮�ڱ�Ϊ��
          ����Ϊ��DS18B20���յ�һ����1���ź�
          ��˰�byte��D7Ϊ�á�1��   */
        if(gpio_get_value(ds18b20_io))
            val = val | 0x80;
        udelay(60);
    }
    return val;
}

/*���ļ�����*/
static int ds18b20_open(struct inode *inide,struct file *flip)
{
    int ret;
    /*����gpio*/
    ret = gpio_request(ds18b20_io,"DS18B20");
    if(ret<0){
        printk(KERN_EMERG "gpio_request is failed!\n");
        return 1;
    }
    printk(KERN_EMERG "open DS18B20\n");

    return 0;
}

/*���ļ�����*/
static ssize_t ds18b20_read(struct file *flip,char __user *buff,size_t count,loff_t *f_ops)
{
    unsigned char buf[2];/*DS18B20���������¶������������ֽڵ���ʽ�洢�������ݴ������¶ȼĴ�����*/

    ds18b20_reset();
    udelay(420);
    write_data(0xcc);/*�������к�����*/
    write_data(0x44);/*����ת������44H������¶Ȳ�����ADת��*/
    mdelay(800);
    ds18b20_reset();
    udelay(400);
    write_data(0xcc);
    write_data(0xbe);/*���Ͷ�ȡ����,��0λ����9λ*/
    buf[0] = read_data();/*��ȡ��λ�¶�*/
    buf[1] = read_data();/*��ȡ��λ�¶�*/
    copy_to_user(buff,buf,sizeof(buf));/*�������ݵ��û��ռ�*/

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
    cdev_init(&cdev,&ds18b20_ops);/*��ʼ��cdev�ṹ*/

    alloc_chrdev_region(&devno, 0 , 1 , "ds18b20");
    cdev_add(&cdev, devno, 1);

    return 0;
}
static void __exit first_exit()
{
    cdev_del(&cdev); /*ע���豸*/
    unregister_chrdev_region(devno,1);
}
module_init(first_init);
module_exit(first_exit);
MODULE_LICENSE("Dual BSD/GPL");








