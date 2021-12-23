############################################################################
# FILE   : makefile
# AUTHOR : Peter C. Chapin <chapinp@acm.org>
#
# This is the makefile for the scr library.
############################################################################

!include ../../master.mif

.cpp:     ../..
.cpp.obj: .AUTODEPEND
    wpp386 $(cpp_options) -DSCR_DIRECT -bt=linux $[*.cpp
