Compiling the compiler
-----------------------
[1] go to the web site [Streett02] and download flex, bison, and bison.simple.
[2] place these in the VertexShaderCompiler\ASTGen directory
[3] open the file VertexShaderCompiler.dsw into Microsoft Visual C++.  
    They have been compiled with version 6.0 successfully under Windows 2000.
[4] F7 should compile the compiler and the result is a file in CodeGen\Debug\compiler.exe

Using the compiler
--------------------
In the bin/ directory lives the file compiler.exe.  It takes two parameters:  the input file 
name and the output file name.  The input file name is a vertex shader in the example language
described in the Gem.  The output is valid DX8 vertex shader assembly code.


Modifying the keywords 
-----------------------
You can modify the register-->keyword default mappings by modifying the entries in the 
symbol table.  yaccer.y has a function InsertSymbols(), that loads the values.
You can also change what symbols you are looking for by changing the strings in each 
of these entries, but you MUST also to remember to change this string in the file lexer.l.
For example, if we need to change 'LightAmb0' to 'ambient' change the keyword in 
InsertSymbols() and also the keyword in the file lexer.l. The keywords are listed in one 
long column of entries. I picked names that seemed to be obvious, you may have different
keywords or terms you prefer.

bin/ 
	Contains the binary compiler.exe.  The build will copy the compiler into this 
	directory, so be sure to copy this off of the CD so the write can take place.
InputExample/
	Example shader(s).
OutputExample/
	Example assembly output from the compiler.
VertexShaderCompiler/
	Contains source for Compiler
VertexShaderCompiler/ASTGen/
	lexer and yaccer files for the compiler.  Also, this is the location you should 
	copy flex.exe, bison.exe, and bison.simple, once they are downloaded.  
	See Compiling the Compiler section above for more details.
VertexShaderCompiler/CodeGen/
	Contains classes for the code generator.  Many of these objects are shared with 
	the lexer and yaccer


References
--------------
[Streett02] http://www.monmouth.com/~wstreett/lex-yacc/lex-yacc.html.
Flex and bison ports to Win32, including source and documentation. 

