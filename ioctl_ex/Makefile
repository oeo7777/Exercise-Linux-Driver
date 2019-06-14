

obj-m := ioctl_kernel.o


KERNEL=$(shell uname -r)


all:
	make -C /lib/modules/$(KERNEL)/build M=$(PWD) modules


clean:
	make -C /lib/modules/$(KERNEL)/build M=$(PWD) clean