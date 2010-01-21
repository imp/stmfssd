#
# Copyright 2010 Grigale Ltd. All rights reserved.
# Use is subject to license terms.
#
.KEEP_STATE:

DATE_CMD	= /bin/date +%Y.%m.%d
REV		= $(DATE_CMD:sh)
HEAD_CMD	= git show-ref --head --hash --abbrev HEAD
HEAD		= $(HEAD_CMD:sh)
VERSION		= $(HEAD),REV=$(REV)
CSTYLE		= /opt/onbld/bin/cstyle
TARGET		= stmf_ssd
TARGET32	= 32/$(TARGET)
TARGET64	= 64/$(TARGET)
SRCS		= \
	stmf_ssd.c \
	ssc.c \
	ssc_cmd_table.c \
	ssc_test_unit_ready.c \
	ssc_erase.c \
	ssc_request_sense.c \
	ssc_inquiry.c	\
	ssc_read.c	\
	ssc_write.c

OBJS32		= $(SRCS:%.c=32/%.o)
OBJS64		= $(SRCS:%.c=64/%.o)

MACH_32		= -m32
MACH_64		= -m64 -xmodel=kernel
C99MODE		= -xc99=%all
CFLAGS		= -v -D_KERNEL $(C99MODE) $(INCLUDES)
LDFLAGS		= -dy -Ndrv/stmf

CTFCONVERT_O	= true

all: $(TARGET32) $(TARGET64)

$(TARGET32): $(OBJS32)
	$(LD) -r -o $(TARGET32) $(LDFLAGS) $(OBJS32)

$(TARGET64): $(OBJS64)
	$(LD) -r -o $(TARGET64) $(LDFLAGS) $(OBJS64)

32 64:
	mkdir $@

32/%.o: 32 %.c
	$(COMPILE.c) $(MACH_32) -o $@ $<

64/%.o: 64 %.c
	$(COMPILE.c) $(MACH_64) -o $@ $<

cstyle:
	$(CSTYLE) -chpPv $(SRCS)
clean:
	$(RM) $(TARGET32) $(OBJS32) $(TARGET64) $(OBJS64)
	$(RM) -r 32 64
	$(RM) -r packages

pkg:	$(TARGET32) $(TARGET64)
	-[ ! -d packages ] && mkdir packages
	pkgmk -o -d packages -v $(VERSION)
