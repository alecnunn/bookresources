// Atomos Interfaces
// --------------------------------------------------------------------------------------------------------
// System:		ATOMBASE
// Description:	...
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/atomos/index.htm
// Version:		0401
// Author:		Frank Luchs
// History:
//				1996-05-07	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of the Atomos Application Base
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------
#ifndef _atomos_interfaces_
#define _atomos_interfaces_

#include "atombase_types.h"
#include "atombase_defines.h"



namespace atomos
{




// forward declarations
interface IObject;
interface IObjects;
interface INamedObject;
interface INode;
interface INodes;
interface IGraph;
interface IClass;
interface IClasses;
interface ISystem;
interface ISystems;
interface IPin;
interface IPins;
interface IConnection;
interface IConnections;
interface IProcessor;
interface IProcessors;
interface ISignal;
interface ISignals;
interface ISignalGraph;
interface IEngine;
interface IDataLocation;
interface IDataLocations;
interface IBuffer;
interface ITuple4f;
interface IScalar;
interface ICHAR8Scalar;
interface IFLOAT32Scalar;
interface IUINT32Scalar;
interface IFloats;
interface IIntegers;
interface IString8;
interface IStrings8;
interface IPlatformObject;
interface IApplication;
interface IWindow;
interface IConsole;
interface IFile;
interface IFileLister;
interface IColor;
interface IDrawing;
interface IImage;
interface IRenderer;
interface IShape;
interface IShapes;


// =================================================================================
// base interfaces
// =================================================================================

// -----------------------------------------------------------------------------------
// IObject
// -----------------------------------------------------------------------------------
/* Description: Base Object. Defines common methods and has no member data
/* 
Methods:
	
GetClass()


*/
// -----------------------------------------------------------------------------------
interface IObject
{
	virtual const IClass*	GetClass()=0;
	virtual BOOL			Initialize(IObject* pob=NULL)=0;
	virtual void			Terminate()=0;
	virtual void			Reset()=0;

	virtual UINT32			GetID()=0;
	virtual void			SetID(UINT32 nID)=0;

	virtual IObject*		GetOwner()=0;
	virtual void			SetOwner(IObject* pOwner)=0;

	virtual UINT32			Sync(IObject* pTimer)=0;
	virtual void			Update(UINT32 nContext=0)=0;
	virtual UINT32			Process(IObject* pContext)=0;
	virtual UINT32			Write(ISignal* pStream)=0;

	virtual BOOL			SetParameter(CHAR8* pszName, CHAR8* pszValue)=0;
};

// -----------------------------------------------------------------------------------
// IObjects
// -----------------------------------------------------------------------------------
interface IObjects : public IObject 
{
public:

	virtual IObject*		GetAt(UINT32 nIndex)=0;

	virtual IObject*		GetObjectFromID(UINT32 nThatID)=0;
	virtual IObject*		GetObjectFromClassID(UINT32 idClass, UINT32 nIndex=0)=0;
	virtual IObject*		GetObjectLike(UINT32 idClass, UINT32 nIndex=0)=0;
	virtual UINT32			GetCount()=0;
	virtual UINT32			Append(IObject* pob)=0;

};

// -----------------------------------------------------------------------------------
// INamedObject
// -----------------------------------------------------------------------------------
interface INamedObject : public IObject
{
	virtual void		SetName(const ATOM_CHAR* pszName)=0;
	virtual ATOM_CHAR*	GetName()=0;
	virtual UINT32		EqualsName(const ATOM_CHAR* pszName, UINT32 nFlags=0)=0;

};

// -----------------------------------------------------------------------------------
// INode
// -----------------------------------------------------------------------------------
interface INode : public IObject  
{

	virtual INode*		GetRoot()=0;
	virtual INode*		GetParent(UINT32 nIndex=0)=0;
	virtual INodes*		GetParents()=0;
	virtual INodes*		GetChildren()=0;
	virtual INodes*		GetSiblings()=0;
	virtual void		SetSiblings(INodes* pSiblings)=0;

	virtual INode*		GetPrev()=0;
	virtual INode*		GetNext()=0;

	virtual IObject*	SetData(IObject* pData)=0;
	virtual IObject*	GetData(UINT32 idClass=0)=0;

	
	virtual void		SetID(UINT32 nID)=0;
	virtual UINT32		GetID()=0;

	virtual INode*		CreateChild(UINT32 idClass);	

}; 

// -----------------------------------------------------------------------------------
// INodes
// -----------------------------------------------------------------------------------
interface INodes : public IObject  
{
	virtual UINT32	GetCount()=0;
	virtual INode*	GetAt(UINT32 nIndex)=0;
	virtual UINT32	Append(INode* pnode)=0;

	virtual INode*	GetFirst()=0;
	virtual INode*	GetLast()=0;
};

// -----------------------------------------------------------------------------------
// IGraph
// -----------------------------------------------------------------------------------
interface IGraph : public INode
{

};

// -----------------------------------------------------------------------------------
// IClass
// -----------------------------------------------------------------------------------
interface IClass : public IObject
{
	virtual UINT32				GetClassID()=0;
	virtual BOOL				GetBaseClassID()=0;
	virtual CHAR8*				GetName()=0;
	virtual UINT32				EqualsName(const CHAR8* pszName, UINT32 nFlags=0)=0;

	virtual void*				CreateObject()=0;
	virtual BOOL				IsA(UINT32 idClass)=0;
	virtual IClass*				GetBaseClass()=0;

};

// -----------------------------------------------------------------------------------
// IClasses
// -----------------------------------------------------------------------------------
interface IClasses  : public IObject
{

	virtual IClass*		GetAt(UINT32 nIndex)=0;
	virtual UINT32		GetCount()=0;
	virtual IClass*		Find(UINT32 idClass)=0;
	virtual IClass*		Find(CHAR8* pszName)=0;
	virtual	BOOL		Append( IClass* pClass)=0;


}; 

// -----------------------------------------------------------------------------------
// ISystem
// -----------------------------------------------------------------------------------
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


// -----------------------------------------------------------------------------------
// ISystems
// -----------------------------------------------------------------------------------
interface ISystems : public IObject
{
	virtual ISystem*		GetAt(UINT32 nIndex)=0;

	virtual ISystem*		Get(UINT32 nClassID)=0;
	virtual UINT32			GetCount()=0;
	virtual UINT32			Append(ISystem* pob)=0;

	virtual IClass*			FindClass(UINT32 idClass)=0;

};


// -----------------------------------------------------------------------------------
// IPin
// -----------------------------------------------------------------------------------
interface IPin : public INamedObject
{
	virtual void			SetDirection(UINT32 nDirection)=0;	
	virtual UINT32			GetDirection()=0;	

	virtual void			SetSignal(ISignal* pSignal)=0;	
	virtual ISignal*		GetSignal()=0;	
	virtual void			SetDataClass(UINT32 idClass)=0;	
	virtual UINT32			GetDataClass()=0;	

	virtual IProcessor*		GetProcessor()=0;
	virtual void			SetProcessor(IProcessor* pOwner)=0;
	virtual IConnection*	GetConnection()=0;
	virtual void			SetConnection(IConnection* pOwner)=0;

	virtual void			SetThrough(UINT32 nPin)=0;	
	virtual UINT32			GetThrough()=0;	

}; 


// -----------------------------------------------------------------------------------
// IPins
// -----------------------------------------------------------------------------------
interface IPins : public IObject
{
	virtual IPin*		GetAt(UINT32 nIndex)=0;
	virtual UINT32		GetCount()=0;
	virtual UINT32		Append(IPin* pob)=0;
	virtual UINT32		Append(UINT32 nPins)=0;
	virtual UINT32		Set(UINT32 nIndex, IPin* pPin)=0;


}; 


// -----------------------------------------------------------------------------------
// IConnection
// -----------------------------------------------------------------------------------
interface IConnection : public IObject
{
	virtual void	SetPins(IPin* ppinFrom, IPin* ppinTo)=0;
	virtual IPin*	GetPin(UINT32 nIndex)=0;


}; 

// -----------------------------------------------------------------------------------
// IConnections
// -----------------------------------------------------------------------------------
interface IConnections : public IObject
{
	virtual IConnection*		GetAt(UINT32 nIndex)=0;
	virtual UINT32				GetCount()=0;
	virtual UINT32				Append(IConnection* pob)=0;
	virtual IConnection*		Find(IPin* ppinFrom,  IPin* ppinTo)=0;
	virtual BOOL				Remove(IConnection* pConnection)=0;


}; 


// -----------------------------------------------------------------------------------
// IProcessor
// -----------------------------------------------------------------------------------
interface IProcessor : public IObject
{
	virtual IPins*	GetPins()=0;
	virtual IPin*	Connect(UINT32 nPin, ISignal* pSignal)=0;
	virtual BOOL	ConnectionComplete(IPin* pPin, ISignal* pSignal)=0;

	virtual BOOL	ProcessOutputs()=0;
	virtual BOOL	ProcessOutput(IPin* ppinOut)=0;
	virtual ISignal* CreateSignal(UINT32 idClass, UINT32 nSize, IPin* pPin=NULL)=0;

}; // interface


// -----------------------------------------------------------------------------------
// IProcessors
// -----------------------------------------------------------------------------------
interface IProcessors : public IObject
{
	virtual IProcessor*		GetAt(UINT32 nIndex)=0;
	virtual IProcessor*		Find(IProcessor* pob)=0;
	virtual UINT32			GetCount()=0;
	virtual UINT32			Append(IProcessor* pob)=0;

}; 

// -----------------------------------------------------------------------------------
// ISignal
// -----------------------------------------------------------------------------------
interface ISignal : public IObject
{
	virtual BOOL		Initialize(UINT32 nElements,BOOL bSetFilled=TRUE)=0;

	virtual UINT32		GetByteSize()=0; 
	virtual UINT32		GetElemSize()=0;	// = stride

	virtual UINT32		SetElemCount(UINT32 nLevel)=0;	// set level of stored valid elements , 1 for scalars
	virtual UINT32		GetElemCount()=0;	// level of stored valid elements , 1 for scalars
	virtual UINT32		GetElemCapacity()=0; // fixed capacity, 1 for scalars

	virtual UINT8*		GetData()=0;
	virtual	BOOL		Set()=0;

	virtual IObject*	GetAt(UINT32 nIndex)=0;
	virtual IObject*	SetAt(UINT32 nIndex, IObject* pob)=0;
	virtual UINT32		Append(IObject* pob)=0;

	virtual ISignal*	Clone(BOOL bCopyData=TRUE)=0;
	virtual IObject*	GetBegin()=0;


}; 

// -----------------------------------------------------------------------------------
// ISignals
// -----------------------------------------------------------------------------------
interface ISignals : public IObject
{
	virtual ISignal*		GetAt(UINT32 nIndex)=0;
	virtual UINT32			GetCount()=0;
	virtual UINT32			Append(ISignal* pob)=0;


}; 


// -----------------------------------------------------------------------------------
// ISignalGraph
// -----------------------------------------------------------------------------------
interface ISignalGraph : public IGraph
{
	virtual IConnections*	GetConnections()=0;
	virtual IProcessors*	GetProcessors()=0;
	virtual IConnection*	ConnectPins(IPin* ppinFrom, IPin* ppinTo)=0;
	virtual IConnection*	ConnectPins(IProcessor* pFrom, UINT32 nPinFrom, IProcessor* pTo, UINT32 nPinTo)=0;
	virtual BOOL			DisconnectPins(IPin* ppinFrom, IPin* ppinTo)=0;
	virtual	void			Start()=0;
	virtual	void			Stop()=0;

}; // interface



// -----------------------------------------------------------------------------------
// IEngine
// -----------------------------------------------------------------------------------
interface IEngine : public IObject
{
	virtual UINT32		LoadPlugins(IStrings8* pPlugs)=0;
	virtual ISystem*	LoadPlugin(ATOM_CHAR* pszName)=0;

	virtual UINT32		AddSystem(ISystem* pSystem)=0;
	virtual ISystems*	GetSystems()=0;
	virtual ISystem*	FindSystem(UINT32 idClass)=0;

	virtual IObject*	CreateObject(UINT32 idClass)=0;
	virtual IObject*	CreateObject(ATOM_CHAR* pszClass)=0;

	virtual UINT32		Signal(UINT32 nSignalID, UINT32 idObject, void* pob)=0;
	virtual void		Output(ATOM_CHAR* pszText)=0;

	virtual IApplication*	CreateApplication(UINT32 idClass, void* pHandle, ATOM_CHAR* pszName)=0;
	virtual IApplication*	GetApplication()=0;
};




// =================================================================================
// data interfaces
// =================================================================================



// -----------------------------------------------------------------------------------
// IDataLocation
// -----------------------------------------------------------------------------------
interface IDataLocation : public INamedObject
{
	virtual BOOL	Initialize( CHAR8* pszType, CHAR8* pszLocation)=0;
	virtual CHAR8*	GetType()=0;
	virtual void	SetLocation(const ATOM_CHAR* pszPathName)=0;
	virtual CHAR8*	GetLocation()=0;
	virtual UINT32	EqualsType(const ATOM_CHAR* pszType, UINT32 nFlags=0)=0;



}; 



// -----------------------------------------------------------------------------------
// IDataLocations
// -----------------------------------------------------------------------------------
interface IDataLocations : public IObject
{
	virtual IDataLocation*	GetAt(UINT32 nIndex)=0;
	virtual UINT32			GetCount()=0;
	virtual UINT32			Append(IDataLocation* pob)=0;
	virtual UINT32			Append(CHAR8* pszType, CHAR8* pszLocation)=0;
	virtual IDataLocation*	FindByName(const CHAR8* pszData)=0;
	virtual IDataLocation*	FindByType(const CHAR8* pszData)=0;



}; 


// -----------------------------------------------------------------------------------
// IBuffer
// -----------------------------------------------------------------------------------
interface IBuffer : public ISignal
{
	
	virtual UINT32	Copy(UINT32 nIndex, void* pData, UINT32 nLength)=0;


}; 


// -----------------------------------------------------------------------------------
// ITuple4f
// -----------------------------------------------------------------------------------
interface ITuple4f : public IObject
{
	virtual void		Set(FLOAT32 f0, FLOAT32 f1, FLOAT32 f2, FLOAT32 f3 )=0;
	virtual void		Set(FLOAT32 f[4])=0;
	virtual void		Get(FLOAT32 f[4])=0;
	virtual void		Set(UINT32 nIndex, FLOAT32 f)=0;
	virtual FLOAT32		Get(UINT32 nIndex)=0;

	virtual void		Set(const ITuple4f* pSource)=0;

}; // interface

// -----------------------------------------------------------------------------------
// IScalar
// -----------------------------------------------------------------------------------
interface IScalar : public ISignal
{

}; 

// -----------------------------------------------------------------------------------
// ICHAR8Scalar
// -----------------------------------------------------------------------------------
interface ICHAR8Scalar : public IScalar
{

	virtual CHAR8	Get()=0;
	virtual CHAR8	Set(CHAR8 nValue)=0;


}; 

// -----------------------------------------------------------------------------------
// IFLOAT32Scalar
// -----------------------------------------------------------------------------------
interface IFLOAT32Scalar : public IScalar
{

	virtual FLOAT32	Get()=0;
	virtual FLOAT32	Set(FLOAT32 fValue)=0;


}; 

// -----------------------------------------------------------------------------------
// IUINT32Scalar
// -----------------------------------------------------------------------------------
interface IUINT32Scalar : public IScalar
{

	virtual UINT32	Get()=0;
	virtual UINT32	Set(UINT32 nValue)=0;


}; // interface


// -----------------------------------------------------------------------------------
// IFloats
// -----------------------------------------------------------------------------------
interface IFloats
{
	virtual UINT32	GetCount()=0;
	virtual UINT32	Append(FLOAT32 f)=0;
}; 

// -----------------------------------------------------------------------------------
// IIntegers
// -----------------------------------------------------------------------------------
interface IIntegers
{
	virtual UINT32	GetCount()=0;
	virtual UINT32	Append(INT32 n)=0;
	virtual INT32*	GetData()=0;

}; 


// -----------------------------------------------------------------------------------
// IString8
// -----------------------------------------------------------------------------------
interface IString8 : public IObject
{
	virtual BOOL		Create(UINT32 nSize)=0;

	virtual CHAR8*		Get()=0;
	virtual CHAR8*		Set(const CHAR8* pszData)=0;
	virtual UINT32		Equals(const CHAR8* pszData, UINT32 nFlags)=0;

	virtual UINT32		GetLength()=0;
	virtual UINT32		Replace(CHAR8* original, CHAR8* replacement)=0;


}; // interface


// -----------------------------------------------------------------------------------
// IStrings8
// -----------------------------------------------------------------------------------
interface IStrings8 : public IObject
{
	virtual IString8*	GetAt(UINT32 nIndex)=0;
	virtual UINT32		GetCount()=0;
	virtual UINT32		Append(IString8* pob)=0;
	virtual UINT32		Append(CHAR8* psz)=0;
	virtual IString8*	Find(const CHAR8* pszData)=0;


}; 

// =================================================================================
// app interfaces
// =================================================================================


// -----------------------------------------------------------------------------------
// IPlatformObject
// -----------------------------------------------------------------------------------
interface IPlatformObject : public IObject  
{
	virtual BOOL	SetHandle(void* pHandle)=0;
	virtual void*	GetHandle()=0;

}; 


// -----------------------------------------------------------------------------------
// IApplication
// -----------------------------------------------------------------------------------
interface IApplication : public IPlatformObject  
{

	virtual void				SetName(const ATOM_CHAR* pszName)=0;
	virtual ATOM_CHAR*			GetName()=0;
	virtual ATOM_CHAR*			GetVersion()=0;


	virtual void				GetPathName(ATOM_PATHNAME& szPathName)=0;
	virtual void				GetRoot(ATOM_PATHNAME& szPathName)=0;

	virtual UINT32				Start()=0;
	virtual IWindow*			GetMainWindow()=0;

};




// -----------------------------------------------------------------------------------
// IWindow
// -----------------------------------------------------------------------------------
interface IWindow  : public IPlatformObject
{

	
	virtual BOOL	Show()=0;
	virtual BOOL	Hide()=0;

	virtual BOOL	SetPos(INT32 nx, INT32 ny)=0;
	virtual void	GetPos(INT32& nx, INT32& ny)=0;

	virtual BOOL	SetSize(INT32 nWidth, INT32 nHeight)=0;
	virtual void	GetSize(INT32& nWidth, INT32& nHeight)=0;


	// @todo: make pins
	virtual void		SetApplication(IApplication* pApp)=0;
	virtual void		SetRenderer(IRenderer* pRend)=0;
	virtual IRenderer*	GetRenderer()=0;
	virtual void		SetDrawing(IDrawing* pDrawing)=0;
	virtual IDrawing*	GetDrawing()=0;
	virtual void		Redraw(ATOM_RECT* prc=NULL)=0;
	virtual BOOL		IsActive()=0; 

	virtual void		SetMouseCapture(BOOL b)=0; 


}; 



// -----------------------------------------------------------------------------------
// IConsole
// -----------------------------------------------------------------------------------
interface IConsole : public IPlatformObject
{
	virtual void	Write(char* pszText, BOOL bCRLF=TRUE)=0;



}; 




// -----------------------------------------------------------------------------------
// IFile
// -----------------------------------------------------------------------------------
interface IFile : public INamedObject
{
	typedef enum eOpenMode
	{
		ReadBin	= ATOM_RDONLY  | ATOM_BINARY,
		WriteBin	= ATOM_WRONLY  | ATOM_BINARY | ATOM_TRUNC,
		AppendBin = ATOM_WRONLY  | ATOM_BINARY | ATOM_APPEND | ATOM_CREAT,
		CreateBin = ATOM_WRONLY  | ATOM_BINARY | ATOM_TRUNC | ATOM_CREAT,
		

	} OPENMODE;

	typedef	enum eSeekMode
	{
		Begin	= ATOM_SEEK_SET,
		End		= ATOM_SEEK_END,
		Current	= ATOM_SEEK_CUR,

	} SEEKMODE;

	virtual BOOL	Open(const ATOM_CHAR* pszPathName, UINT32 nMode= ReadBin)=0;
	virtual void	Close()=0;

	virtual INT32	Read(void* pBuffer, INT32 nSizeToRead)=0;
	virtual INT32	Write(void* pBuffer, INT32 nSizeToWrite)=0;

	virtual INT32	Seek(INT32 nPos,UINT32 nMode )=0;
	virtual INT32	GetSize()=0;
	virtual INT32	GetFilePosition()=0;

	virtual BOOL	Compare( const INT8 *pData, INT32 nBytes )=0;

	virtual BOOL	ReadINT32( INT32* pnValue )=0;
	virtual BOOL	ReadINT16( INT16* pnValue )=0;
	virtual BOOL	ReadINT8( INT8* pnValue )=0;

	virtual BOOL	WriteINT8( INT8* pnValue )=0;


}; 

// -----------------------------------------------------------------------------------
// IFileLister
// -----------------------------------------------------------------------------------
interface IFileLister : public IObject
{

	virtual BOOL		Initialize(ATOM_CHAR* pszStartDir, IStrings8* pNames)=0;
	virtual UINT32		AppendFile(ATOM_CHAR* pszPathName)=0;
	virtual ATOM_CHAR*	GetStartDir()=0;
	virtual IStrings8*	GetFileNames(ATOM_CHAR* pszPathName)=0;


};


// =================================================================================
// graphic interfaces
// =================================================================================

// -----------------------------------------------------------------------------------
// IColor
// -----------------------------------------------------------------------------------
interface IColor : public ITuple4f
{
	virtual void		SetRed(ATOM_FLOAT f)=0;
	virtual ATOM_FLOAT	GetRed()=0;
	virtual void		SetGreen(ATOM_FLOAT f)=0;
	virtual ATOM_FLOAT	GetGreen()=0;
	virtual void		SetBlue(ATOM_FLOAT f)=0;
	virtual ATOM_FLOAT	GetBlue()=0;
	virtual void		SetAlpha(ATOM_FLOAT f)=0;
	virtual ATOM_FLOAT	GetAlpha()=0;

	virtual UINT32		GetRGB()=0;
	virtual void		SetRGB(FLOAT32 f0, FLOAT32 f1, FLOAT32 f2)=0;

}; 

// -----------------------------------------------------------------------------------
// IDrawing
// -----------------------------------------------------------------------------------
interface IDrawing : public IGraph
{
	virtual void		SetWindow(IWindow* Window)=0;
	virtual IWindow*	GetWindow()=0;

	virtual IObject*	GetObjectAtPos(INT32 x, INT32 y)=0;

	virtual void		OnMouseDown(UINT32 nButton, INT32 x, INT32 y)=0;
	virtual void		OnMouseUp(UINT32 nButton, INT32 x, INT32 y)=0;
	virtual void		OnMouseMove(INT32 x, INT32 y)=0;

	virtual IShapes*	GetShapes()=0;
	virtual IShape*		CreateShape(UINT32 idClass, UINT32 x=0, UINT32 y=0, UINT32 w=0, UINT32 h=0)=0;

	virtual void		Redraw(ATOM_RECT* prc=NULL)=0;
}; 

// -----------------------------------------------------------------------------------
// IImage
// -----------------------------------------------------------------------------------
interface IImage : public INamedObject
{
    virtual UINT32 Read(ATOM_CHAR* pszFileName)=0;
    virtual UINT32 Write(ATOM_CHAR* pszFileName)=0;

	virtual INT32	GetWidth()=0;
	virtual INT32	GetHeight()=0;
	virtual INT32	GetBitCount()=0;
	virtual INT32	GetPitch ()=0;
	virtual INT32	GetColorCount ()=0;
	virtual INT32	GetCompression ()=0;

	virtual BOOL	IsValid ()=0;

	virtual BOOL	Render(void* pContext)=0;

}; 

// -----------------------------------------------------------------------------------
// IRenderer
// -----------------------------------------------------------------------------------
interface IRenderer : public IObject  
{
	virtual void		SetWindow(IWindow* pWindow)=0;
	virtual IWindow*	GetWindow()=0;
	virtual void		Present(INode* pnode=NULL)=0;
	virtual void		ProcessNodes(INode* pRoot)=0;
		
	virtual void		Rectangle(IShape* pShape)=0;
	virtual void		Polygon(IShape* pShape)=0;


	virtual	void		SetMaterial(IObject* pMaterial)=0;

};

// -----------------------------------------------------------------------------------
// IShape
// -----------------------------------------------------------------------------------
interface IShape : public IObject
{
	virtual void	SetDrawing(IDrawing* pDrawing)=0;

	virtual void	GetBounds(ATOM_RECT& rcBounds)=0;
	virtual void	SetBounds(ATOM_RECT& rcBounds)=0;
	virtual void	SetBounds(UINT32 x, UINT32 y,UINT32 w, UINT32 h)=0;

	virtual void	GetPos(UINT32& x, UINT32& y)=0;
	virtual void	SetPos(UINT32 x, UINT32 y)=0;
	virtual void	SetXPos(UINT32 x)=0;
	virtual void	SetYPos(UINT32 y)=0;
	virtual void	MoveBy(UINT32 x, UINT32 y)=0;

	virtual void	SetSize(UINT32 w, UINT32 h)=0;
	virtual void	GetSize(UINT32& w, UINT32& h)=0;

	virtual UINT32	GetWidth()=0;
	virtual void	SetWidth(UINT32 w)=0;

	virtual UINT32	GetHeight()=0;
	virtual void	SetHeight(UINT32 h)=0;
	
	virtual UINT32	Touch(INT32 x, INT32 y)=0;
	virtual void	Redraw()=0;

	virtual void	OnMouseDown(UINT32 nButton, INT32 x, INT32 y)=0;
	virtual void	OnMouseUp(UINT32 nButton, INT32 x, INT32 y)=0;
	virtual void	OnMouseMove(INT32 x, INT32 y)=0;

	virtual INT32	GetStatus()=0;
	virtual void	SetStatus(INT32 nStatus)=0;

}; 


// -----------------------------------------------------------------------------------
// IShapes
// -----------------------------------------------------------------------------------
interface IShapes : public IObject
{
	virtual IShape*		GetAt(UINT32 nIndex)=0;
	virtual UINT32		GetCount()=0;
	virtual UINT32		Append(IShape* pob)=0;
	virtual IShape*		Find(INT32 x, INT32 y)=0;
	virtual BOOL		Remove(IShape* pShape)=0;
	virtual UINT32		RemoveAll()=0;


}; 


} // namespace 

typedef atomos::IEngine* (__stdcall* CREATE_ENGINE)();
typedef atomos::ISystem* (__stdcall* GET_SYSTEM)();


#endif // _atomos_interfaces_
