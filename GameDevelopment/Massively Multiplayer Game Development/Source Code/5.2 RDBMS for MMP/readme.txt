4 python source modules are included:

1)  constantmodule.py - demonstrates on disk and in memory constant modules
2)  lookup.py - demonstrates lookups of various types, both on disk and in memory
3)  stringtable.py - demonstrates a simple implementation of a string table using a lookup
4)  localization.py - demonstrates an implementation to handle localization using lookups and constant modules

Each of these files can be run from the command prompt.  CD to the location where the files are located and type

"python <filename>" where filename is one of the modules, including the .py extension.  

This assumes you have python installed (if not, go to www.python.org to download), and that it is in your path.

If it's not in your path, you'll have to fully qualify the Python location.

You can also run the tests interactively from inside the Python interpreter by importing the module, and calling the Test() method
as follows:

>>> import localization
>>> localization.Test()


The last source file provided is stringscript.sql, which is an SQL script that can be run under Microsoft SQL Server to generate all the 
database objects described in the String Table section of the article.


Enjoy

Jay Lee