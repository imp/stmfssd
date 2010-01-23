#!/usr/bin/python2.6
#
# Copyright 2010 Grigale Ltd. All right reserved.
# Use is subject to license terms.
#
import fcntl
import optparse
import os

STMFSSDADMDEV = "/devices/pseudo/stmf_ssd@0:admin"

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
        fcntl.ioctl(self.ctldev, op, arg, mutate_flag)
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
