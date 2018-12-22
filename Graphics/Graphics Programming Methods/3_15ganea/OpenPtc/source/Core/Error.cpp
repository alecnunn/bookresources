//
// Error class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream.h>
#include <windows.h>
#include "Core/Log.h"
#include "Core/Error.h"
#include "Win32/Cursor.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




PTCAPI Error::Error()
{
    // defaults
    defaults();
}


PTCAPI Error::Error(const char message[])
{
    // allocate new message
    m_message = new char[strlen(message)+1];

    // set error message
    strcpy(this->m_message,message);

    // log error creation
    LOG("error",*this);
}


PTCAPI Error::Error(const char message[],const Error &error)
{
    // calculate length of composite error message string
    int length = strlen(message) + strlen("\n") + strlen(error.m_message) + 1;

    // allocate new message
    m_message = new char[length];

    // setup composite error message
    sprintf(m_message,"%s\n%s",message,error.m_message);

    // log composite error creation
    LOG("composite error",*this);
}


PTCAPI Error::Error(const Error &error)
{
    // defaults
    defaults();

    // assignment
    *this = error;
}


PTCAPI Error::~Error()
{
    // free message
    delete[] m_message;
}




void PTCAPI Error::report() const
{
    // log error reporting
    LOG("error report",*this);

    // show cursor
    Win32Cursor::show();

    // report error
    MessageBox(0,m_message,"Error",MB_OK | MB_ICONERROR | MB_SETFOREGROUND | MB_TOPMOST);

    // exit
    exit(1);
}




const char* PTCAPI Error::message() const
{
    // get message
    return m_message;
}




Error& PTCAPI Error::operator =(const Error &error)
{
    // check for self assignment
    if (this==&error) throw Error("self assignment is not allowed");
        
    // check if error message string will fit in this one
    if (strlen(error.m_message)>strlen(m_message))
    {
        // free old message
        delete[] m_message;

        // allocate new message
        m_message = new char[strlen(error.m_message)+1];
    }

    // copy error message string
    strcpy(m_message,error.m_message);

    // finished
    return *this;
}


bool PTCAPI Error::operator ==(const Error &error) const
{
    // is equal to
    if (!strcmp(error.m_message,m_message)) return true;
    else return false;
}


bool PTCAPI Error::operator !=(const Error &error) const
{
    // not equal to
    return !(*this==error);
}




void Error::defaults()
{
    // defaults
    m_message = new char[1];
    m_message[0] = 0;
}
