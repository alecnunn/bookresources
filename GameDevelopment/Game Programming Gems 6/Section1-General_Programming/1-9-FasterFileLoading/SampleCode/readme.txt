Faster Loading with Access-Based File Re-ordering
David L. Koenig
Touchdown Entertainment, Inc.
david@touchdownentertainment.com
yarnhammer@hotmail.com
August, 2005

Code Sample

The code sample should compile under both Microsoft Visual C++ .NET 2003 and GCC. If compiling under GCC, make sure to compile the source/libs/FileIO library first.

Take a look at the bin/build.bat and bin/test.bat for example usage of the binaries. If you have trouble with them, you can always email me, and I'd be happy to help.

The data included comes from an old abandoned project built on the WildTangent Web Driver technology. It was fairly small and provided a useful test bed for the gem.

Copy the sample on to your local hard drive in order to get full use of the sample. Make sure the files are not set to read-only.

All source is released to the public domain. If you find it useful, feel free to drop me a line.


/* Resource File Format */
The file format is composed of 4 parts, the header, the string table, the file entry table, and the data chunks.

FileHeader		= 8 bytes
String table		= 2 bytes + string data * number of files
File Entry Table	= 18 bytes * number of files
Data Chunks		= file size of that entry

The system uses a STL map of std::strings in order to provide a fast lookup. The downside to this is that the file names are case sensitive. This also means that the entries are "slash" sentitive. So, a stock linux/cygwin file entry will use a forward slash, while Win32 uses the back slash. In order for the same resource file to be used on both systems, a string conversion function would be required.
