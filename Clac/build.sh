#!/bin/bash
#
# This script builds clac by first running 'make' in the two subprojects. Why can't this be done
# with the make utility itself? In general use of recursive make has issues and should probably
# be avoided. See http://miller.emu.id.au/pmiller/books/rmch/.
#
# This project is simple enough that the issues described in the above article are not
# significant here. Still, it seems appropriate to manage the meta-dependencies separately to
# avoid any appearance of them being done automatically.
#

(cd ClacEntity; make)
(cd ClacEngine; make)
make
