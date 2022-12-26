
C++ Style Guide
===============

## Introduction

This document describes the style used for the C++ code in the Y project. This document is,
of course, subject to discussion, change, and evolution.

The intent of this style guide is to promote a consistent, readible style, and to reduce the
chances for errors. Exceptions to this style should be made with those primary goals in mind.
Exceptions are expected and are not considered problematic, particularly if they enhance local
readibility, unless the exceptions become commonplace. In that situation, either the code or the
style guide should be reviewed and modified.

In this document "shall" is intended to define a requirement of this guide, "should" is intended
to define a recommendation, and "may" is intended to define an option.

## Naming

+ Abstract types, classes, structs enumerations, and class templates shall be given names
  consisting of a capital letter followed by all lowercase letters. Multiple words in such names
  shall each start with a capital letter but there shall be no space or other punctuation marks
  between words. For example: `WordBuffer`.

+ Objects, functions (including methods), function templates, function parameters, and class
  data members, shall be given names consisting of all lower case letters. Multiple words in
  such names shall be separated by a single underscore character. For example: `letter_count`.

+ Type names introduced with a typedef declaration outside of class scope shall be given names
  consisting of all lower case letters with a `_t` suffix. Multiple words in such names shall be
  separated by a single underscore character. For example: `index_t`.

+ Type names introduced with a typedef declaration inside class scope shall be given names
  consisting of all lower case letters with a `_type` suffix. Multiple words in such names shall
  be separated by a single underscore character. For example: `size_type`.
 
+ Names of entities that are intended to extend or mimic the standard library should follow the
  same naming convention as the standard library even if that would violate the above rules. For
  example: `netbuff`.

+ Names of constant objects and enumerators shall consist entirely of upper case letters.
  Multiple words in such names shall be separated by a single underscore. For example:
  `RED_LIGHT`, `PI`.

+ Macros, both object-like and function-like, should be avoided in favor of constant objects and
  inline functions. However, if macros are used their names shall consist entirely of uppercase
  letters. Multiple words in such names shall be separated by a single underscore. These rules
  are intended to be the same as for constant objects and enumerators. For example:
  `BUFFER_SIZE`.

+ Abbreviations should be avoided in names except for certain very well known exceptions.
  Abbreviations used by the standard library are acceptable for names of things that are
  intended to extend or mimic the library. For example: `line_buffer` (not `line_buf`), but
  `max_count` is okay because "max" is a widely used abbreviation for maximum.

+ Names should be long and meaningful. However, a special exception is made for loop index
  variables: such names can be one of i, j, k, m, or n as is traditional. Also it is recognized
  that short names are meaningful in some contexts. For example, `x_coordinate` is probably the
  best but `x_coord` might be acceptable under some circumstances. Even `x` could be acceptable
  as, for example, a cartesian coordinate in a graphing program. Short names may also acceptable
  if they follow the names used in a reference (for example, an algorithms text book) provided
  the reference is listed in code comments.

+ Names, particularly of library components should be introduced into an appropriate name space.
  No names should be added to the std name space except for specializations of standard
  templates (this exception is required by the language).

+ Except for the std name space (see below), using directives shall not be used. In any case,
  neither using directives nor using declarations shall appear in a header file. Using
  declarations should be used sparingly and shall never be used at global scope. In general
  names, other than standard names, should be qualified by their name space.

+ A using directive for the std name space should be put at the top of each file immediately
  after the include directives. The standard name space is special, being standard, and names
  from that name space need not be qualified.
  
## Spacing/Formatting

+ The braces that delimit a block, class definition, array or structure initialization, or
  enumeration definition shall follow this format:
  
      if( x == y ) {
          // Blah...
      }
           
+ The braces that delimit a function or method definition shall follow this format:
  
      void f( )
      {
          // Blah...
      }
      
+ Material inside a block shall be indented by four spaces. Declarations beneath the access
  specifiers in a class definition shall be intended by four spaces past that of the access
  specifiers (the access specifiers themselves need not be indented relative to the class
  header). Statements beneath case labels in a switch statement shall be indented by four spaces
  past that of the case labels. These rules apply recursively to nested blocks, class
  definitions, and switch statements.

+ Lines that are continued on the next line should be indented four spaces past that of the
  continued line.

+ Function calls or function declarations that are broken over multiple lines should treat the
  parenthesis in the function call or declaration like braces for purposes of formatting. In
  such a case, each parameter of the call or declaration should be on a line by itself and
  indented by four spaces (although closely related parameters may be grouped if desired).

+ If there is only one statement inside a block, the braces are optional. However, the
  indentation rules shall still apply if that statement is on a separate line from the control
  structure that opens the block.

+ Extremely short functions (particularly in-line functions) may have bodies that are all on one
  line even though this would violate rule 2.2 above. For example
  
      inline int max( int a, int b )
          { return ( a > b ) ? a : b; }

+ The `else` of a conditional statement and `catch` clauses of exception handlers shall be the
  first non-white space character of a line (no preceeding '}').
  
      try {
          if( a < b ) {
              // Do stuff...
          }
          else {
              // Do other stuff...
          }
      }
      catch( ... ) {
          // Handle me!
      }
  
+ Function and method definitions shall be separated by two blank lines.

+ There should be no space between a unary operator and its operand.
  
+ There should be at least one space between a binary operator and its operands. Exception: the
  '.', '->', '.*', and '->*' operators shall not have any spaces between them and their
  operands.
    
+ There shall be at least one space after each comma or semicolon. However, there shall be no
  space before each comma or semicolon. (This rule is intended to mimic the normal rules of
  English writing).

+ There should be one space after a '(' and before a ')'.

+ There should be no space after the name of a function in a function call or declaration. This
  rule also applies to function-like macros.

+ There should be no space after the reserved words `if`, `for`, or `while`.

+ There should be no space after '<' or before '>' in a template argument or paramter list.

      template<typename T>

+ A source file shall be formatted assuming that a fixed width font is used. Alignment may be
  used between source lines to emphasize relationships between those lines even if doing so
  would violate the other rules of this guide. In fact such alignment is encouraged. RATIONALE:
  Although proportional width fonts are popular in most writing, and are even occasionally used
  in programming, they don’t make aligning one line with another very feasible. Such alignment
  is an excellent device for highlighting the similarity between related, but complicated
  operations.

+ A source file shall be formatted so that no line is longer than 96 characters. RATIONALE: To
  maximize the likelihood that the source file will display properly in a wide range of
  environments, very long lines should be avoided. The 96 character limit, while arbitrary,
  allows reasonably long lines without getting into problems of line wrapping or truncation on
  most systems. On a device with a large display, the 96 character limit makes putting multiple
  editor windows side by side a feasible thing to do.
  
## Structure

+ C++ source files shall be named with a `.cpp` extension. Header files containing C++
  constructors (or potentially containing such constructs) shall be named with a `.hpp`
  extension.
  
## Usage

+ Every object should be initialized when it is declared. Every object’s point of declaration
  should be close to where it is first used. For loop indexes should be declared as part of the
  loop except in the cases where the value of the index variable is needed outside of the loop.
  
+ Only new style casts shall be used. RATIONALE: New style casts provide more information to the
  reader of the program. Also, since they are more specific than the general purpose old style
  cast, their consistent usage may help the compiler detect inappropriate casts.

+ All expressions shall have appropriately matched types. Explicit casts shall be used to
  indicate intent in cases where the types do not match naturally. Casts shall be used even when
  automatic type conversion rules would cause the same conversion. RATIONALE: Although C++
  provides many implicit type conversions, one should program as if it did not have those
  conversions. Such an approach produces more robust programs. If available a tool should be
  used to verify type usage strictly.

+ Programs should be developed using the highest reasonable warning level provided by the
  compiler. Code should be written so that it compiles without warnings. Exception: It is
  understood that some compilers provide very aggressive warning levels that might be considered
  unreasonable for general use.

+ An explicit return statement may be placed at the end of void functions.

+ Every possible value of a switch statement's controlling expression should be accounted for in
  the switch statement's case list. If necessary one should include an empty default case to
  explicitly document one's intention to ignore values not otherwise mentioned.

+ Any class with virtual methods shall have a virtual destructor.

+ Any class that has either a destructor, a non-trivial copy constructor, or a non-trivial
  assignment operator should have all three of these methods. If any of these methods are not
  actually necessary, explicit documentation to that effect shall be added to the class.

+ All data members of a class should be private. Protected data may be used in some cases.
  Structures (as opposed to classes) may have public data members. Indeed, that is presumed to
  be the reason why a struct is being used.

## Documentation

+ Each source file shall have a comment header containing the name of the file, the author, and
  a description of the file’s contents. The comment header may also include a “to-do” list, and
  relevant references.

+ Each module in the program should have associated documentation that describes how to use the
  module and, optionally, how the module works. The documentation for a module may include
  pseudo-code, UML diagrams, sample code, and any other information that will aid in the
  understanding of the module.

+ Each declaration in a header file should have an associated comment that describes the
  interface to the declared entity. If a module has associated documentation, these comments may
  be sparse (perhaps just references to the documentation) otherwise they should be complete
  enough to serve as the documentation. An inline documentation tool such as Doxygen may be used
  to create documentation but the use of such tools is neither required nor necessarily
  recommended. RATIONALE: Inline documentation tools have their uses, but the large comments
  they require (if good quality documentation is desired) can be distracting in the C++ source
  files. Some programs may be better served with out-of-line documentation.
  
+ Each function in an implementation source file should have a comment header containing a brief
  description of how the function works. Note that implementation comments should not describe
  the interface to a function if the interface has been described elsewhere.
 
 + The body of each function shall be broken into logically distinct parts with blank lines
   separating the parts. A short comment may be provided for each of these parts.
   
