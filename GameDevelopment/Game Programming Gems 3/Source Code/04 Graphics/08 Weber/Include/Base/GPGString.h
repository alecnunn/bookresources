
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

#ifndef __GPGString_h__
#define __GPGString_h__

#include "GPGMemory.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define GPGSTRING_DEBUG		FALSE


/******************************************************************************
					compare		#chars		to wide

ASCII				strcmp		strlen

Multibyte	PC		_mbscmp		_mbstrlen	wcstombs/mbstowcs
			Linux				NOT mblen	wcstombs/mbstowcs

16-bit		PC		wcscmp		wcslen
			Linux	memcmp		<manual>

NOTE Number of represented characters may not be number of bytes.
	In multibyte, character lengths can vary.

NOTE It would seem that strcmp should work for multibyte.
	That's what were using for Linux (and Mac?).

NOTE wchar_t appears to be 'unsigned long' on some systems.
	If we want U16 we should be explicit.

******************************************************************************/


#ifdef WIN32
#include <mbstring.h>
//#define GPGSTRLEN_MB(x,y)		_mbstrlen((char *)x)
#define GPGSTRCMP_MB(x,y)		_mbscmp(x,y)
#define GPGSTR_MB2UPPER(x)		_mbsupr(x)
#define GPGSTR_MB2LOWER(x)		_mbslwr(x)
#define GPGSTR_WC2UPPER(x)		_wcsupr(x)
#define GPGSTR_WC2LOWER(x)		_wcslwr(x)
#define GPGVSNPRINTF(x,y,z,a)	_vsnprintf(x,y,z,a)
#else
#include <ctype.h>
#define GPGVSNPRINTF(x,y,z,a)	vsnprintf(x,y,z,a)
#endif

#ifdef __linux__
//#define GPGSTRLEN_MB(x,y)		mblen((char *)x,(size_t)y)
#define GPGSTRCMP_MB(x,y)		strcmp((char *)x,(char *)y)
#endif

#if defined(GPG_OSTYPE) && GPG_OSTYPE==GPG_FBSD
#define GPGSTRING_16			__wchar_t		// Wide 16-bit char
#define GPGSTRCMP_MB(x,y)		strcmp((char *)x,(char *)y)
#define GPGSTR_MB2WC(x,y,z)		mbstowcs((GPGSTRING_16 *)x,(const char *)y,z)
#define GPGSTR_WC2MB(x,y,z)		wcstombs((char *)x,(const GPGSTRING_16 *)y,z)

#elif defined(MACINTOSH)		// TODO check: only if wchar_t support is off
#define GPGSTRING_16			unsigned short	// Wide 16-bit char
//#define GPGSTRLEN_MB(x,y)		mblen((char *)x,MB_CUR_MAX)
#define GPGSTRCMP_MB(x,y)		strcmp((char *)x,(char *)y)

// Need to do this otherwise get type mismactch when building GPGMRMAUTHOR.
#ifndef MRMAUTHOR
#define GPGSTR_MB2WC(x,y,z)		mbstowcs((char *)x,(const char *)y,z)
#define GPGSTR_WC2MB(x,y,z)		wcstombs((char *)x,(const char *)y,z)
#else
#define GPGSTR_MB2WC(x,y,z)		mbstowcs((unsigned short *)x,(const char *)y,z)
#define GPGSTR_WC2MB(x,y,z)		wcstombs((char *)x,(const unsigned short *)y,z)
#endif
//* TODO check for MAC upper/lower converters

#else	// not MAC
#define GPGSTRING_16			wchar_t			// Wide 16-bit char
#define GPGSTR_MB2WC(x,y,z)		mbstowcs(x,(char *)y,z)
#define GPGSTR_WC2MB(x,y,z)		wcstombs((char *)x,y,z)
#endif

#define GPGSTRLEN(x)			strlen((char *)x)
#define GPGSTRCPY(x,y)			strcpy((char *)x,(char *)y)

#define GPGSTRING_I8	char			// Ascii 8-bit char (probably 7-bit)
#define GPGSTRING_U8	unsigned char	// Ascii 8-bit char (probably MB)

#define GPGSTRING_16_SIZE		sizeof(GPGSTRING_16)

/**************************************************************************//**
class GPGString

	reference counted with auto-destruction
	operates in multiple formats with JIT conversion

	Initially based on: Stroustrup C++ Ref Man, 2nd ed., pg. 248

	(Historical) Previously for NewBuffer/DeleteBuffer:
	The private virtual allocation functions exist to allow a DLL to
	assign a value even if the object was created in the app (or vica versa).
	Otherwise newing in the DLL and deleting in the app would cause a fault.
	The virtual functions force the allocation in the same heap that the
	object was created in.  The GPGString itself still has to be created and
	destructed in the same heap.
*//***************************************************************************/
class GPGString
	{
	private:

	class GPGStringRep
		{
		public:
			void					NewBuffer(long size);
			void					DeleteBuffer(void);

			GPGDeallocateFunction	*m_pDeallocateRep;
			GPGDeallocateFunction	*m_pDeallocateBuffer;
			long					m_references;
			GPGSTRING_U8			*m_buffer8;
			GPGSTRING_16			*m_buffer16;
			bool					m_stale8;
			bool					m_stale16;
		};

	public:
					GPGString(void);
					GPGString(const GPGString &operand);
					GPGString(const GPGSTRING_I8 *operand);
					GPGString(const GPGSTRING_U8 *operand);
					GPGString(const GPGSTRING_16 *operand);

					/// allow init as 0 in templates
					GPGString(I32 operand);

virtual				~GPGString(void);

					operator const GPGSTRING_I8 *(void) const {return Raw(); }
					operator const GPGSTRING_U8 *(void) const {return RawU8();}
					operator const GPGSTRING_16 *(void) const {return Raw16();}

		GPGString	&operator=(const GPGString &operand);
		GPGString	&operator=(const GPGSTRING_I8 *operand)
						{ return operator=((const GPGSTRING_U8 *)operand); };
		GPGString	&operator=(const GPGSTRING_U8 *operand);
		GPGString	&operator=(const GPGSTRING_16 *operand);

#if 0
		bool		operator==(const GPGString &operand) const;
		bool		operator==(const GPGSTRING_I8 *operand) const
						{ return operator==((const GPGString)operand); };
		bool		operator==(const GPGSTRING_U8 *operand) const
						{ return operator==((const GPGString)operand); };
		bool		operator==(const GPGSTRING_16 *operand) const
						{ return operator==((const GPGString)operand); };
#endif

/*
		bool		operator!=(const GPGString &operand) const;
		bool		operator!=(const GPGSTRING_I8 *operand) const
						{ return operator!=((const GPGString)operand); };
		bool		operator!=(const GPGSTRING_U8 *operand) const
						{ return operator!=((const GPGString)operand); };
		bool		operator!=(const GPGSTRING_16 *operand) const
						{ return operator!=((const GPGString)operand); };
*/

					/** Standard string compare: returns -1, 0, or 1 if the
					string is alphabetically less than, equal, or greater than
					the operand */
		I32			Compare(const GPGString &operand) const;

		void		ForceUppercase(void)	{ ForceCase(true); };
		void		ForceLowercase(void)	{ ForceCase(false); };

					/** Number of represented characters,
						not neccessarily the size of any buffer */
		U32			Length(void) const;

		GPGString	&SPrintf(const char* fmt, ...);
		GPGString	&Cat(const char* fmt, ...);
		GPGString	&VSPrintf(const char* fmt,va_list ap);

const	GPGSTRING_I8	*Raw(void) const	{ return (GPGSTRING_I8 *)RawU8(); };
const	GPGSTRING_I8	*Raw8(void) const	{ return (GPGSTRING_I8 *)RawU8(); };
const	GPGSTRING_U8	*RawU8(void) const;
const	GPGSTRING_16	*Raw16(void) const;

	private:

		void			Init(void);

		void			ForceCase(bool upper);
static	U32				Strlen16(const GPGSTRING_16 *buffer);

		void			Confirm8(void);
		void			Confirm16(void);

virtual	GPGStringRep	*NewRep(void);
virtual	void			DeleteRep(GPGStringRep *pRep);

		GPGStringRep	*m_rep;
	};


GPGINLINE void GPGString::GPGStringRep::NewBuffer(long size)
{
	//* for U8 buffer, alloc double space for worst case multibyte

	GPGASSERT(!m_pDeallocateBuffer);
//	GPGASSERT(gs_pDeallocateFunction);

	if(gs_pAllocateFunction)
	{
		m_buffer8=(GPGSTRING_U8 *)GPGAllocate(2*size*sizeof(GPGSTRING_U8));
		m_buffer16=(GPGSTRING_16 *)GPGAllocate(size*sizeof(GPGSTRING_16));
		m_pDeallocateBuffer=gs_pDeallocateFunction;
	}
	else
	{
		m_buffer8=(GPGSTRING_U8 *)malloc(2*size*sizeof(GPGSTRING_U8));
		m_buffer16=(GPGSTRING_16 *)malloc(size*sizeof(GPGSTRING_16));
		m_pDeallocateBuffer=free;
	}

	m_stale8=true;
	m_stale16=true;
}


GPGINLINE void GPGString::GPGStringRep::DeleteBuffer(void)
{
	GPGASSERT(m_pDeallocateBuffer);

	if (m_buffer8)
	{
		m_pDeallocateBuffer(m_buffer8);
		m_buffer8 = NULL;
	}
	if (m_buffer16)
	{
		m_pDeallocateBuffer(m_buffer16);
		m_buffer16 = NULL;
	}
	m_pDeallocateBuffer=NULL;
}


GPGINLINE GPGString::GPGStringRep *GPGString::NewRep(void)
{
	GPGStringRep *pRep=NULL;

	if(gs_pAllocateFunction)
	{
		pRep=(GPGStringRep *)GPGAllocate(sizeof(GPGStringRep));
		pRep->m_pDeallocateRep=gs_pDeallocateFunction;
	}
	else
	{
		pRep=(GPGStringRep *)malloc(sizeof(GPGStringRep));
		pRep->m_pDeallocateRep=free;
	}

	pRep->m_references=1;
	pRep->m_pDeallocateBuffer=NULL;

	return pRep;
}


GPGINLINE void GPGString::DeleteRep(GPGStringRep *pRep)
{
	GPGDeallocateFunction *pDeallocateRep=pRep->m_pDeallocateRep;
	GPGASSERT(pDeallocateRep);
	pDeallocateRep(pRep);
}


GPGINLINE GPGString::GPGString(void)
	{
	m_rep=NULL;
	}

GPGINLINE void GPGString::Init(void)
	{
	m_rep=NewRep();
	m_rep->m_buffer8=NULL;
	m_rep->m_buffer16=NULL;
	m_rep->m_stale8=true;
	m_rep->m_stale16=true;
	}

GPGINLINE GPGString::GPGString(const GPGString &operand)
{
	if(operand.m_rep)
	{
		operand.m_rep->m_references++;
		m_rep=operand.m_rep;
	}
	else
		m_rep=NULL;
}

GPGINLINE GPGString::GPGString(const GPGSTRING_I8 *operand)
	{
	if(!operand)
		{
		Init();
		return;
		}
	m_rep=NewRep();
	m_rep->NewBuffer(GPGSTRLEN(operand)+1);
	GPGSTRCPY(m_rep->m_buffer8,operand);
	m_rep->m_stale8=false;
	m_rep->m_stale16=true;
	}

GPGINLINE GPGString::GPGString(const GPGSTRING_U8 *operand)
	{
	if(!operand)
		{
		Init();
		return;
		}
	m_rep=NewRep();
	m_rep->NewBuffer(GPGSTRLEN(operand)+1);
	GPGSTRCPY(m_rep->m_buffer8,operand);
	m_rep->m_stale8=false;
	m_rep->m_stale16=true;
	}

GPGINLINE GPGString::GPGString(const GPGSTRING_16 *operand)
	{
	if(!operand)
		{
		Init();
		return;
		}
	m_rep=NewRep();
	U32 length=Strlen16(operand)+1;
	m_rep->NewBuffer(length);
	memcpy(m_rep->m_buffer16,operand,length*2);
	m_rep->m_stale8=true;
	m_rep->m_stale16=false;
	}

GPGINLINE GPGString::GPGString(I32 operand)
	{
	if(!operand)
		{
		Init();
		return;
		}
	m_rep=NewRep();
	U32 length=12;
	m_rep->NewBuffer(length);
	GPG_SNPRINTF((char *)m_rep->m_buffer8,length,"%d",operand);
	m_rep->m_stale8=false;
	m_rep->m_stale16=true;
	}

GPGINLINE GPGString::~GPGString(void)
	{
	if(m_rep && --m_rep->m_references == 0)
		{
#if GPGSTRING_DEBUG
		Confirm8();
		GPGLOG("destruct %s\n",m_rep->m_buffer);
#endif
		m_rep->DeleteBuffer();
		DeleteRep(m_rep);
		}
	}

GPGINLINE GPGString &GPGString::operator=(const GPGSTRING_U8 *operand)
{
	// if someone else is still using the old data
	if(m_rep)
	{
		if(m_rep->m_references>1)
		{
			// leave old m_rep for others
			m_rep->m_references--;
			m_rep=NewRep();
		}
		else
		{
#if GPGSTRING_DEBUG
			Confirm8();
			GPGLOG("destruct %s\n",m_rep->m_buffer8);
#endif
			m_rep->DeleteBuffer();
		}
	}
	else
		Init();

	if(!operand)
	{
		Init();
		return *this;
	}
	m_rep->NewBuffer(GPGSTRLEN(operand)+1);
	GPGSTRCPY(m_rep->m_buffer8,operand);
	m_rep->m_stale8=false;
	m_rep->m_stale16=true;
	return *this;
}


GPGINLINE GPGString &GPGString::operator=(const GPGSTRING_16 *operand)
{
	// if someone else is still using the old data
	if(m_rep)
	{
		if(m_rep->m_references>1)
		{
			// leave old m_rep for others
			m_rep->m_references--;
			m_rep=NewRep();
		}
		else
		{
#if GPGSTRING_DEBUG
			Confirm8();
			GPGLOG("destruct %s\n",m_rep->m_buffer8);
#endif
			m_rep->DeleteBuffer();
		}
	}
	else
		Init();

	if(!operand)
	{
		Init();
		return *this;
	}
	U32 length=Strlen16(operand)+1;
	m_rep->NewBuffer(length);
	memcpy(m_rep->m_buffer16,operand,length*2);
	m_rep->m_stale8=true;
	m_rep->m_stale16=false;
	return *this;
}


GPGINLINE GPGString &GPGString::operator=(const GPGString &operand)
	{
	// protect against string=string
	if(operand.m_rep)
		operand.m_rep->m_references++;

	if(m_rep && --m_rep->m_references == 0)
		{
#if GPGSTRING_DEBUG
		Confirm8();
		GPGLOG("destruct %s\n",m_rep->m_buffer);
#endif
		m_rep->DeleteBuffer();
		DeleteRep(m_rep);
		}

	m_rep=operand.m_rep;
	return *this;
	}

#if 0
GPGINLINE bool GPGString::operator==(const GPGString &operand) const
	{
	if(Compare(operand))
		return false;
	return true;
	}
#endif

/*
GPGINLINE bool GPGString::operator!=(const GPGString &operand) const
	{
	if(Compare(operand))
		return true;
	return false;
	}
*/

GPGINLINE U32 GPGString::Strlen16(const GPGSTRING_16 *buffer)
	{
	U32 m=0;
	if(buffer)
		while(buffer[m])
			m++;
	return m;
	}

GPGINLINE U32 GPGString::Length(void) const
	{
	//* TODO multi-platform multibyte length (instead of convert to wide)
//	if(!m_rep->m_stale8)
//		return GPGSTRLEN_MB(m_rep->m_buffer8,);

	if(m_rep)
	{
		if(m_rep->m_stale16)
			((GPGString*)this)->Confirm16();

		if(!m_rep->m_stale16)
			return Strlen16(m_rep->m_buffer16);
	}

	return 0;
	}

GPGINLINE const GPGSTRING_U8 *GPGString::RawU8(void) const
	{
	if(!m_rep)
		return (const GPGSTRING_U8 *)"";

	((GPGString *)this)->Confirm8();	// bypass const
	return m_rep->m_buffer8;
	}

GPGINLINE const GPGSTRING_16 *GPGString::Raw16(void) const
	{
	if(!m_rep)
		return NULL;

	((GPGString *)this)->Confirm16();	// bypass const
	return m_rep->m_buffer16;
	}

GPGINLINE void GPGString::Confirm8(void)
	{
	if(!m_rep)
		return;

	//* JIT conversion
	if(m_rep->m_stale8)
		{
		if(m_rep->m_stale16)
			(*this)="";
		else
			{
			GPGSTR_WC2MB(m_rep->m_buffer8,m_rep->m_buffer16,
												Strlen16(m_rep->m_buffer16)+1);
/*
			U32 m,length=Strlen16(m_rep->m_buffer16)+1;
			for(m=0;m<length;m++)
				{
				if(m_rep->m_buffer16[m]<256)
					m_rep->m_buffer8[m]=
									(GPGSTRING_U8)m_rep->m_buffer16[m];
				else
					m_rep->m_buffer8[m]='_';
				}
*/
			m_rep->m_stale8=false;
			}
		}
	}

GPGINLINE void GPGString::Confirm16(void)
	{
	if(!m_rep)
		return;

	//* JIT conversion
	if(m_rep->m_stale16)
		{
		if(m_rep->m_stale8)
			(*this)="";

		GPGASSERT(!m_rep->m_stale8);
		GPGSTR_MB2WC(m_rep->m_buffer16,m_rep->m_buffer8,
												GPGSTRLEN(m_rep->m_buffer8)+1);
/*
		U32 m,length=GPGSTRLEN(m_rep->m_buffer8);
		for(m=0;m<length;m++)
			m_rep->m_buffer16[m]=m_rep->m_buffer8[m];
*/
		m_rep->m_stale16=false;
		}
	}

//* TODO optimize array usage
GPGINLINE void GPGString::ForceCase(bool upper)
	{
	if(!m_rep)
		return;

	GPGString temp=*this;
	if(!m_rep->m_stale8)
		{
		*this=(const GPGSTRING_U8 *)temp;	// replicate

#ifdef GPGSTR_MB2UPPER
		if(upper)
			GPGSTR_MB2UPPER(m_rep->m_buffer8);
		else
			GPGSTR_MB2LOWER(m_rep->m_buffer8);
#else
		//* manual 8-bit version
		U32 m,length=GPGSTRLEN(m_rep->m_buffer8);
		for(m=0;m<length;m++)
//			if(m_rep->m_buffer8[m] < 0x7f)
				m_rep->m_buffer8[m]=upper?
					toupper(m_rep->m_buffer8[m]):tolower(m_rep->m_buffer8[m]);
#endif
		}
	else if(!m_rep->m_stale16)
		{
		*this=(const GPGSTRING_16 *)temp;	// replicate

#ifdef GPGSTR_WC2UPPER
		if(upper)
			GPGSTR_WC2UPPER(m_rep->m_buffer16);
		else
			GPGSTR_WC2LOWER(m_rep->m_buffer16);
#else
		//* manual 16-bit version
		U32 m,length=Strlen16(m_rep->m_buffer16);
		for(m=0;m<length;m++)
			if(m_rep->m_buffer16[m] < 0x7f)
				m_rep->m_buffer16[m]=upper?
					toupper(m_rep->m_buffer16[m]):tolower(m_rep->m_buffer16[m]);
#endif
		}
	else
		(*this)="";
	}

GPGINLINE bool operator == (const GPGString &s1, const GPGString &s2)
{
	if(s1.Compare(s2)) { return false; }
	return true;
}

GPGINLINE bool operator == (const char *s1, const GPGString &s2)
{
	if(((const GPGString)s1).Compare(s2)) { return false; }
	return true;
}

GPGINLINE bool operator == (const GPGString &s1, const char *s2)
{
	if(s1.Compare((const GPGString)s2)) { return false; }
	return true;
}

GPGINLINE bool operator != (const GPGString &s1, const GPGString &s2)
{
	if(s1.Compare(s2)) { return true; }
	return false;
}

GPGINLINE bool operator != (const char *s1, const GPGString &s2)
{
	if(((const GPGString)s1).Compare(s2)) { return true; }
	return false;
}

GPGINLINE bool operator != (const GPGString &s1, const char *s2)
{
	if(s1.Compare((const GPGString)s2)) { return true; }
	return false;
}

#endif /* __GPGString_h__ */
