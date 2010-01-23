#!/usr/bin/python2.6
#
# Copyright 2010 Grigale Ltd. All right reserved.
# Use is subject to license terms.
#
import fcntl
import optparse
import os
import struct

STMFSSDADMDEV = "/devices/pseudo/stmf_ssd@0:admin"

CMDHEADER = "IIL"
CMDREPLY  = "IIL"
STMFSSD_IOCTL =	0x53534400 # "SSDx"
STMFSSD_GET_VERSION = (STMFSSD_IOCTL | 0)
STMFSSD_LIST_DEV = (STMFSSD_IOCTL | 1)
STMFSSD_CREATE_DEV = (STMFSSD_IOCTL | 2)
STMFSSD_REMOVE_DEV = (STMFSSD_IOCTL | 3)

STMFSSD_ABI_VERSION_0 = 0
STMFSSD_ABI_VERSION_1 = 1
STMFSSD_ABI_VERSION = STMFSSD_ABI_VERSION_1

class VTAdmException(Exception):
    def __init__(self, msg=None):
        self.msg = msg

class StmfSsdAdmin():
    def __init__(self):
        try:
            self.ctldev = os.open(STMFSSDADMDEV, os.O_RDWR)
	except OSError:
            raise VTAdmException("Cannot open stmfssd control device")

    def status(self):
        arg = struct.pack(CMDHEADER, STMFSSD_ABI_VERSION, 0, 0)
        fcntl.ioctl(self.ctldev, STMFSSD_GET_VERSION, arg)
	ver, status, len = struct.unpack(arg, CMDREPLY)
	print "Version %s, Status = %s, len = %s" % (ver, status, len)
	print "Status OK"


def parse():
    usage = "Usage: %prog verb [options]"
    version = "1"
    parser = optparse.OptionParser(usage, version=version)
    parser.add_option("status", dest="status")


def main():
    parse()
    ctldev = StmfssdAdmin()
    ctldev.status()


if __name__ == "__main__":
    try:
        main()
    except VTAdmException as e:
        print e.msg
