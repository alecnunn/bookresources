//
// Dynamic Error class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_DYNAMIC_ERROR_H
#define __PTC_DYNAMIC_ERROR_H

// include files
#include "config.h"
#include "C/Error.h"
#include "C/Extra.h"
#include "Core/Types.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class DynamicError
{
    public:

        DynamicError()
        {
            // use exceptions
            ptc_use_exceptions();

            // create error object
            object = ptc_error_create("");
        }

        DynamicError(const char message[])
        {   
            // use exceptions
            ptc_use_exceptions();

            // create error object
            object = ptc_error_create((char*)message);
        }

        DynamicError(const char message[],const DynamicError &error)
        {
            // use exceptions
            ptc_use_exceptions();

            // create error object
            object = ptc_error_create_composite((char*)message,error.object);
        }

        DynamicError(const DynamicError &error)
        {
            // use exceptions
            ptc_use_exceptions();

            // create error object
            object = ptc_error_create("");

            // assign error
            ptc_error_assign(object,error.object);
        }

        ~DynamicError()
        {
            // destroy error object
            ptc_error_destroy(object);
        }
        
        void report() const
        {
            // report error
            ptc_error_report(object);
        }

        const char* message() const
        {
            // get error message
            return ptc_error_message(object);
        }
        
        DynamicError& operator =(const DynamicError &error)
        {
            // assignment operator
            ptc_error_assign(object,error.object);
            return *this;
        }

        bool operator ==(const DynamicError &error) const
        {
            // is equal to
            return (ptc_error_equals(object,error.object)==1);
        }

        bool operator !=(const DynamicError &error) const
        {
            // not equal to
            return (ptc_error_equals(object,error.object)==0);
        }
        
    private:

        // error object
        PTC_ERROR object;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
