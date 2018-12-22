// GATrapezEnvelope - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	Symnetric trapezoid envelope
//				...
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

#ifndef _atomosGATrapezEnvelope_
#define _atomosGATrapezEnvelope_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGATrapezEnvelope : public IGAEnvelope
{

	virtual void SetSlope(FLOAT32 fSlope)=0;


}; // interface


class ATOMGAUD_DLLCLASS CGATrapezEnvelope : public CGAEnvelope  
{
public:

	// IObject
	enum { CID = CID_GATrapezEnvelope };
	static const CClass classCGATrapezEnvelope;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32	Process(IObject* pob=NULL);

	// IGAProcessor

	// IGATrapezEnvelope
	virtual void SetSlope(FLOAT32 fSlope);


	// CTOR / DTOR
	CGATrapezEnvelope();
	~CGATrapezEnvelope();

protected:

	FLOAT32	m_fSlope; // from 0 to 0.5f (triangle) 

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGATrapezEnvelope_
// --------------------------------------------------------------------------------------------------------
