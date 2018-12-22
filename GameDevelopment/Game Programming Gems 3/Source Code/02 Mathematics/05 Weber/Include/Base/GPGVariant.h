
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

#ifndef __GPGVariant_h__
#define __GPGVariant_h__


#define GPGVARIANT_DEBUG			FALSE	//* verbosity
#define GPGOWNER_DEBUG				FALSE	//* verbosity

#define GPGVARIANT_CLONING			TRUE	//* HParser uses it
#define GPGVARIANT_PACKFUNCTION		FALSE

//* register on construction if not done manually (may not cross DLL's)
#define GPGVARIANT_AUTOREGISTER		TRUE


/// \todo replace U32 with GPGVariantID where necessary
typedef	U32 GPGVariantID;

#define GPG_VTYPE(T)		GPGVariantType< T >::GetStaticID()
#define GPG_ISTYPE(T,V)		(V.IsType(GPG_VTYPE(T)))

/******************************************************************************

	GPGVariant is a typesafe version of (void *)

	NOTE requires compiler support of templated members

	NOTE this is not a memory management or reference counting object

Usage:

	GPGVariant variant=new int(7);	// 7 is just an arbitrary test value

	char *pChar=NULL;
	GPGRESULT ok1=variant.CopyPointerTo(&pChar);	// fails, pchar unchanged

	int *pInt=NULL;
	GPGRESULT ok2=variant.CopyPointerTo(&pInt);	// points pint at the 7

	float *pFloat=NULL;
	GPGRESULT ok3=variant.CopyPointerTo(&pFloat);	// fails, pfloat unchanged

Ignoring result codes, you can also use direct assignment using implied cast:

	char *pChar=variant;							// pointer set to NULL
	int *pInt=variant;								// valid pointer returned
	float *pFloat=variant;							// pointer set to NULL

You can also copy the contents of the pointer implicitly:

	float fvalue=variant;							// if float type, set value

	In this case, if 'variant' contains a float, it's value will be copied.
	Otherwise, the copied result is an object of that type using an
	no-argument constructor.  Since native types don't have arg-less
	constructors with default values, the result of a mismatch is undefined.

	Since the implicit-cast methods can try both the pointed-to and
	not-pointed-to types, it is possible that you could get a false-positive
	match when a deference was not intended.

	For safe usage, stick with CopyPointerTo().

To bypass type checking:

	other *pOther=NULL;
	GPGRESULT ok3=variant.CopyPointerTo((void **)&pOther);

	NOTE Objects typecast to/from a real (void *) are not type checked.

Remember:

	(GPGVariant) is like (void *), so (GPGVariant *) is like (void **).
	If you are thinking (void *), don't use (GPGVariant *).

	GPGVariant is very picky.  Like a general (void *),
	you are not allowed to pass between base and derived classes.
	Also, separate types that may be equivalent, such as long and int,
	are not interchangeable.
	However, if you register using an GPGTypeMaster, you can specify
	multiple types for the same name and force types to be treated as
	equivalent.  This should only be done for true pointer equivalance.

	Simple single-step auto-conversion functions can be specified using
	GPGTypeMaster::RegisterConvertor();

	Since the default return for a mismatch is NULL, any GPGVariant's
	set to a NULL pointer of any type are indistinguishable from each other.
	So, any GPGVariant set to NULL can be considered to be a special
	"NULL type".
	In the following example, vs1 and vs2 will report as different types
	and vs2 and vs3 will report as the same type.
		GPGVariant vs1=new int(7);
		GPGVariant vs2=((int *)NULL);
		GPGVariant vs3=((float *)NULL);

	TODO fix the following (possible?)
	WARNING A non-specific dereference may confuse the compiler:
		GPGVariant variant=new int(7);
		*(variant)=2;			// potential compiler error
		*(int *)(variant)=2;	// OK, but not as flexible

******************************************************************************/


typedef BOOL (*GPGTypeConvertor)(void *in,void *out);


class GPGCoreArray;
class GPGVariant;


class GPGTypeMaster
{
	private:
	class GPGTypeMasterEntry
	{
		public:
			GPGBaseType				*Type(void)
									{
										return m_pType;
										//return m_typeList.GetHead();
									}
			void					AddType(GPGBaseType *pType)
									{
										m_pType = pType;
#if 0
										m_typeList.Append(pType);
										if(pType != Type())
										{
											Type()->AddNamesFrom(*pType);
										}
#endif
									}
#if 0
			GPGList<GPGBaseType>	&List(void)
									{
										return m_typeList;
									}
#endif
		private:
			GPGBaseType				*m_pType;
			//GPGList<GPGBaseType>	m_typeList;
	};

	class GPGTypeConversion
	{
		public:
			GPGTypeConvertor	m_convertor;
			U32					m_idOut;
			U32					m_idIn;
	};

	public:
							GPGTypeMaster(void);
							~GPGTypeMaster(void);
		GPGBaseType			*Register(GPGBaseType *type);
		void				RegisterConvertor(GPGTypeConvertor convertor,
														U32 idIn,U32 idOut);
		GPGBaseType			*LookupName(GPGString name, UWORD *index=NULL);
		GPGBaseType			*LookupID(U32 id);
		void				Clear(void);

							template<class T>
		void				NewType(GPGType<T> **pType)
							{
								*pType = new GPGType<T>;
								m_typeList.Append(*pType);
							}

		bool				SafeToClearOwner(U32 owner);
		GPGTypeConvertor	LookupConvertor(U32 idIn,U32 idOut);

		GPGString			Dump(void);

	private:
		GPGCoreArray		*m_pIdArray;
		GPGCoreArray		*m_pConvertorArray;
		GPGList<GPGBaseType> m_typeList;
};


class GPGBaseVariantType
{
	public:
		U32		GetClassID(void) const			{ return m_classid; };
		U32		GetPointerClassID(void) const	{ return m_pointerclassid; };
		void	SetPointerClassID(U32 set)		{ m_pointerclassid=set; };

static	U32		BaseRegister(GPGTypeMaster *master,U32 owner,
						GPGBaseType *pType,GPGBaseType **ppStaticType,
						GPGString *name=NULL);

virtual	void				*GetRawPointer(void)							=0;
virtual	void				DeletePointer(void)								=0;
virtual	GPGBaseType			*BaseType(void)									=0;

virtual	GPGBaseVariantType *Clone(void)										=0;
#if GPGVARIANT_CLONING
virtual	GPGBaseVariantType *Clone(void *newptr)								=0;
virtual	GPGBaseVariantType *SubClone(U32 offset=0)							=0;
#endif

#if GPGVARIANT_PACKFUNCTION
virtual	U32					Pack(GPGVariant variant,U8 *buffer,U32 limit)	=0;
virtual	U32					UnPack(GPGVariant variant,U8 *buffer,U32 limit)	=0;
#endif
	protected:
		void				SetClassID(U32 set)				{ m_classid=set; };

	private:
		U32					m_classid;
		U32					m_pointerclassid;
};


#if GPGVARIANT_PACKFUNCTION
typedef U32 (*GPGVariantPackFunction)(GPGVariant variant,U8 *buffer,U32 limit);
typedef U32 (*GPGVariantUnPackFunction)(GPGVariant variant,U8 *buffer,U32 limit);
#endif


template<class T> class GPGVariantType: public GPGBaseVariantType
	{
	public:
					GPGVariantType(T *initval)
					{
#if GPGVARIANT_AUTOREGISTER
						if(!ms_pType)
						{
#if GPGVARIANT_DEBUG
							GPGLOG("AutoRegister %s\n",hexname);
#endif
							BaseRegister(GPGVariant::GetTypeMaster(),0,
													new GPGType<T>,&ms_pType);
						}
#endif

						GPGASSERT(ms_pType);
						SetClassID(GetStaticID());
//fprintf(stderr,"GPGVariantType id %u\n", GetClassID());
						//SetRawPointer((void *)initval);
						m_tptr=initval;
					}

		T			*GetPointer(void)		{ return m_tptr; }
		void		*GetRawPointer(void)	{ return (void *)m_tptr; }

static	U32			Register(GPGString name,GPGTypeMaster *master, U32 owner=0)
					{
						return BaseRegister(master,owner,
												new GPGType<T>,&ms_pType,&name);
					}
#if FALSE
static	U32			Register(GPGString name,GPGTypeMaster *master, U32 owner=0)
					{
#ifdef OLDWAY
/*
						if(!master)
							return 0;

						ms_type.AddName(name);

						ms_pType = master->Register(&ms_type);
						if(ms_pType != &ms_type)
						{
							//ms_pType->AddNamesFrom(ms_type);
#if GPGVARIANT_DEBUG
							GPGLOG("Register(\"%s\") 0x%x added type to 0x%x\n",
										name.Raw8(),&ms_type,ms_pType->ID());
#endif
						}
						else
						{
#if GPGVARIANT_DEBUG
							GPGLOG("Register(\"%s\") 0x%x original assigned"
									" to 0x%x\n",
										name.Raw8(),&ms_type,ms_pType->ID());
#endif
						}

//						m_name=name;
//						m_staticvalue=master->Register(name,
//														(U32)&m_staticvalue);
						return 1;
*/
#else
						if(!master) { return 0; }

						GPGBaseType *pType;
						if(!ms_pType)
						{
							pType = new GPGType<T>;
//							fprintf(stderr,"set owner %u\n", owner);
							pType->SetOwner(owner);
						}
						else
						{
							pType = ms_pType;
						}
						pType->AddName(name);
						ms_pType = master->Register(pType);
						if(pType != ms_pType)
						{
							//delete pType;
							return 0;
						}
						return 1;

/*
						GPGBaseType *pType;
						if(!ms_pType)
						{
							pType = new GPGType<T>;
#if GPGOWNER_DEBUG
							fprintf(stderr,"set owner %u\n", owner);
#endif
							pType->AddName(name);
							pType->SetOwner(owner);
							ms_pType = master->Register(pType);
							if(ms_pType != pType)
							{
								delete pType;
							}
						}
						else
						{
							ms_pType->AddName(name);
						}
						return 1;
*/
#endif
					}
#endif

static	U32			GetStaticID(void)
					{
						GPGASSERT(ms_pType);
						return ms_pType->ID();
					}

virtual	void		DeletePointer(void)
					{	delete (T*)m_tptr;
						m_tptr=NULL; }

//TODO: remove pack and unpack
#if GPGVARIANT_PACKFUNCTION
static	void		SetPackFunction(GPGVariantPackFunction fp)
					{	m_packfunction=fp; }
static	void		SetUnPackFunction(GPGVariantUnPackFunction fp)
					{	m_unpackfunction=fp; }

virtual	U32			Pack(GPGVariant variant,U8 *buffer,U32 limit)
					{
						if(m_packfunction)
							return m_packfunction(variant,buffer,limit);
						return 0;
					}
virtual	U32			UnPack(GPGVariant variant,U8 *buffer,U32 limit)
					{
						if(m_unpackfunction)
							return m_unpackfunction(variant,buffer,limit);
						return 0;
					}
#endif

virtual	GPGBaseVariantType *Clone(void)
					{
						GPGBaseVariantType *clone=new GPGVariantType<T>(m_tptr);
						clone->SetPointerClassID(GetPointerClassID());
						return clone;
					}

#if GPGVARIANT_CLONING
virtual	GPGBaseVariantType *Clone(void *newptr)
					{
						GPGBaseVariantType *clone;
						if(newptr)
							clone=new GPGVariantType<T>((T *)newptr);
						else
							clone=new GPGVariantType<T>(m_tptr);

						clone->SetPointerClassID(GetPointerClassID());
						return clone;
					}

virtual	GPGBaseVariantType *SubClone(U32 offset=0)
					{
						U8 *offset_tptr=(U8 *)m_tptr;
						offset_tptr+=offset;
						GPGBaseVariantType *clone=
									new GPGVariantType<T>((T *)offset_tptr);

						clone->SetPointerClassID(GetPointerClassID());
						return clone;
					}
#endif

virtual	GPGBaseType			*BaseType(void)
					{
						GPGASSERT(ms_pType);
						return ms_pType;
					}
static	GPGBaseType			*StaticBaseType(void)
					{
						GPGASSERT(ms_pType);
						return ms_pType;
					}
	private:
		T							*m_tptr;

//static	U32							m_staticvalue;
//static	GPGString					m_name;
#ifdef OLDWAY
static	GPGType<T>					ms_type;
#endif
static	GPGBaseType					*ms_pType;

#if GPGVARIANT_PACKFUNCTION
static	GPGVariantPackFunction		m_packfunction;
static	GPGVariantUnPackFunction	m_unpackfunction;
#endif
};


class GPGVariant
{
	public:
					// optional arg allows GPGVariant to be assigned as NULL
					GPGVariant(U32 ignored=0)
					{
						GPGASSERT(ignored==0);
						m_pBase=NULL;
					};

					GPGVariant(const GPGVariant &vs)
					{
						m_pBase=NULL;
						operator=(vs);
					};

virtual				~GPGVariant(void);
		void		operator=(U32 ignored);
		void		operator=(const GPGVariant &vs);

#if GPGVARIANT_CLONING
					/// create new m_pBase of same type and specified size
		void		*Malloc(U32 size);
					/** return object of same type but offset in memory
					(element in array) */
		GPGVariant	GetOffsetElement(U32 offset);
#endif


static	void		SetTypeMaster(GPGTypeMaster *pTypeMaster)
					{	m_pTypeMaster=pTypeMaster; }
static	GPGTypeMaster	*GetTypeMaster(void)
					{	return m_pTypeMaster; }

					/// returns true if argument is of the same type
//		BOOL		IsPointerToTypeOf(const GPGVariant &vs);
		BOOL		IsType(U32 id)
					{	return id==m_pBase->GetClassID();	};
		BOOL		IsEmpty(void)
					{	return m_pBase==NULL;	};
		U32			GetClassID(void)
					{	return m_pBase? m_pBase->GetClassID(): 0; };

					/// delete the object pointed to as whatever type it is
		void		DeletePointer(void)
					{	if(m_pBase)
							m_pBase->DeletePointer();
						operator=(0L); }

		GPGBaseType	*BaseType(void)
					{
						return m_pBase? m_pBase->BaseType(): NULL;
					}

					/// bypass type checking
		void		*GetRawPointer(void);

#if GPGVARIANT_PACKFUNCTION
		U32			Pack(U8 *buffer,U32 limit)
					{
						return m_pBase? m_pBase->Pack(*this,buffer,limit): 0;
					}
		U32			UnPack(U8 *buffer,U32 limit)
					{
						return m_pBase? m_pBase->UnPack(*this,buffer,limit):0;
					}
#endif

				//* the following doesn't work, trying to do: *variant=value;
/*
		T		operator*(void)
				{
					T *pointer=NULL;
					CopyPointerTo(&pointer);
					return *pointer;
				}
*/

				//		`template member functions'

					template<class T>
					GPGVariant(T *tptr)
					{
						m_pBase=NULL;
						operator=(tptr);
					}

					template<class T>
		T			*operator=(T *tptr)
					{
						if(m_pBase)
						{
							delete m_pBase;
							m_pBase=NULL;
						}
						if(tptr)
							m_pBase=new GPGVariantType<T>(tptr);
						return tptr;
					}

		GPGVariant	*PointTo(GPGVariant *vs);

#if FALSE
				// alleged MAC equivelent to following function
				template<class T>
		T*			operator *(void)

				/*** 20000525 jpweber: Does this really work on the Mac?
					This appears to oppose Stroustrup references below.
					Templates are not supposed to trigger on a return value. */

#endif
#if FALSE
				/// transparent return, but without result code
				/// Conversion Operator: see Stroustrup #2,pg232; #3,pg275
				template<class T>
					operator T*(void)
					{
						T *pointer=NULL;
						CopyPointerTo(&pointer);

//	printf("T* pointer=0x%x\n",(U32)pointer);

						if(pointer)
							return pointer;

						T **ppointer=NULL;
						CopyPointerTo(&ppointer);
//	printf("T* ppointer=0x%x\n",(U32)*ppointer);
						return ppointer? *ppointer: NULL;
					}

				template<class T>
					operator T(void)
					{
						T *pointer=NULL;
						CopyPointerTo(&pointer);
//	printf("T pointer=0x%x\n",(U32)pointer);
						if(pointer)
							return *pointer;
						T t;
						return t;
					}
#endif

					template<class T>
		GPGRESULT	CopyPointerTo(T **tptr) const
					{
						U32 voidT=GPG_VTYPE(void);
						U32 staticT=GPG_VTYPE(T);
#if GPGVARIANT_DEBUG
						GPGLOG("GPGVariant copy type 0x%x to 0x%x"
													" (\"%s\" to \"%s\")\n",
								m_pBase->GetClassID(),staticT,
								m_pTypeMaster->LookupID(m_pBase->GetClassID())
														->NameDump().Raw8(),
								m_pTypeMaster->LookupID(staticT)
														->NameDump().Raw8() );
#endif
						if(!m_pBase)
						{
#if GPGVARIANT_DEBUG
							GPGLOG("GPGVariant::CopyPointerTo()"
													" m_pBase==NULL\n");
#endif
							return GPG_E_NOT_INITIALIZED;
						}
						// NOTE if either is (void *), ignore check
						if(staticT==voidT || m_pBase->GetClassID()==voidT ||
								m_pBase->GetClassID()==staticT)
						{
							*tptr=((GPGVariantType<T> *)m_pBase)->GetPointer();
							return GPG_OK;
						}
#if GPGVARIANT_DEBUG
						else
						{
							GPGLOG("GPGVariant::CopyPointerTo()"
													" invalid pointer\n");
						}
#endif
						return GPG_E_INVALID_POINTER;
					}

					/// T must have an operator=(T)
					//* TODO conversion operators
					template<class T>
		GPGRESULT	ConvertInto(T *tptr)
					{
						if(!tptr)
							return GPG_E_INVALID_POINTER;

						U32 voidT=GPG_VTYPE(void);
						U32 staticT=GPG_VTYPE(T);

/*						GPGLOG("holder=0x%x T=0x%x void=0x%x"
													" I32=0x%x F32=0x%x\n",
								m_pBase->GetClassID(),staticT,voidT,
								GPG_VTYPE(I32),GPG_VTYPE(F32));
*/
						if(!m_pBase)
						{
							return GPG_E_NOT_INITIALIZED;
						}

						if(staticT==voidT || m_pBase->GetClassID()==voidT ||
								m_pBase->GetClassID() == staticT)
						{
							// if either is (void *), ignore check
							*tptr=*((GPGVariantType<T> *)m_pBase)->GetPointer();
							return GPG_OK;
						}
						else if(m_pTypeMaster)
						{
							GPGTypeConvertor convertor=m_pTypeMaster
									->LookupConvertor(
											m_pBase->GetClassID(),staticT);

							if(convertor)
							{
								convertor(((GPGVariantType<T> *)m_pBase)
														->GetPointer(),tptr);
								return GPG_OK;
							}
#if GPGVARIANT_DEBUG
						GPGLOG("GPGVariant cannot convert type 0x%x to 0x%x"
													" (\"%s\" to \"%s\")\n",
								m_pBase->GetClassID(),staticT,
								m_pTypeMaster->LookupID(m_pBase->GetClassID())
														->NameDump().Raw8(),
								m_pTypeMaster->LookupID(staticT)
														->NameDump().Raw8() );
#endif
						}

						return GPG_E_INVALID_POINTER;
					}
		GPGRESULT	CopyInto(GPGVariant &variant);

	private:
/// \todo turn back to private
	public:
		GPGBaseVariantType	*m_pBase;
static	GPGTypeMaster		*m_pTypeMaster;
	};


template<class T> T GPGDereference(GPGVariant &variant)
{
	T *pointer=NULL;
	variant.CopyPointerTo(&pointer);

//	printf("T pointer=0x%x id=0x%x\n",(U32)pointer,variant.GetClassID());

	if(pointer)
		return *pointer;

	return (T)0;
}


template<class T> T GPGCast(GPGVariant &variant)
{
	T pointer=NULL;
	variant.CopyPointerTo(&pointer);

//	printf("T* pointer=0x%x id=0x%x\n",(U32)pointer,variant.GetClassID());

	if(pointer)
		return pointer;

//	T *ppointer=NULL;
//	variant.CopyPointerTo(&ppointer);
//	printf("T** ppointer=0x%x\n",(U32)ppointer);
//	return ppointer? *ppointer: NULL;

	return NULL;
}


void GPGVariantType<void>::DeletePointer(void)
{	GPGDeallocate(m_tptr);
	m_tptr=NULL; }


// Stroustrup C++ 2nd Ed r.9.4 pg548
// "Static members of a global class have external linkage."
// however,
// Stroustrup C++ 2nd Ed r.3.3 pg484
// "There must be exactly one definition for each function, object,
// class and enumerator used in a program."
// This compiles under g++ and VC6, so does this link into one definition
// even after it is included in every file?
// The same seems to apply to enumerators found in headers.
#ifdef OLDWAY
template<class T> GPGType<T> GPGVariantType<T>::ms_type;
template<class T> GPGBaseType *GPGVariantType<T>::ms_pType
	= &(GPGVariantType<T>::ms_type);
#else
template<class T> GPGBaseType *GPGVariantType<T>::ms_pType = NULL;
#endif

#if GPGVARIANT_PACKFUNCTION
template<class T> GPGVariantPackFunction GPGVariantType<T>::m_packfunction;
template<class T> GPGVariantUnPackFunction GPGVariantType<T>::m_unpackfunction;
#endif

#endif /* __GPGVariant_h__ */
