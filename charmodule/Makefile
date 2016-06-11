obj-m := hellochar.o
kernel_path=/usr/src/linux-headers-`uname -r`

all:
	make -C ${kernel_path} M=`pwd` modules

clean:
	make -C ${kernel_path} M=`pwd` clean
install:
	make -C ${kernel_path} M=`pwd` modules_install
