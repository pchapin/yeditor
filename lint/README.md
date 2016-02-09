
Lint Configuration
==================

This directory contains configuration files for the PC-Lint static analysis tool. See

     http://www.gimpel.com/
     
for more information about PC-Lint.

The files in this directory are global configuration files used by all Y related projects and
subprojects. Most of the *.lnt files are intended to be exact copies of the files that come with
the PC-Lint distribution. Note that these files are sometimes updated; the latest versions are
on the Gimpel web site.

The file std.lnt coordinates the use of the other files. It is currently set up to use the Open
Watcom specific configuration. However, with a few adjustments it could be switched to use the
Visual C++ specific configuration.

The file options.lnt contains overall options that pertain to all projects. Use this file for
any new global settings.

Each project contains a project specific configuration file that contains settings that pertain
to only that project. In addition each project contains a LIN.BAT batch file that runs PC-Lint
with appropriate options. Finally each project contains a file named FILES.lnt that contains a
list of source files in that project that have passed (or mostly passed) PC-Lint's analysis.
Running LIN.BAT on FILES.lnt causes PC-Lint to examine all files in one run. Under those
conditions the global analysis results should be (more) meaningful.

In this folder you will also find C++ "translation" headers. When one includes <algorithm> in an
Open Watcom program, the compiler actually locates the file algorith instead. Open Watcom can be
installed on an old style FAT16 file system with 8.3 file name limitations. It uses header name
conversion rules to associate standard names as used in programs with files a FAT16 file system
can handle. However, PC-Lint doesn't know these rules. Instead it will locate the <algorithm>
header in this directory. That header just contains an explicit #include of <algorith> that
PC-Lint can use to locate the real header file.
