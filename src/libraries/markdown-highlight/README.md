
PEG Markdown Highlight
========================


Description
-------------------------------------------------------------------------------

This project contains:

- A Markdown parser for syntax highlighting, written in C. The parser itself
  should compile as is on OS X, Linux and Windows (at least).
- Helper classes for syntax highlighting `NSTextView`s in Cocoa applications.
- A simple example on how to highlight a `GtkTextView` in a GTK+ application.
- A simple example on how to highlight a `QTextEdit` in a Qt application.
- A parser for stylesheets that define syntax highlighting styles

_Copyright 2011 Ali Rantakari_ -- <http://hasseg.org>

This program uses the PEG grammar from John MacFarlane's `peg-markdown` project,
and the `greg` parser generator by Why The Lucky Stiff and Amos Wenger (`greg`
is heavily based on `peg/leg` by Ian Piumarta). It also contains an
implementation of the merge sort algorithm for linked lists by Simon Tatham.

Thanks to these gentlemen (and everyone who contributed to their projects) for
making this one possible.

_See the `LICENSE` file for licensing information._



API Documentation
-------------------------------------------------------------------------------

The public APIs are documented using [Doxygen][dox]. If you have it installed,
just run `make docs` and they should be available under the `docs/` directory.

[dox]: http://doxygen.org



Using the Parser in Your Application
-------------------------------------------------------------------------------

The parser has been written in ANSI/ISO C89 with GNU extensions, which means
that you need a _GCC-compatible compiler_ (see section on MSVC below, though).
You also need _Bourne Shell_ and some common Unix utilities due to a utility
shell script that is used to combine some files in a `make` step.


### Files You Need

You need to add the following files into your project:

- `pmh_definitions.h`
- `pmh_parser.h`
- `pmh_parser.c`

`pmh_parser.c` implements the parser and must be generated with `make`.
`pmh_parser.h` contains the parser's public interface and
`pmh_definitions.h` some public definitions you might want to use in files
where you don't wish to import the parser interface itself.


### Compiling in Microsoft Visual C++

First you need to generate `pmh_parser.c` somehow. There are two main
ways to do this:

- Use a Linux or OS X machine to generate it
- Generate it on Windows using MinGW (you'd run `make` in the MinGW shell)

Whichever way you go, the command you run is `make pmh_parser.c`.

MSVC does not support some of the GNU extensions the code uses, but it should
compile it nicely as C++ (just change the extensions to `.cpp` or set some
magic switch in the project settings to get the same effect). You may need to
insert the following to the beginning of `pmh_parser.c`:

    #include "stdafx.h"



Stylesheet Parser
-------------------------------------------------------------------------------

The `pmh_styleparser.h` file contains the style parser's public interface and
the `pmh_styleparser.c` file implements the parser. The style parser depends
on the main parser.

The stylesheet syntax is documented in `/styleparser/stylesheet_syntax.md`.
This file is compiled into an HTML format upon running `make docs`.



