// GAGenerator - Interface
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

#ifndef _atomosGAGenerator_
#define _atomosGAGenerator_


// baseclass for all generators
// with built-in sample buffer


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAGenerator : public IGAProcessor
{

//	virtual void		SetFrequency(FLOAT32 fFreq)=0;
//	virtual void		SetFMGain(FLOAT32 fGain)=0;
	virtual void SetAmplitude(FLOAT32 fLevel)=0;
	virtual void SetAMAttenuation(FLOAT32 fAttenuation)=0;

}; // interface


class ATOMGAUD_DLLCLASS CGAGenerator : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GAGenerator };
	static const CClass classCGAGenerator;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	

	// IGAObject
	virtual BOOL	SetParameter(CHAR8* pszName, CHAR8* pszValue);

	// IGAGenerator

	virtual void SetAmplitude(FLOAT32 fLevel);
	virtual void SetAMAttenuation(FLOAT32 fAttenuation);


	// CTOR / DTOR
	CGAGenerator();
	~CGAGenerator();

protected:

	FLOAT32	m_fAmplitude;	// base amplitude, value 0 to 1 
	FLOAT32	m_fAMAttenuation;		// attenuator , value 0 to 1


};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAGenerator_
// --------------------------------------------------------------------------------------------------------
