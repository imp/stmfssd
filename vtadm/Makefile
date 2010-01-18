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
TARGET		= vtadm
SRCS		= vtadm.c

OBJS		= $(SRCS:.c=.o)

MACH_32		= -m32
C99MODE		= -xc99=%all
CFLAGS		= -v $(C99MODE) $(INCLUDES)
LDFLAGS		= 

CTFCONVERT_O	= true

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(LDFLAGS) $(OBJS)

cstyle:
	$(CSTYLE) -chpPv $(SRCS)
clean:
	$(RM) $(TARGET) $(OBJS)
