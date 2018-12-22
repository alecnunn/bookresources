//=============================================================================
//
// 	main.cpp   
//
// 	Author:	 James Boer
//
//	Copyright (C) 2001 James R. Boer  All Rights Reserved.
//
//  This is a demonstration of the Parser and associated classes.  The text
//  parser is designed to handle any file format, including languages as
//  complex in syntax as C++.  
//	
//=============================================================================



#include <iostream>
#include <conio.h>
#include "Parser.h"

using namespace std;


int main()
{
	
	Parser parser;
	TokenList toklist;
	TokenFile tokfile;

	// Create a parser with the default operators (the default list is similar
	// to C++)
	parser.Create();

	// Parsing happens in three optional stages.  ProcessSource() extracts the
	// text from the file and organizes all elements into token categories.
	if(!parser.ProcessSource("Test.txt", &toklist))
	{
		cerr << "Could not read file 'Test.txt'.  Be sure the file is located in the same directory as Parser.exe";
		getch();
		return -1;
	}

	// ProcessHeaders() is only needed if you're using the built-in header file
	// processing mechanism.
	if(!parser.ProcessHeaders(&toklist))
	{
		cerr << "Could not read header file text.  Be sure the file is located in the same directory as Parser.exe";
		getch();
		return -1;
	}	
	// Macros need to be processed explicitly if they are used in headers and
	// source text.
	parser.ProcessMacros(&toklist);

	// Cycle through the list and look at the individual tokens extracted.  Note
	// that the list operates just like a normal stl list, but has a few additional
	// functions such as GetDescriptiveString().
	TokenList::iterator itr;
	for(itr = toklist.begin(); itr != toklist.end(); ++itr)
		cout << toklist.GetDescriptiveString(itr).c_str() << endl;

	cout << "Press any key to continue...\n";
	getch();

	// Demonstrates how to write to and read from a binary file
	// using a token list.
	if(!tokfile.Write("Test.bin", &toklist))
	{
		cerr << "Could not write binary file 'Test.bin'.  Be sure to copy this sample program to your hard drive before attempting to run it.";
		getch();
		return -1;
	}

	toklist.clear();
	tokfile.Read("Test.bin", &toklist);

	// Note that the token list read from the binary file is identical
	// to the one read from the text file, with the exception of the
	// line and file debugging information.
	for(itr = toklist.begin(); itr != toklist.end(); ++itr)
		cout << toklist.GetDescriptiveString(itr).c_str() << endl;

	cout << "Press any key to continue...\n";
	getch();

	return 0;
}