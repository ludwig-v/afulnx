ifndef KERNEL
KERNEL = /lib/modules/$(shell uname -r)/build
endif
ifndef OUTPUT
OUTPUT = ../amifldrv_mod
endif
PATH_SHIPPED = ..
OUTPUT_SHIPPED = $(PATH_SHIPPED)/amifldrv.o_shipped

DEFAULT_CC			:= /usr/bin/gcc
ERROR_LOG			:= /tmp/afudrv.gen_error
EXTRA_CFLAGS		:= -Wall -Wstrict-prototypes -Wno-int-to-pointer-cast -O2 -fno-strict-aliasing -DBUILD_AMIFLDRV_MOD
obj-m				+= amifldrv_mod.o amifldrv_impl.o
amifldrv_impl-objs	:= amimemdrv.o amiiodrv.o amiother.o amismmbufferv1.o amismmbufferv2.o
amifldrv_mod-objs	:= amifldrv.o amimemdrv.o amiiodrv.o amiother.o amismmbufferv1.o amismmbufferv2.o

.PHONY : default gendrv clean

default:
	@if [ -d $(KERNEL) ] && [ -f $(DEFAULT_CC) ]; then \
		make -C $(KERNEL) M=$(PWD) modules 2>/dev/null>/dev/null; \
		mv amifldrv_mod.ko $(OUTPUT); \
	fi
	@if [ ! -d $(KERNEL) ]; then \
		echo "Error: Cannot locate kernel source." > $(ERROR_LOG); \
	fi
	@if [ ! -f $(DEFAULT_CC) ]; then \
		echo "Error: gcc complier did not exist." >> $(ERROR_LOG); \
	fi

gendrv:
	@if [ -d $(KERNEL) ] && [ -f $(DEFAULT_CC) ]; then \
		make -C $(KERNEL) M=$(PWD) modules 2>/dev/null>/dev/null; \
		mv amifldrv_impl.o $(OUTPUT_SHIPPED); \
		mv .amifldrv_impl.ko.cmd $(OUTPUT_SHIPPED).cmd; \
		mv amifldrv.c $(PATH_SHIPPED)/amiwrap.c; \
		mv amifldrv.h $(PATH_SHIPPED); \
		mv amifldrvdefs.h $(PATH_SHIPPED); \
		mv Makefile.Shipped $(PATH_SHIPPED)/Makefile; \
	fi
	@if [ ! -d $(KERNEL) ]; then \
		echo "Error: Cannot locate kernel source." > $(ERROR_LOG); \
	fi
	@if [ ! -f $(DEFAULT_CC) ]; then \
		echo "Error: gcc complier did not exist." >> $(ERROR_LOG); \
	fi

clean:
	rm -f *.*~
	rm -f test
	rm -f .*cmd
	rm -f *.o
	rm -f *mod*
	rm -f *.ko
	rm -f amifldrv.o_shipped
	rm -rf .tmp_versions/
	rm -f Module.*
