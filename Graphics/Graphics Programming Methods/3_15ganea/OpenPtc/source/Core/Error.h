//
// Error class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_ERROR_H
#define __PTC_ERROR_H

// include files
#include "config.h"
#include "Core/Types.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class Error
{
    public:

        // setup
        PTCAPI Error();
        PTCAPI Error(const char message[]);
        PTCAPI Error(const char message[],const Error &error);
        PTCAPI Error(const Error &error);
        PTCAPI ~Error();

        // report error
        void PTCAPI report() const;

        // get error message
        const char* PTCAPI message() const;

        // operators
        Error& PTCAPI operator =(const Error &error);
        bool PTCAPI operator ==(const Error &error) const;
        bool PTCAPI operator !=(const Error &error) const;

    protected:

        // defaults
        void defaults();
        
        // error message
        char *m_message;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
