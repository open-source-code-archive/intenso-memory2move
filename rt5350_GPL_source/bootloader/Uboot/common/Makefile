#
# (C) Copyright 2004
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

LIB	= libcommon.a

AOBJS	=

COBJS	= main.o cmd_bdinfo.o cmd_boot.o cmd_bootm.o cmd_console.o \
	cmd_load.o cmd_misc.o cmd_net.o \
	cmd_nvedit.o command.o console.o devices.o dlmalloc.o \
	env_common.o exports.o lists.o ui_menu.o vs_cfg.o

#ifdef RALINK_USB
COBJS += usb.o usb_storage.o cmd_usb.o cmd_fat.o
#endif

OBJS	= $(AOBJS) $(COBJS)

ifeq ($(CFG_ENV_IS), IN_FLASH)
OBJS	+= cmd_flash.o cmd_mem.o env_flash.o flash.o
endif
ifeq ($(CFG_ENV_IS), IN_SPI)
OBJS	+= env_spi.o cmd_mem.o
endif
ifeq ($(CFG_ENV_IS), IN_NAND)
OBJS	+= env_nand.o cmd_mem.o
endif

CPPFLAGS += -I..

all:	$(LIB) $(AOBJS)

$(LIB): .depend $(OBJS)
	$(AR) crv $@ $(OBJS)

environment.o: environment.c ../tools/envcrc
	$(CC) $(AFLAGS) -Wa,--no-warn \
		-DENV_CRC=$(shell ../tools/envcrc) \
		-c -o $@ environment.c

../tools/envcrc:
	$(MAKE) -C ../tools

#########################################################################

.depend:	Makefile $(AOBJS:.o=.S) $(COBJS:.o=.c)
		$(CC) -M $(CFLAGS) $(AOBJS:.o=.S) $(COBJS:.o=.c) > $@

sinclude .depend

#########################################################################
