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

#ifndef __GPGType_h__
#define __GPGType_h__

#define GPGTYPE_USEMAP
#define GPGTYPE_DEBUG	FALSE
#define GPGTYPE_CHECK	FALSE

extern UWORD GPGStringHash(const GPGString &string);

#define GPGTYPE_NONE		(0)
#define GPGTYPE_VARSIZE		(1 << 0)
#define GPGTYPE_OPTIONAL	(1 << 1)
#define GPGTYPE_ALLBITS		0xFFFF
/**************************************************************************//**
\addtogroup TypeSystem The Compile Time Type System

This system enables both the state extensibility framework and the functional
dispatch mechanism which both in turn enable plugins.  Both of these systems
require run time type functionality beyond that directly usable with the
C++ rtti mechanism.
Namely, in addition to name it is necessary to:
	- handle multiple plugins defining like type without requiring the user to follow difficult to track and enforce conventions (like using == on type_info objects instead of pointers to type_info objects).
	- control memory allocation
	- know whether or not construction is necessary
	- to construct and destruct pre-allocated memory blocks
	- to serialize types

In addition to this, scripting language support requires bidirectional string
to type convertions.

In this system a type is basicly a binding, or tuple, of the following:
	- C++ type (class)
	- string name
	- memory size
	- serialization functions
	- construction and destruction functions

Explicit handling and unification of duplicate type information in an
environment of independently developed plugins is necessary and provided.
(see GPGTypeMaster).

*//***************************************************************************/

class GPGVariant;

class GPGTypeHelp
{
	public:
				GPGTypeHelp(void)
				{
#if GPGTYPE_CHECK==TRUE && GPG_OSTYPE==GPG_UNIX

					U8 current_stack;
					if((unsigned int)this >
						(unsigned int)&current_stack)
					{
						throw GPGException(
							"attempt to create GPGTypeHelp object"
							" on stack");
					}
#endif
				}
virtual			~GPGTypeHelp(void)					{ }
virtual	bool	GetConstruct(void)					{ return FALSE; }
virtual	void	Construct(void *instance)			{ }
virtual	void	Destruct(void *instance)			{ }
virtual	U32		Flag(void)							{ return GPGTYPE_NONE; }
virtual	bool	Out(void *from,GPGMessage &message)	{ return 0; }
virtual	bool	In(void *to,GPGMessage &message)
				{
#if GPGTYPE_DEBUG
					GPGLOG("GPGTypeHelp::In\n");
#endif
					return 0;
				}
};


template <class T>
class GPGTypeHelpConstructable : public GPGTypeHelp
{
	public:
virtual	bool	GetConstruct(void)					{ return TRUE; }
virtual	void	Construct(void *instance)			{ new(instance)T; }
virtual	void	Destruct(void *instance)			{ ((T *)instance)->~T(); }
};


template <class T>
class GPGConstructableType
{
	public:
		void	*operator new(size_t s, void *p)	{ return p; }
		void	operator delete(void *ptr, void *p)	{ }
		void	*operator new(size_t s)				{ return ::new T; }
		void	operator delete(void *ptr)			{ ::delete (T *)ptr; }
};


class GPGBaseType
{
	public:
					GPGBaseType(void);
virtual				~GPGBaseType(void);
		void		AddName(const GPGString &name);
		bool		LookupName(const GPGString &name);
		void		AddNamesFrom(GPGBaseType &other);
		void		RewindNames(void);
		bool		NextName(GPGString &name);
		UWORD		NameCount(void);
		void		SetID(const U32 &id)			{ m_id = id; }
const	U32			&ID(void) const					{ return m_id; }
		void		SetOwner(const U32 &owner)		{ m_owner = owner; }
const	U32			&Owner(void) const				{ return m_owner; }
		GPGString	NameDump(void);
		void		Create(GPGVariant *v);
		void		BaseDestroy(void *t);

virtual	void		Destroy(GPGVariant *v)									=0;
virtual	void		PointToInstance(GPGVariant *v, void *instance)			=0;
virtual	void		Copy(GPGVariant *vto, GPGVariant *vfrom)				=0;
virtual	UWORD		Size(void)						{ return m_size; }

virtual	void		SetHelp(GPGTypeHelp *pHelp,U32 owner);
virtual	bool		GetConstruct(void);
virtual	void		Construct(void *instance);
virtual	void		Destruct(void *instance);
virtual	U32			Flag(void);
virtual	bool		Out(void *from, GPGMessage &message);
virtual	bool		In(void *to, GPGMessage &message);

	private:
		U32							m_id;
		U32							m_owner;
#ifdef GPGTYPE_USEMAP
		GPGMap<GPGString,UWORD>		m_nameMap;
#else
		GPGList<GPGString>			m_nameList;
		GPGListContext				m_context;
#endif
	protected:
		U32							m_size;
		GPGTypeHelp					*m_pHelp;

};


template <class T>
class GPGType : public GPGBaseType
{
	public:
				GPGType(void)							{ m_size=sizeof(T); }
virtual			~GPGType(void)							{ }
virtual	void	Destroy(GPGVariant *variant)
				{
					T *t;
					if(GPG_OK == variant->CopyPointerTo(&t))
						BaseDestroy(t);
				}

virtual	void	PointToInstance(GPGVariant *variant, void *instance)
				{	*variant = (T *)(instance); }

virtual	void	Copy(GPGVariant *vto, GPGVariant *vfrom)
				{
					GPGASSERT(vfrom->GetClassID() == ID());
					GPGASSERT(vto->GetClassID() == ID());
					*((T *)(vto->GetRawPointer())) =
								*((T *)(vfrom->GetRawPointer()));
				}
};


void GPGType<void>::Copy(GPGVariant *vto, GPGVariant *vfrom)
{
	/* NOOP */
}


GPGType<void>::GPGType(void)
{ m_size=0; }

#endif /* __GPGType_h__ */
