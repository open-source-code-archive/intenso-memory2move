# Makefile for udev extra invoked from the udev main Makefile
#
# Copyright (C) 2004-2005 Kay Sievers <kay.sievers@vrfy.org>
#
# Released under the GNU General Public License, version 2.
#

PROG = edd_id
OBJ =
HEADERS =
GEN_HEADERS =
MAN_PAGES =

prefix =
etcdir =	${prefix}/etc
sbindir =	${prefix}/sbin
usrbindir =	${prefix}/usr/bin
usrsbindir =	${prefix}/usr/sbin
libudevdir =	${prefix}/lib/udev
mandir =	${prefix}/usr/share/man
configdir =	${etcdir}/udev/

INSTALL = install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA  = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL}

all: $(PROG) $(MAN_PAGES)
.PHONY: all
.DEFAULT: all

%.o: %.c $(GEN_HEADERS)
	$(E) "  CC      " $@
	$(Q) $(CC) -c $(CFLAGS) $< -o $@

$(PROG): %: $(HEADERS) %.o $(OBJS)
	$(E) "  LD      " $@
	$(Q) $(LD) $(LDFLAGS) $@.o $(OBJS) -o $@ $(LIBUDEV) $(LIB_OBJS)

# man pages
%.8: %.xml
	$(E) "  XMLTO   " $@
	$(Q) xmlto man $?
.PRECIOUS: %.8

clean:
	$(E) "  CLEAN   "
	$(Q) rm -f $(PROG) $(OBJS) $(GEN_HEADERS)
.PHONY: clean

install-bin: all
	$(INSTALL) -d $(DESTDIR)$(libudevdir)
	$(INSTALL_PROGRAM) $(PROG) $(DESTDIR)$(libudevdir)/$(PROG)
	$(INSTALL) -d $(DESTDIR)$(libudevdir)/rules.d/
	$(INSTALL_DATA) 61-persistent-storage-edd.rules $(DESTDIR)$(libudevdir)/rules.d/61-persistent-storage-edd.rules
.PHONY: install-bin

uninstall-bin:
	- rm $(DESTDIR)$(libudevdir)/$(PROG)
	- rm $(DESTDIR)$(libudevdir)/rules.d/61-persistent-storage-edd.rules
.PHONY: uninstall-bin

install-man:
	$(INSTALL) -d $(DESTDIR)$(mandir)/man8
	$(INSTALL_DATA) $(PROG).8 $(DESTDIR)$(mandir)/man8/$(PROG).8
.PHONY: install-man

uninstall-man:
	-rm -f $(DESTDIR)$(mandir)/man8/$(PROG).8
.PHONY: uninstall-man

install-config:
	@echo "no config file to install"
.PHONY: install-config

