////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002
//  Department of Computer Science Copenhagen University (DIKU).
//
//  All Rights Reserved.
//
//  This software is provided as is WITHOUT ANY WARRANTY; Use it on
//  your own risk.
//
//  Please send remarks, questions and bug reports to kenny@diku.dk
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __OUTPUT_INCLUDED__
#define __OUTPUT_INCLUDED__
#include<fstream>
#include<string>
using namespace std;

/**
 * Output Class
 */
class Output
{
public:

  static void error(string classname,string method, string message);
  static void warning(string classname,string method, string message);
  static void message(string classname,string method, string message);


public:

  static fstream file;

};/* End class Output */

#endif //__OUTPUT_INCLUDED__
