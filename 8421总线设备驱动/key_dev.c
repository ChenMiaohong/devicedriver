#include<linux/module.h>
#include<linux/init.h>
#include<linux/platform_device.h>
#define GPXCON  0x11000C20
#include<linux/interrupt.h>
#include<linux/irq.h>
#define GPXCON 0x11000C20
#define GPXDAT 0x11000C24

MODULE_LICENSE("GPL");
struct resource key_resource[]={
	[0]= {
		.start = GPXCON,
		.end = GPXCON+8,
		.flags = IORESOURCE_MEM,		
	},
	[1] = {
		.start = IRQ_EINT(9),
		.end = IRQ_EINT(10),
		.flags = IORESOURCE_IRQ,

	},
};
struct platform_device key_device = {
	.name = "my-key",
	.id = 0,
	.num_resources = 2,
	.resource = key_resource,


};
int keydev_init()
{
	platform_device_register(&key_device);


}
void keydev_exit()
{
	platform_device_unregister(&key_device);


}
module_init(keydev_init);
module_exit(keydev_exit);
