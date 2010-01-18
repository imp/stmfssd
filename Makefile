#
# Copyright 2010 Grigale Ltd. All rights reserved.
# Use is subject to license terms.
#
.KEEP_STATE:

all clean:
	cd stmf_ssd; $(MAKE) $@
	cd vtadm; $(MAKE) $@

pkg: all
	cd pkg; $(MAKE) $@
