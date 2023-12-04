# KERNEL_HEADERS=/lib/modules/$(shell uname -r)/build
KERNEL_HEADERS=/usr/src/linux-headers-$(shell uname -r)

CFILES = driver.c
obj-m := mod_test.o
$(obj-m:.o=-objs) := $(CFILES:.c=.o)

ccflags-y += -std=gnu99 -Wall -Wno-declaration-after-statement
ccflags-y += -DDRIVER_NAME=\"$(obj-m:.o=)\"

all:
	$(MAKE) -C $(KERNEL_HEADERS) M=$(PWD) modules
	-modinfo ./*.ko
clean:
	$(MAKE) -C $(KERNEL_HEADERS) M=$(PWD) clean

.PHONY: rmmod insmod
rmmod: all
	-sudo rmmod $(obj-m:.o=)
	-lsmod | grep $(obj-m:.o=)

insmod: rmmod
	-modprobe $(obj-m:.o=)
	#sudo insmod $(obj-m:.o=.ko) param0=123 param1=2345 param2=3456 param3=7890 verbose=N
	sudo insmod $(obj-m:.o=.ko) param0=123 param1=2345 param2=3456 param3=7890 verbose=Y
	sudo dmesg | tail
	grep "" -r /sys/module/$(obj-m:.o=)/parameters/

test:
	sudo insmod $(obj-m:.o=.ko)
	sudo dmesg | tail
	sudo rmmod $(obj-m:.o=)
	sudo dmesg | tail

