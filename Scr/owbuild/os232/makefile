############################################################################
# FILE   : makefile
# AUTHOR : Peter C. Chapin <PChapin@vtc.vsc.edu>
#
# This is the makefile for the scr library.
############################################################################

!include ..\..\master.mif

.cpp:     ..\..
.cpp.obj: .AUTODEPEND
    wpp386 $(cpp_options) -DSCR_DIRECT /bt=os2 $[*.cpp
