Clac
====

A programmable calculator based on the HP48 series of calculators.

The primary implementation is in C++ and has three components. The ClacEntity library is a
library of calculator (mostly numeric) data types. The ClacEngine library provides the execution
engine that interpretes calculator input and coordinates the computations. The top level code
defines the user interface. In theory other programs could make use of Clac's entity library and
execution engine while using a different user interface.

In addition there is a completely independent implementation of Clac written in Java in the
jaClac folder. This implementation does not currently share any code with the C++ version. See
the README in that folder for more information.

Finally there is a lightweight version of Clac written in Java in the Clacky folder. This
version is independent of both the C++ version and of jaClac. It is suitable for study by first
time programming students using Java.

The C++ version of Clac can be built with Visual Studio on Windows and g++ (via Code::Blocks or
Makefiles) on Unix. It is also possible that some or all of Clac may be buildable with Open
Watcom.

Peter C. Chapin  
PChapin@vtc.vsc.edu
