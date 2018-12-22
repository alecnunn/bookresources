
/******************************************************************************
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.

By downloading, copying, installing or using the software you agree to this
license. If you do not agree to this license, do not download, install,
copy or use the software.

Intel Open Source License for "Improved Bones Deformation" and
"Constrained Inverse Kinematics" sample code

Copyright (c) 2002, Intel Corporation.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
	be used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR
JURISDICTION. It is each licensee's responsibility to comply with any export
regulations applicable in each licensee's jurisdiction.
******************************************************************************/

#ifndef __GPGException_h__
#define __GPGException_h__


#include <stdarg.h>


/** Initialization and/or configuration error. */
#define GPG_E_CONFIGURATION		MAKE_GPGRESULT_FAIL( GPGRESULT_COMPONENT_GENERIC, 0x1001 )
/** Implementation usage error.  The code user of the framework has made an
mplemntation error.  */
#define GPG_E_IMPLEMENTATION	MAKE_GPGRESULT_FAIL( GPGRESULT_COMPONENT_GENERIC, 0x1002 )
/** Dynamic library error.  The dynamic component mechanism had a failure.  */
#define GPG_E_DYNAMICLIB		MAKE_GPGRESULT_FAIL( GPGRESULT_COMPONENT_GENERIC, 0x1003 )

/*	Sometimes exception throwing seems to get derailed (or we want the stack
	preserved)...so we cause assert. */
#define GPG_E_ASSERT


/**********************************************************************//**
This is the base exception class for the particle system framework.
This class supports both GPGRESULT codes and string messages.

/verbatim
Example calling code usage:
try
{
	something();
}
catch(GPGException &e)
{
	e.Log();
	throw;
}

Example called code:
something()
{
	throw GPGException("something is broken");
}

or

something()
{
    throw GPGException(GPG_E_OUT_OF_MEMORY,"no more space");
}
/endverbatim
*//***********************************************************************/
class GPGException
{
	public:
		/** Default constructor */
		GPGException(void)
		{
#ifdef GPG_E_ASSERT
			GPGASSERT(FALSE);
#endif
		}
		/** Constructor setting an GPGRESULT code. */
		GPGException(const GPGRESULT r)
		{
			m_result = r;
#ifdef GPG_E_ASSERT
			GPGASSERT(FALSE);
#endif
		}
		/** Constructor setting the message string via a printf interface. */
		GPGException(const char *fmt, ...)
		{
			m_result = GPG_E_UNDEFINED;
			va_list ap;
			va_start(ap, fmt);
			m_text.VSPrintf(fmt,ap);
			va_end(ap);
#ifdef GPG_E_ASSERT
			GPGLOG("ASSERTION: %s\n",m_text.Raw());
			GPGASSERT(FALSE);
#endif
		}
		/** Constructor setting the message string via a printf interface,
			and setting an GPGRESULT code. */
		GPGException(const GPGRESULT r, const char *fmt, ...)
		{
			m_result = r;
			va_list ap;
			va_start(ap, fmt);
			m_text.VSPrintf(fmt,ap);
			va_end(ap);
#ifdef GPG_E_ASSERT
			GPGLOG("ASSERTION: %s\n",m_text.Raw());
			GPGASSERT(FALSE);
#endif
		}
		/** Copy constructor */
		GPGException(const GPGException &other)
		{
			*this = other;
#ifdef GPG_E_ASSERT
			GPGASSERT(FALSE);
#endif
		}
		/** Destructor */
virtual	~GPGException(void)
		{
		}
		/** Assignment operator */
		GPGException &operator=(const GPGException &other)
		{
			if(this != &other)
			{
				m_result = other.m_result;
				m_text = other.m_text;
			}

			return *this;
		}

		/** Get the GPGResult code. */
		GPGRESULT		GetGPGResult(void)
		{ return m_result; }

		/** Get the exception message string. */
		const GPGString &GetText(void)
		{ return m_text; }

		/** Use GPGLOG to output information about this exception. */
		void			Log(void)
		{
			if(NULL == (const char *)m_result)
			{
				GPGLOG("[31mEXCEPTION:[37m 0x%x[0m\n",m_result);
			}
			else
			{
				GPGLOG("[31mEXCEPTION:[37m 0x%x %s[0m\n",
												m_result,(const char *)m_text);
			}
		}

	protected:
		GPGRESULT	m_result;
		GPGString	m_text;
};


#endif /* __GPGException_h__ */
