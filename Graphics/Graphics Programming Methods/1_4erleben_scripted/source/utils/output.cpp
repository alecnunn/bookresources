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
#if(_MSC_VER == 1200)
#pragma warning(disable:4244)   // No warnings on precision truncation
#pragma warning(disable:4305)   // No warnings on precision truncation
#pragma warning(disable:4786)   // stupid symbol size limitation
#endif
#include <utils/output.h>
#include <iostream>
using namespace std;


fstream Output::file;         ///< An output file

/**
 *
 */
void Output::error(string classname,string method, string message){
  if(file.is_open()){
    file << "Error " << classname << "." << method << ": " << message << endl;
  }else{
    cerr << "Error " << classname << "." << method << ": " << message << endl;
  }
};

/**
 *
 */
void Output::warning(string classname,string method, string message){
  if(file.is_open()){
    file << "Warning " << classname << "." << method << ": " << message << endl;
  }else{
    cout << "Warning " << classname << "." << method << ": " << message << endl;
  }
};

/**
 *
 */
void Output::message(string classname,string method, string message){
  if(file.is_open()){
    file << "Message " << classname << "." << method << ": " << message << endl;
  }else{
    cout << "Message " << classname << "." << method << ": " << message << endl;
  }
};

