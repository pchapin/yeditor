
Overview of Y
=============

This file has several sections.

 1. Introduction.
 2. Prerequisites.
 3. Compiling.
 4. Building the Documentation.
 5. Testing.
 6. Analysis.
 7. Adding a Module.

See also `TODO.md` for a roadmap of what needs to be done on Y.

Introduction
------------

Greetings!

This is the code base for the Y text editor, version 2.0. Although currently unfinished, it is
my intention to release the source and documentation of Y under the terms of the GPL. Please
note that I disclaim all warranties implied or otherwise. All the necessary, precise legal
language will be added later.

Y is a simple, but effective cross platform editor. The supported platforms are Win32 consoles
and Linux terminals (with some testing also on Cygwin). Some level of support also exists for
OS/2 text mode and DOS 16 bit targets. However, this support relies on the Open Watcom compiler
which, unfortunately, is too far behind the latest C++ standard to compile Y at this time.
However, it is my hope that Open Watcom will be updated at some point allowing these other
targets to be used again.

I invite you to play around with Y. Compile it, use it, and let me know what you think. If you
find or fix a problem, send me email. I can be reached at PChapin@vtc.vsc.edu.

There is some sketchy documentation about Y in the doc directory. Some of that document is
outdated and is in serious need of editing. Fixing the documentation is a work in progress.

Peter C. Chapin


Prerequisites
-------------

Y depends on a library of various utility components called Spica. This library can also be
found on GitHub here

    https://github.com/pchapin/spica

In addition Y depends on a cross platform screen handling library called Scr. The Scr library
can be found on GitHub here

    https://github.com/pchapin/scr

You should install Spica and Scr (for example by cloning the GitHub repositories) in sibling
folders of your Y working copy. The build control files assume this layout. Note that Spica and
Scr need to be built separately (unless you are using Visual Studio or Code::Blocks). Consult
the documentation for these other libraries for more information on how to build them. However,
the build systems used for Spica and Scr are largely similar to the build systems used for Y so
familiarity with the way one project is organized should make it easy to understand the others.


Compiling
---------

### g++

G++ targetting Unix-like systems is the primary compiler and platform for Y. Makefiles are
provided that build each component of the system. In general you can just enter a folder and
type `make` to build whatever entity is contained in that folder.

Currently it is necessary to build the Spica and Scr libraries first manually (using Makefiles)
before you build Y itself. Y should probably be built before you build the test programs but
that is not strictly necessary at this time.

### Code::Blocks

The Makefiles mentioned above do not presume the use of any particular IDE. However,
Code::Blocks project files are also provided for users who prefer using an IDE of some sort. The
project files assume you are using the g++ compiler.

Note that the Code::Blocks project files are not used as much as the Makefiles so they may be in
need of some maintenance. In particular you may need to add/remove some files from the project
definitions.

To build Y with Code::Blocks load src/Y.workspace into Code::Blocks and build the Debug
and Release configurations. This workspace includes the Spica and Scr projects as well.

### Visual C++

Solution and project files for Visual C++ version 12.0 (Visual Studio 2013) are also provided.
Although development on Windows tends to be less frequent than on Linux, the Windows build is
reasonably well maintained and should always be working.

To build Y with Visual Studio load `Y.sln`. This solution file contains references to all
required projects (including Spica and Scr). Thus building this solution will build everything.

### Open Watcom

Open Watcom is a cross platform compiler that supports Windows, OS/2, DOS, and Linux. In theory
it can be used to build Y for all of these systems regardless of your development platform.
Unfortunately Open Watcom does not currently (February 2015) support C++ 2011 to any significant
degree. Thus it can't be used to build Y at this time. However the project files for Open Watcom
are retained in the hope that some future version of that compiler will be able to compile Y
again. Open Watcom was once the primary compiler for Y development and continued support for it,
at least on paper, should be retained out of respect if nothing else.

To build Y with Open Watcom follow this procedure.

+ In the `src\owbuild` folder, change into the folder dos16, os232, linux, or win32 depending on
  which platform(s) you want to target. Issue a `wmake` command in that folder to build the
  appropriate platform specific version of Y. Note that you might have to adjust the INCLUDE
  environment variable if you switch between OS/2 and Win32 development.

You will need to build the supporting Spica and Scr libraries using Open Watcom as well. Look
for owbuild folders in those code bases; the build system is organized in a similar way.

You may find project files for the Open Watcom IDE (*.wpj and *.tgt). These files are likely to
be very outdated.

### clang++

At one point compilation of Y with the clang++ compiler was supported via Makefiles. That
support was dropped but it could (should?) be reinstated. The Makefile support for g++ should be
retained, however.


Building the Documentation
--------------------------

The primary documentation for Y is in the `doc` folder. It is in LaTeX format. The master
document is `y20.tex`. This is the document to build with LaTeX; doing so will pull in all the
other documents in that folder as chapters. Note that the file `y11.tex` is a section of the
document related to Y version 1.1 compatibility. It is not a stand-alone document.

The documentation contains both tutorial information and a complete Y reference manual. Be aware
that some of this document is outdated and refers to Y version 1.1. Updating the documentation
is a work in progress.

Y's internals are documented using the [Doxygen](http://www.doxygen.org/) system. If you have a
suitable version of Doxygen installed you can run the tool in the `src` folder. This will create
an `internal` folder under the `doc` folder where the HTML formatted documentation can be found.

The Spica and Scr projects are documented separately but have a similar organization. See those
projects for more information.


Testing
-------

Y has a unit test program in the `src\check` folder. It is automatically built by the Visual
Studio solution but it is (at the time of this writing) not well supported by the other build
systems. It is also extremely minimal. Enhancing Y's unit tests is a work in progress.

Note that the unit test program makes use of a simple unit test framework for C++ called UTM
(Unit Test Manager). This framework generates XML reports of test results and is intended to be
used with a continuous integration server. UTM is a subproject of Spica; see the Spica project
for more information.

The Spica and Scr projects have their own testing system. See those projects for more
information.


Analysis
--------

The Y code base contains configuration information for the commercial C++ static analysis tool
[PC-Lint](http://www.gimpel.com/html/pcl.htm). In the `src` folder there is a batch file
`LIN.bat` that invokes PC-Lint using appropriately defined configuration files. The PC-Lint
configuration is part of this code base. The file `FILES.lnt` contains a list of files that have
approximately passed PC-Lint's analysis. The goal is to eventually have all source files in Y
listed there.

The use of other analysis tools is encouraged. Possibilities include valgrind and/or clang's
static analyzer. The use of the static analysis tools provided with Visual Studio should be
investigated.


Adding a Module
---------------

If a new module (source file) is added to Y (or an existing file is removed), there are a number
of steps required to put all the build control systems in order. It might be appropriate to
create a script at some point to automate this process.

### g++

1. Edit the depend.dep file to add/remove the file to/from the list. Rerun the `depend` tool
   (see my [Tools project on GitHub](https://github.com/pchapin/tools)) via the shell script
   `depend.sh`. This builds dependency lists for the Makefile. Replace the existing dependency
   list with the contents of `depend.out`.

2. Update the list of source files in the Makefile.

### Code::Blocks

1. Edit the appropriate Code::Blocks project file to add/remove the file to/from the list. This
   is probably best done inside Code::Blocks but it could be done with a text editor since
   Code::Blocks project files use a (relatively) simple XML format.

### Visual Studio

1. Edit the appropriate Visual Studio project file to add/remove the file to/from the list. This
   is probably best done inside Visual Studio. It is possible to edit Visual Studio project
   files with a text editor but they have a more complex XML format and so this approach is not
   recommended unless you are very familiar with Visual Studio.

### Open Watcom

1. Edit `src\master.mif` to add/remove the file to/from the list of OBJS.

2. Edit `src\Y.cds` to add/remove the file to/from the list of files.

These changes will make the file known to `wmake` during Open Watcom builds (owbuild). The Open
Watcom IDE is not very well supported. Any files related to that product are likely to be
outdated. You are invited (encouraged) to update them if you desire.
