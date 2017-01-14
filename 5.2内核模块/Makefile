obj-m := helloworld.o
KDIR := /home/topeet/Android/iTop4412_Kernel_3.0
all:
    make -C $(KDIR) M=$(PWD) modules
clean:
    rm -f *.ko *.o *.mod.o *.mod.c *.symvers *.order

	

