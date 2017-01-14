#include<linux/module.h>
#include<linux/init.h>
#include<linux/cdev.h>
#include<linux/fs.h>
#include<asm/uaccess.h>

struct cdev mdev;
dev_t devno;
int regist_dev0[5];
int regist_dev1[5];
loff_t my_llseek(struct file *filp, loff_t offset, int whenv)
{	
	loff_t new_pos;
	switch(whenv)
	{
		case SEEK_SET:
			new_pos = offset;break;
		case SEEK_CUR:
			new_pos = filp->f_pos+offset;break;
		case SEEK_END:
			new_pos = 5*sizeof(int)+filp->f_pos;break;
		default:
			return -ENODEV;
	}
	filp->f_pos = new_pos;
	return new_pos;

}
ssize_t my_read(struct file *filp, char __user *buf, size_t count, loff_t *offp)
{
	int *regist_base = filp->private_data;
	copy_to_user(buf, regist_base+*offp, count);
	
	filp->f_pos +=count;
	return  count;

}
ssize_t my_write (struct file * filp, const char __user *buf, size_t count, loff_t *offp)
{
	int *regist_base = filp->private_data;
    copy_from_user(regist_base+*offp, buf, count);

    filp->f_pos +=count;
    return  count;


}
int my_open (struct inode *node, struct file *filp)
{
	int num;
	
	num = MINOR(devno);
	if(num==0)
		filp->private_data = regist_dev0;
	if(num==1)
    	filp->private_data = regist_dev1;
	return 0;
}	
int my_close(struct inode *node, struct file *filp)
{


	return 0;

}

const struct file_operations myops={
	.llseek = my_llseek,
	.read = my_read,
	.write = my_write,
	.open = my_open,
	.release = my_close, 
};
int mydev_init()
{
	cdev_init(&mdev,&myops);
	alloc_chrdev_region(&devno, 0, 2,"mydev");
	cdev_add(&mdev,devno, 2);

}
void mydev_exit()
{
	cdev_del(&mdev);	
	unregister_chrdev_region(devno,2);

}

module_init(mydev_init);
module_exit(mydev_exit);
