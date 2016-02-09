############################################################################
# FILE   : makefile
# AUTHOR : Peter C. Chapin <PChapin@vtc.vsc.edu>
#
# This is the makefile for the Y project.
############################################################################

!include ..\..\master.mif

.cpp:     ..\..
.cpp.obj: .AUTODEPEND
    wpp $(cpp_options) -DSCR_DIRECT /ml /bt=dos $[*.cpp
