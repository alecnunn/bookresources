// VMSystem.h: Schnittstelle für die Klasse CSystem.
//
//////////////////////////////////////////////////////////////////////

#ifndef _atomosCSystem_
#define _atomosCSystem_

// system is part of the engine:
// there are systems for app, gui, graphics, audio, natural language etc...
#include "Object.h"


namespace atomos
{
interface IEngine;
class CClass;
/*
interface ISystem : public INamedObject  
{
public:

	virtual IEngine*	GetEngine()=0;
	virtual BOOL		SetEngine(IEngine* pEngine)=0;
	virtual BOOL		IsLoaded()=0;
	virtual BOOL		IsStarted()=0;

	virtual BOOL		SetSystemHandle(void* pSystemHandle)=0;
	virtual void*		GetSystemHandle()=0;

	virtual IObject*	CreateObject(UINT32 idClass)=0;
	virtual IObject*	CreateObject(CHAR8* pszClassName)=0;

	virtual IClasses*	GetClasses()=0;
	virtual IClass*		FindClass(UINT32 idClass)=0;
	virtual	BOOL		AppendClass( IClass* pClass)=0;

	virtual void			SetDataLocations(IDataLocations* pDataLocations)=0;
	virtual IDataLocations*	GetDataLocations()=0;
	virtual UINT32		ReadConfiguration(ATOM_CHAR* pszFileName)=0;

};
*/

class ATOMBASE_DLLCLASS CSystem : public CNamedObject  
{



public:
	// IObject
	enum { CID = CID_System };
	static const CClass classCSystem;
	virtual const IClass*		GetClass();

	virtual void			Terminate();

	// ISystem
	virtual IEngine*	GetEngine();
	virtual BOOL		SetEngine(IEngine* pEngine);
	virtual BOOL		IsLoaded() { return(false); }
	virtual BOOL		IsStarted() { return(false); }

	virtual BOOL		SetSystemHandle(void* pSystemHandle);
	virtual void*		GetSystemHandle();
	virtual IObject*	CreateObject(UINT32 idClass);
	virtual IObject*	CreateObject(CHAR8* pszClassName);

	virtual IClasses*	GetClasses();
	virtual IClass*		FindClass(UINT32 idClass);
	virtual	BOOL		AppendClass( IClass* pClass);

	virtual void			SetDataLocations(IDataLocations* pDataLocations);
	virtual IDataLocations*	GetDataLocations();


	virtual UINT32		ReadConfiguration(ATOM_CHAR* pszFileName);

	// CTOR / DTOR
	CSystem();
	~CSystem();


protected:
	
	IEngine*			m_pEngine;
	void*				m_pSystemHandle;
	IClasses*			m_pClasses;

//	CClasses			m_Classes;
//	ClassMap			m_Classes;

	IDataLocations*		m_pDataLocations; // data path for this plugin

};



} // namespace

#endif // _atomosCSystem_
