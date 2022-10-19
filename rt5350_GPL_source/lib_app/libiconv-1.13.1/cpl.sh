CURPATH=`pwd`
make clean
CC=arm-linux-gcc ./configure --prefix=${CURPATH}/install --build=i386-linux --host=arm-linux --enable-static --enable-shared --without-random --with-gnu-ld

