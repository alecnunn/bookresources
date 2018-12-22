// GAShapeOscillator - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	The ShapeOscillator uses a function to build a waveform/shape. The function is controllable
//				by up to 4 parameters.
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0401
// Author:		Frank Luchs
// History:
//				1996-05-07	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#ifndef _atomosGAShapeOscillator_
#define _atomosGAShapeOscillator_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAShapeOscillator : public IGAOscillator
{
//	virtual BOOL	SetParameter(UINT32 nIndex, FLOAT32 fValue)=0;


}; // interface


class ATOMGAUD_DLLCLASS CGAShapeOscillator : public CGAOscillator  
{
public:

	// IObject
	enum { CID = CID_GAShapeOscillator };
	static const CClass classCGAShapeOscillator;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32 Process(IObject* pob);

	// IGAShapeOscillator
//	virtual BOOL	SetParameter(UINT32 nIndex, FLOAT32 fValue);


	// CTOR / DTOR
	CGAShapeOscillator();
	~CGAShapeOscillator();

protected:




};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAShapeOscillator_
// --------------------------------------------------------------------------------------------------------
