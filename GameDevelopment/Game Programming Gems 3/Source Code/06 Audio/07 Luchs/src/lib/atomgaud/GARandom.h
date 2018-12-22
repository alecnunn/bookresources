// GARandom - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0401
// Author:		Frank Luchs
// History:
//				1994-11-12	first draft
//				1995-03-06	replace of rand()
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#ifndef _atomosGARandom_
#define _atomosGARandom_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGARandom : public IGAObject
{
	virtual FLOAT32 UniformU()=0;
	virtual FLOAT32 UniformB()=0;

	virtual FLOAT32 LinearU()=0;
	virtual FLOAT32 LinearB()=0;

	virtual FLOAT32 TriangleB()=0;
	virtual FLOAT32 ExponentialU()=0;	
	virtual FLOAT32 ExponentialB()=0;	

	virtual FLOAT32 GaussianB(FLOAT32 fIn)=0;	

	virtual FLOAT32 CauchyU(FLOAT32 fIn)=0;	
	virtual FLOAT32 CauchyB(FLOAT32 fIn)=0;	

	virtual FLOAT32 Poisson(FLOAT32 fIn)=0;	



}; // interface


class ATOMGAUD_DLLCLASS CGARandom : public CGAObject  
{
public:

	// IObject
	enum { CID = CID_GARandom };
	static const CClass classCGARandom;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual void Reset();

	// IGARandom
	virtual FLOAT32 UniformU();
	virtual FLOAT32 UniformB();

	virtual FLOAT32 LinearU();
	virtual FLOAT32 LinearB();

	virtual FLOAT32 TriangleB();
	virtual FLOAT32 ExponentialU();	
	virtual FLOAT32 ExponentialB();	

	virtual FLOAT32 GaussianB(FLOAT32 fIn);	

	virtual FLOAT32 CauchyU(FLOAT32 fIn);	
	virtual FLOAT32 CauchyB(FLOAT32 fIn);	

	virtual FLOAT32 Poisson(FLOAT32 fIn);	




	// CTOR / DTOR
	CGARandom();
	~CGARandom();

protected:

	// noise init variables
	UINT32 m_vssRandSeed;
	UINT32 m_nflone;
	UINT32 m_nflmask;
	UINT32 m_ntemp;


};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGARandom_
// --------------------------------------------------------------------------------------------------------
