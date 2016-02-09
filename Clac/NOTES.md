
General Comments
================

Clac is in a state of disarray. A lot of work will need to be done on it before it is reasonably
well organized again. Here is the current state of the program:

Clac is intended to have three parts

+ The entity library. This is a library of calculator entity types.

+ The execution engine. This does the actual calculations. The execution engine together with
  the entity types could be made into a stand alone scripting language (at least that is the
  idea).

+ The user interface. This gives a nice, generic interface to the execution engine.

Right now the entity library source is in clac_entity and the execution engine is in
clac_engine. The files related to the UI are in the top leve directory. Documentation is in the
doc directory.

Notes, Bugs
===========

+ There are "To Do" lists for each source file in the comment header of that file. If you are
  looking for something useful to do, just review the various source files until you find
  something that looks interesting to you!

+ Here is a list of additional operations and features that probably should be supported
  someday. Many of these items are taken directly from the HP-48 documentation.

     ! (gamma)
     %
     %ch
     %t
     >rat
     beep
     debugging
     claculating (please wait...)
     commenting
     constants library
     d->r
     det
     dir ops
     documentation
     dot and cross products
     engineering notation for integers
     fp
     hyperbolics
     log base a of x
     matrices
     move screen stuff into separate file.
     pick level
     programming capability
     random (0 to 1) <working on it in actions.cpp>
     relational tests (==, !=, <=, etc.)
     relocate purge & sto
     shifts + rotates
     signed binarys
     size
     stack editing... (pop up window)
     statistics + probability
     tags
     trace     (matrices)
     transpose (matrices)
     type
     undo (last)
     units
