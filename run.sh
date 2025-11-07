#!/bin/sh
make clean
rm -rf config.status config.log autom4te.cache configure Makefile 
rm -rf lib share bin include
autoconf
#./configure --prefix=`pwd` --exec-prefix=`pwd` --enable-gcc --enable-threads --enable-symbols --with-tcl=/usr/lib64
./configure --enable-gcc --enable-threads --enable-symbols --with-tcl=/usr/lib64
make
sudo make install
man -Hcat tcl-snmptools > index.html
