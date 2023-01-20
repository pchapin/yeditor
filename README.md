
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

See also `TODO.md` and the GitHub issues list for this project for a roadmap of what needs to be
done on Y.

Introduction
------------

Greetings!

This is the code base for the Y text editor, version 2.0. Although currently unfinished, it is
my intention to release the source and documentation of Y under the terms of the some
appropriate open source license. Please note that I disclaim all warranties implied or
otherwise. All the necessary, precise legal language will be added later.

Y is a simple, but effective cross platform editor. The supported platforms are Win32 consoles
and Linux terminals (with some testing also on Cygwin and macOS). Some level of support also
once existed for OS/2 text mode and DOS 16 bit targets. However, that support is largely
untested, and maybe considerably broken.

I invite you to play around with Y. Compile it, use it, and let me know what you think. If you
find or fix a problem, send me email or, better yet, open an issue on GitHub or create a pull
request. I can be reached at chapinp@proton.me.

There is some sketchy documentation about Y in the doc directory. Some of that document is
outdated and is in serious need of editing. Fixing the documentation is a work in progress.

Peter Chapin


Prerequisites
-------------

Y depends on a cross platform screen handling library called Scr. This library is part of the Y
repository (in the `scr` folder). However, it is fairly independent of Y and could potentially
be used in other projects. The Scr library can be compiled separately or, if you are using
Visual Studio or Code::Blocks, it is also automatically built during the build of Y itself.

A tool named `depend` is used to compute Makfile dependencies. It is located in my
[tools](https://github.com/pchapin/tools) repository. It is not strictly necessary to use this
tool, but the configuration files used by `depend` are in the Y repository.


Compiling
---------

### g++

G++ targetting Unix-like systems is the primary compiler and platform for Y at this time. Note
that on macOS, `g++` is an alias for `clang++`. Makefiles are provided that build each component
of the system. In general you can just enter a folder and type `make` to build whatever entity
is contained in that folder.

Currently it is necessary to build the Scr library first manually (using Makefiles) before you
build Y itself. Y should probably be built before you build the test programs but that is not
strictly necessary.

### Code::Blocks

The Makefiles mentioned above do not presume the use of any particular IDE. However,
Code::Blocks project files are also provided for users who prefer using an IDE of some sort. The
project files assume you are using the g++ compiler.

Note that the Code::Blocks project files are not used as much as the Makefiles so they may be in
need of some maintenance. In particular, you may need to add/remove some files from the project
definitions.

To build Y with Code::Blocks load src/Y.workspace into Code::Blocks and build the Debug and
Release configurations. This workspace contains references to all required projects (including
Scr and the test programs). Thus building this workspace will build everything.

### Visual C++

Solution and project files for Visual Studio 2022 are also provided. Although development on
Windows tends to be less frequent than on Linux, the Windows build is reasonably well maintained
and should always be working.

To build Y with Visual Studio load `Y.sln`. This solution file contains references to all
required projects (including Scr and the test programs). Thus building this solution will build
everything.

### Open Watcom

Open Watcom is a cross platform compiler that supports Windows, OS/2, DOS, and Linux. In theory
it can be used to build Y for all of these systems regardless of your development platform. At
the current time Y development is limited to C++98, which Open Watcom largely supports. Newer
C++ features can only be used to the extent that Open Watcom supports them.

To build Y with Open Watcom follow this procedure.

+ In the `src\owbuild` folder, change into the folder dos16, os232, linux, or win32 depending on
  which platform(s) you want to target. Issue a `wmake` command in that folder to build the
  appropriate platform specific version of Y. Note that you might have to adjust the INCLUDE
  environment variable depending on if your target platform is different than your build
  platform.

You will need to build the supporting Scr library using Open Watcom as well. Look for owbuild
folders in those code bases; the build system is organized in a similar way.

You may find project files for the Open Watcom IDE (*.wpj and *.tgt). If they exist at all these
files are likely to be very outdated.

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


Testing
-------

Y has a unit test program in the `src\check` folder. It is automatically built by the Visual
Studio solution and the CodeBlocks workspace, but it is (at the time of this writing) not well
supported by the other build systems. It is also extremely minimal. Enhancing Y's unit tests is
a work in progress.

Note that the unit test program makes use of a simple unit test framework for C++ called UTM
(Unit Test Manager). This framework generates XML reports of test results and is intended to be
used with a continuous integration server.

There are also some test/demonstration programs for the Scr library in `scr/check_simple` and
`scr\check_manager`. These programs illustrate how Scr can be used, but do not attempt to
exhaustively test the library. More work is needed in this area.


Analysis
--------

The use of analysis tools is encouraged. Possibilities include valgrind and/or clang's static
analyzer. The use of the static analysis tools provided with Visual Studio should be
investigated.


Adding a Module
---------------

If a new module (source file) is added to Y (or an existing file is removed), there are a number
of steps required to put all the build control systems in order. It might be appropriate to
create a script at some point to automate this process.

### g++

1. Edit the depend.dep file to add/remove the file to/from the list. Rerun the `depend` tool in
   the `Tools` folder via the shell script `depend.sh`. This builds dependency lists for the
   Makefile. Replace the existing dependency list with the contents of `depend.out`.

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
outdated. You are invited (encouraged) to create/update them if you desire.
