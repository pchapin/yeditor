############################################################################
# FILE   : makefile
# AUTHOR : (C) Peter C. Chapin <chapinp@acm.org>
#
# This is the makefile for the Y project.
############################################################################

!include ../../master.mif

.cpp:     ../..
.cpp.obj: .AUTODEPEND
    wpp386 $(cpp_options) -DSCR_DIRECT -bt=linux $[*.cpp
