#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>

struct workqueue_struct *my_wq;
struct work_struct *work1;
struct work_struct *work2;

MODULE_LICENSE("GPL");

void work1_func(struct work_struct *work)
{
    printk("this is work1->\n");	
}

void work2_func(struct work_struct *work)
{
    printk("this is work2->\n");	
}

int init_que(void)
{	
    //1. 创建工作队列
    my_wq = create_workqueue("my_que");
    	
    //2. 创建工作
    work1 = kmalloc(sizeof(struct work_struct),GFP_KERNEL);
    INIT_WORK(work1, work1_func);
    
    //3. 挂载（提交）工作
    queue_work(my_wq,work1);
   
     
    //2. 创建工作
    work2 = kmalloc(sizeof(struct work_struct),GFP_KERNEL);
    INIT_WORK(work2, work2_func);
    
    //3. 挂载（提交）工作
    queue_work(my_wq,work2);
    	
    return 0;
}

void clean_que()
{
	
}


module_init(init_que);
module_exit(clean_que);
