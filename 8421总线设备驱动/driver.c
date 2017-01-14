#include<linux/module.h>
#include<linux/init.h>
#include<linux/device.h>
#include<linux/kernel.h>

extern struct bus_type my_bus_type;//变量来自于外部
MODULE_LICENSE("GPL");
int my_probe(struct device *dev)
{
	printk("the bus's driver find the device it can handle\n ");
	//实际的硬件设备初始化

	return 0;
}

struct device_driver my_driver = {
	.name = "my_dev",//驱动名称
	.bus = &my_bus_type,//所挂载的总线
	.probe = my_probe,//匹配调用函数

};

int my_driver_init()
{
	int ret;
	ret =driver_register(&my_driver);
	return ret;

}
void my_driver_exit()
{
	driver_unregister(&my_driver);
}

module_init(my_driver_init);
module_exit(my_driver_exit);
