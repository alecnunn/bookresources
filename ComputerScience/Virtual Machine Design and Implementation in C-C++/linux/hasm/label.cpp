/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+ label.cpp - metadata for labels               	  		        +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ decalaration                                                      + 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

struct Label
{
	U8 text;		/* index into StrTbl of where identifier begins */
	U8 address;		/* address of label*/
	U4 line;		/* source code line containing declaration */
};





