// GAFile - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0202
// Author:		Frank Luchs
// History:
//				2001-10-03	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#ifndef _atomosGAFile_
#define _atomosGAFile_
#include "io.H"
#include "FCNTL.H"
#include "tchar.H"
#include <sys/types.h>
#include <sys/stat.h>

#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAFile : public IGAObject
{
	typedef enum eOpenMode
	{
		ReadBin	= _O_RDONLY  | _O_BINARY,
		WriteBin	= _O_WRONLY  | _O_BINARY | _O_TRUNC,
		AppendBin = _O_WRONLY  | _O_BINARY | _O_APPEND | _O_CREAT,
		CreateBin = _O_WRONLY  | _O_BINARY | _O_TRUNC | _O_CREAT,
		

	} OPENMODE;

	typedef	enum eSeekMode
	{
		Begin	= SEEK_SET,
		End		= SEEK_END,
		Current	= SEEK_CUR,

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


}; // interface


class ATOMGAUD_DLLCLASS CGAFile : public CGAObject  
{
public:

	// IObject
	enum { CID = CID_GAFile };
	static const CClass classCGAFile;
	virtual const IClass*		GetClass();

	virtual void Terminate();
/*
	// IGAFile
	typedef enum eOpenMode
	{
		ReadBin	= _O_RDONLY  | _O_BINARY,
		WriteBin	= _O_WRONLY  | _O_BINARY | _O_TRUNC,
		AppendBin = _O_WRONLY  | _O_BINARY | _O_APPEND | _O_CREAT,
		CreateBin = _O_WRONLY  | _O_BINARY | _O_TRUNC | _O_CREAT,

	} OPENMODE;

	typedef	enum eSeekMode
	{
		Begin	= SEEK_SET,
		End		= SEEK_END,
		Current	= SEEK_CUR,

	} SEEKMODE;
*/
	// IFile
	virtual BOOL	Open(const ATOM_CHAR* pszPathName, UINT32 nMode= IGAFile::ReadBin);
	virtual void	Close();

	virtual INT32	Read(void* pBuffer, INT32 nSizeToRead);
	virtual INT32	Write(void* pBuffer, INT32 nSizeToWrite);

	virtual INT32	Seek(INT32 nPos, UINT32 nMode=IGAFile::Begin );
	virtual INT32	GetSize();
	virtual UINT32	GetPosition();

	virtual INT32	Compare( const INT8 *pData, INT32 nBytes );

	virtual BOOL	ReadINT32( INT32* pnValue );
	virtual BOOL	ReadINT16( INT16* pnValue );
	virtual BOOL	ReadINT8( INT8* pnValue );

	virtual BOOL	WriteINT8( INT8 nValue );

	
	// CTOR / DTOR
	CGAFile();
	~CGAFile();

protected:

	int	m_Handle;



};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAFile_
// --------------------------------------------------------------------------------------------------------
