This directory contains the sample code for a simple interpreter called
SInterp.  It has an associated compiler called SCC that generates the
bytecode streams it needs to execute.  SCC included in this directory is a
more advanced version of the SCC created for my previous article.  This
compiler supports variables, if-statements, and for-loops.

In order to execute a bytecode generated from SCC, simply run SInterp with
the name of the script to execute.  It will run the script and spew to
stdout the results of each opcode as they are executed.  A sample bytecode
stream sample.bin is included.

