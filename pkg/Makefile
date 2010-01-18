#
# Copyright 2010 Grigale Ltd. All rights reserved.
# Use is subject to license terms.
#
.KEEP_STATE:

DATE_CMD	= /bin/date +%Y.%m.%d
REV		= $(DATE_CMD:sh)
HEAD_CMD	= git show-ref --head --hash --abbrev HEAD
HEAD		= $(HEAD_CMD:sh)
VERSION		= 1,REV=$(REV),$(HEAD)

PROTOTYPE	= stmfssd.prototype

all: pkg

pkg:
	-[ ! -d ../packages ] && mkdir ../packages
	pkgmk -o -d ../packages -v $(VERSION) -f $(PROTOTYPE)

clean:
	$(RM) $(TARGET32) $(OBJS32) $(TARGET64) $(OBJS64)
	$(RM) -r 32 64
