#obj-$(CONFIG_HELLO_DRIVER)    += hello.o

obj-m := kernel_ex.o


KERNEL=$(shell uname -r)


all:
	make -C /lib/modules/$(KERNEL)/build M=$(PWD) modules


clean:
	make -C /lib/modules/$(KERNEL)/build M=$(PWD) clean