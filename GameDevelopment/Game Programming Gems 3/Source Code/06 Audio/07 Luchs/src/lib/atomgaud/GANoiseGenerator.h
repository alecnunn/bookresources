// GANoiseGenerator - Interface
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

#ifndef _atomosGANoiseGenerator_
#define _atomosGANoiseGenerator_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGANoiseGenerator : public IGAGenerator
{



}; // interface


class ATOMGAUD_DLLCLASS CGANoiseGenerator : public CGAGenerator  
{
public:

	// IObject
	enum { CID = CID_GANoiseGenerator };
	static const CClass classCGANoiseGenerator;
	virtual const IClass*		GetClass();

	virtual	BOOL	Initialize(IObject* pob=NULL);
	virtual void Terminate();
	virtual UINT32	Process(IObject* pob=NULL);


	// IProcessor
//	virtual FLOAT32*	GetPin(UINT32 nType, UINT32 nIndex);

	// IGANoiseGenerator
	UINT32 GenerateRandomNumber( void );

	virtual FLOAT32 ProcessSample();

	// CTOR / DTOR
	CGANoiseGenerator();
	~CGANoiseGenerator();



protected:

	UINT32	   m_nSeed;
	INT32      m_nRunningSum;   // Used to optimize summing of generators.
	INT32      m_nIndex;        // Incremented each sample.
	INT32      m_nIndexMask;    // Index wrapped by ANDing with this mask.
	FLOAT32    m_fScale;       // Used to scale within range of -1.0 to +1.0

//	FLOAT32	m_fSample;			// last sample value


protected:

	CGASignalRandomizer		m_Random;


};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGANoiseGenerator_
// --------------------------------------------------------------------------------------------------------
