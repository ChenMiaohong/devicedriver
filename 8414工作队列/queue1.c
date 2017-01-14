#include<linux/init.h>
#include<linux/module.h>
#include<linux/slab.h>

struct work_struct *work1; 
struct work_struct *work2;
static void work1_fun(struct work_struct *work)
{


	printk("this is work1\n");
}
static void work2_fun(struct work_struct *work)
{
	printk("this is work2\n");

}
MODULE_LICENSE("GPL");

int init_queue(void)
{
	//创建工作队列
	//my_wq = create_workqueue("mywq");

	//创建工作
	work1 = kmalloc(sizeof(struct work_struct),GFP_KERNEL);
	INIT_WORK(work1,work1_fun);
	//挂载工作
	//queue_work(my_wq,work1);
	schedule_work(work1);
	//创建工作
	work2 = kmalloc(sizeof(struct work_struct),GFP_KERNEL);
	INIT_WORK(work2,work2_fun);
	//挂载工作
	//queue_work(my_wq,work2);
	schedule_work(work2);
}

void clean_queue(void)
{


}
module_init(init_queue);
module_exit(clean_queue);
