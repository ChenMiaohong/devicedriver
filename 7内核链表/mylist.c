#include<linux/init.h>
#include<linux/module.h>
struct score{
    int num;
    int english;
    int math;
    struct list_head list;
};
struct list_head score_head;
struct list_head *pos;
struct score stu1,stu2,stu3;
struct score *tmp;
int mylist_init()
{
    INIT_LIST_HEAD(&score_head);
    stu1.num=1;
    stu1.english=90;
    stu1.math=90;
    list_add_tail(&(stu1.list),&score_head);

    stu2.num=2;
    stu2.english=91;
    stu2.math=92;
    list_add_tail(&(stu2.list),&score_head);

    stu3.num=3;
    stu3.english=94;
    stu3.math=95;
    list_add_tail(&(stu3.list),&score_head);

    list_for_each(pos,&score_head)
    {
        tmp = list_entry(pos,struct score,list);
        printk("NO is %d,english is %d,math is %d\n",tmp->num,tmp->english,tmp->math);



    }

}
void mylist_exit()
{
    list_del(&(stu1.list));
    list_del(&(stu1.list));

}
module_init(mylist_init);
module_exit(mylist_exit);

~                         