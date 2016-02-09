############################################################################
# FILE   : makefile
# AUTHOR : (C) Peter C. Chapin <PChapin@vtc.vsc.edu>
#
# This is the makefile for the Y project.
############################################################################

!include ../../master.mif

.cpp:     ../..
.cpp.obj: .AUTODEPEND
    wpp386 $(cpp_options) -DSCR_DIRECT -bt=linux $[*.cpp
