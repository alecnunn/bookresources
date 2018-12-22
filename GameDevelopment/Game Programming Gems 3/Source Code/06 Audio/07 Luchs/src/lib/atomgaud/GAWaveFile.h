// GAWaveFile - Interface
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

#ifndef _atomosGAWaveFile_
#define _atomosGAWaveFile_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAWaveFile : public IGAFile
{
	virtual ATOM_WAVEFILEHEADER* ReadHeader()=0;


}; // interface


class ATOMGAUD_DLLCLASS CGAWaveFile : public CGAFile  
{
public:

	// IObject
	enum { CID = CID_GAWaveFile };
	static const CClass classCGAWaveFile;
	virtual const IClass*		GetClass();

	virtual void Terminate();

	// IGAWaveFile
	virtual ATOM_WAVEFILEHEADER* ReadHeader();



	// CTOR / DTOR
	CGAWaveFile();
	~CGAWaveFile();

protected:

	ATOM_WAVEFILEHEADER	m_FileHeader;



};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAWaveFile_
// --------------------------------------------------------------------------------------------------------
