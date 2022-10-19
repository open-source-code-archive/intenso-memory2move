all:
	arm-linux-gcc -o libexfat/cluster.o -c -Wall -O2 -ggdb -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -Ilibexfat libexfat/cluster.c
	arm-linux-gcc -o libexfat/io.o -c -Wall -O2 -ggdb -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -Ilibexfat libexfat/io.c
	arm-linux-gcc -o libexfat/log.o -c -Wall -O2 -ggdb -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -Ilibexfat libexfat/log.c
	arm-linux-gcc -o libexfat/lookup.o -c -Wall -O2 -ggdb -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -Ilibexfat libexfat/lookup.c
	arm-linux-gcc -o libexfat/mount.o -c -Wall -O2 -ggdb -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -Ilibexfat libexfat/mount.c
	arm-linux-gcc -o libexfat/node.o -c -Wall -O2 -ggdb -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -Ilibexfat libexfat/node.c
	arm-linux-gcc -o libexfat/time.o -c -Wall -O2 -ggdb -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -Ilibexfat libexfat/time.c
	arm-linux-gcc -o libexfat/utf.o -c -Wall -O2 -ggdb -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -Ilibexfat libexfat/utf.c
	arm-linux-gcc -o libexfat/utils.o -c -Wall -O2 -ggdb -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -Ilibexfat libexfat/utils.c
	
	arm-linux-gcc -o fuse/main.o -c -Wall -O2 -ggdb -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -Ilibexfat fuse/main.c
	arm-linux-gcc -o fuse/mount.exfat-fuse fuse/main.o -lpthread -lexfat -lfuse
	arm-linux-strip fuse/mount.exfat-fuse
		
	arm-linux-gcc -o dump/main.o -c -Wall -O2 -ggdb -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -Ilibexfat dump/main.c
	arm-linux-gcc -o dump/dumpexfat dump/main.o -lexfat
	arm-linux-strip dump/dumpexfat
		
	arm-linux-gcc -o fsck/main.o -c -Wall -O2 -ggdb -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -Ilibexfat fsck/main.c
	arm-linux-gcc -o fsck/exfatfsck fsck/main.o -lexfat
	arm-linux-strip fsck/exfatfsck
		
	arm-linux-gcc -o mkfs/cbm.o -c -Wall -O2 -ggdb -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -Ilibexfat mkfs/cbm.c
	arm-linux-gcc -o mkfs/fat.o -c -Wall -O2 -ggdb -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -Ilibexfat mkfs/fat.c
	arm-linux-gcc -o mkfs/main.o -c -Wall -O2 -ggdb -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -Ilibexfat mkfs/main.c
	arm-linux-gcc -o mkfs/rootdir.o -c -Wall -O2 -ggdb -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -Ilibexfat mkfs/rootdir.c
	arm-linux-gcc -o mkfs/uct.o -c -Wall -O2 -ggdb -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -Ilibexfat mkfs/uct.c
	arm-linux-gcc -o mkfs/vbr.o -c -Wall -O2 -ggdb -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -Ilibexfat mkfs/vbr.c
	arm-linux-gcc -o mkfs/mkexfatfs mkfs/cbm.o mkfs/fat.o mkfs/main.o mkfs/rootdir.o mkfs/uct.o mkfs/vbr.o -lexfat
	arm-linux-strip mkfs/mkexfatfs
		
	arm-linux-gcc -o label/main.o -c -Wall -O2 -ggdb -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -Ilibexfat label/main.c
	arm-linux-gcc -o label/exfatlabel label/main.o -Llibexfat -lexfat
	arm-linux-strip label/exfatlabel

install:all
	cp fuse/mount.exfat-fuse dump/dumpexfat fsck/exfatfsck mkfs/mkexfatfs label/exfatlabel /opt/toolchain/gcc-4.2.4-uClibc-0.9.29-toolchain/usr/bin/ -arf
