obj-m += MPU.o

CROSS=/home/desd/beaglebone/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-
KDIR= /home/desd/beaglebone/bb-kernel/KERNEL/


PWD := $(shell pwd)


all:
	make -C $(KDIR) ARCH=arm M=$(PWD) CROSS_COMPILE=$(CROSS) modules
clean:
	make -C $(KDIR) ARCH=arm M=$(PWD) CROSS_COMPILE=$(CROSS) clean










