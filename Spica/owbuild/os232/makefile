############################################################################
# FILE   : makefile
# AUTHOR : Peter C. Chapin <PChapin@vtc.vsc.edu>
#
# This is the makefile for the SpicaCpp library.
############################################################################

!include ..\..\master.mif

.cpp:     ..\..
.cpp.obj: .AUTODEPEND
    wpp386 $(cpp_options) /bt=os2 $[*.cpp
