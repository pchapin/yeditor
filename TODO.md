
Y 2.0 TODO List
===============

This file contains an overview of items that need work in Y. If you are looking for a way to
contribute to Y, start here. Some of the items listed here are actually bugs and should be
transferred to the GitHub issues list (for that matter some of the TODO items probably should be
transferred to GitHub as "enhancement" tickets). See also the Doxygen generated todo list for
additional items.

The items below are in no particular order.

+ Write a Y style guide to help future Y developers understand the style used in Y's source
  code.

+ Update the comments in the source files to Doxygen style. [In progress]

+ Remove C library I/O functions and replace them with IOStreams facilities instead.

+ Update the Y version 2.0 design documentation. The current design document is very old and
  outdated.

+ Create a UML class diagram that describes the structure of Y's classes.

+ Create unit tests for the low level classes and EditFile classes.

+ Set up some kind of continuous integration server to do automated builds and tests.

+ Think about how the UI components of Y could be factored out so that a GUI user interface
  could be layered on top of the EditFile classes and macro engine. Would it be possible to
  build a wxWidgets UI for Y? This might be a question for the Scr project.

+ Run PC-Lint over the entire project.

+ Fix the size_t to int conversion warnings that arise when building Y as a 64 bit executable in
  Visual Studio.

+ Start the design of the Y server.

+ Define an application-wide exception handling policy and start implementing it.

+ Define thread safety requirements (if any) and review code for conformance. Can any part of Y
  be usefully parallelized?

+ Think about how Y could handle non-ASCII text (such as UTF-8 text, etc). This will probably
  require a considerable amount of reworking. However in today's environment support for UTF-8
  is becoming increasingly essential.

+ Should Y's macro engine be a real Forth interpreter?

+ Y should probably either leave line endings alone (writing back whatever it finds) or
  completely regularize them (does it do that already?).

+ Typing a left arrow when at the beginning of a line should wrap to the end of the previous
  line. Although this is a bit inconsistent with what happens when typing a right arrow at
  the end of a line, I think overall it would provide a better user experience.

+ Right now Y only checks for a changed file (on disk) when it comes back from executing an
  external command. While this makes sense in a world where Y is the only programming running,
  it is not necessarily the right behavior when there are other programs on the system editing
  files as well. Perhaps Y should also periodically (in a second thread??) check for changed
  files on disk. Should it attempt to merge such changes with any changes in its local buffers?

+ The total number of lines in the file should be displayed along with current point information
  in the lower right corner of the window.

+ The `next_procedure` and `previous_procedure` commands could work better. In C++ files, they
  can't seem to find functions inside a namespace (for example).

+ Implement improved word wrapping, including "smart" wrapping in the comments of the supported
  programming languages.

+ In the current incarnation of Y, the command line and the text in the `Find_File` parameter
  box are handled the same way. I don't think this should continue. As Y gets more complicated,
  the desired functionality of the command line and the desired functionality of the Find_File
  box are going to diverge. While the `Find_File` box *should* support multiple names,
  wildcards, and abbreviated names, there is no need for it to support some of the command line
  switches, the name of the start-up macro or other things we might want to include. Y needs to
  be adjusted so that the code that implements the command line and the code that implements the
  `Find_File` box is different.

+ Add macro commands that manipulate the stack.

+ Add variables to the macro language.

+ Add input commands to the macro language.

+ Add flow of control structures to the macro language (if, while, etc).

+ Add macro commands that allow the text being edited to be examined.

+ Some users want Shift+Tab to be a recognized keystroke. In particular they want it to do a
  back tab operation. I'm not sure how I feel about this concept, but it's worth thinking about.

+ Make Y remember and process tabs as tabs? There are applications where tabs must remain in a
  file as they are entered (for example Makefiles... I think this might be the only place where
  it truly matters).

+ Change the special handling of awk programs to special handling of perl programs. Actually I'm
  not sure how I feel about this. It might be nice to provide a version of awk with Y, along
  with suitable documentation and tutorial material. That would make the awk support more
  useful. In general awk is lighter weight than Perl and that's a plus. Perhaps both languages
  (or even other languages: Lua? Python?) could be supported.

+ Currently `KeyboardWord` objects (derived from `WordSource`) try to push a `StringWord` object
  onto the macro stack for each keystroke they process. This requires that the macro stack have
  external linkage. It is unnecessary coupling between `macro_stack.cpp` and `WordSource.cpp`.
  Word sources are lower level concepts and should not know about the macro stack. To fix this,
  `KeyboardWord` should hold a private `StringWord` and draw from that as necessary. Fix this
  situation.

+ To support terminal mode better, it would be nice if Y could properly handle the escape
  sequences sent by terminals when the various special keys are pressed. Doing this well will be
  a bit of work. However, even a partial solution (for example for the arrow keys) would be
  helpful. [Note that this is only relevant in a situation where Y is not using curses... such
  as when Y is used on an embedded system over a serial connection. It is unclear how important
  this mode of operation really is and if it should even continue to be supported.]

+ Generally terminal support, even under curses, is rought and flakey. This needs to be fixed.

+ There is odd flickering of the display when Y is used in a Windows console. I don't recall it
  doing this in the past so something changed "recently" to cause it. It should be fixed; the
  flickering is annoying.
