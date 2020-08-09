obj-m := producer.o consumer.o
# obj-m += $(addsuffix .o, $(notdir $(basename $(wildcard $(BR2_EXTERNAL_KERNEL_MODULE_PATH)/*.c))))
# ccflags-y := -DDEBUG -g -std=gnu99 -Wno-declaration-after-statement

KVERSION := $(shell uname -r)
KDIR := /lib/modules/$(KVERSION)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules 
	# $(MAKE) -C $(LINUX_DIR) M=$(PWD) modules 

clean: 
	$(MAKE) -C $(KDIR) M=$(PWD) clean 
	# $(MAKE) -C $(LINUX_DIR) M=$(PWD) clean 

install:
	$(MAKE) -C $(KDIR) M=$(PWD) modules_install