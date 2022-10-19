#!/bin/sh
make clean
make
arm-linux-strip udevd udevadm

