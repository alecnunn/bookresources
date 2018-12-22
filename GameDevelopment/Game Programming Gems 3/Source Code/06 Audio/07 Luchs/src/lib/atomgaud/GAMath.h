// GAMath - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	Simple math for FLOAT32 scalar and vector
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

#ifndef _atomosGAMath_
#define _atomosGAMath_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAMath : public IGAObject
{



}; // interface


class ATOMGAUD_DLLCLASS CGAMath : public CGAObject  
{
public:

	// IObject
	enum { CID = CID_GAMath };
	static const CClass classCGAMath;
	virtual const IClass*		GetClass();

	virtual void Terminate();

	// IGAMath
	INT32 Round (FLOAT32 fValue);

	void	Zero (FLOAT32* pfDest, UINT32 nCount);

	void	Set (FLOAT32* pfDest, FLOAT32 fSource, UINT32 nCount);

	void	Add (FLOAT32* pfDest, FLOAT32 fSource, UINT32 nCount);
	void	Add (FLOAT32* pfDest, const FLOAT32* pfSource, UINT32 nCount);
	void	Add (FLOAT32* pfDest, const FLOAT32* pfSource1, const FLOAT32* pfSource2, UINT32 nCount);

	void	Sub (FLOAT32* pfDest, FLOAT32 fSource, UINT32 nCount);
	void	Sub (FLOAT32* pfDest, const FLOAT32* pfSource, UINT32 nCount);
	void	Sub (FLOAT32* pfDest, const FLOAT32* pfSource1, const FLOAT32* pfSource2, UINT32 nCount);

	void	Mul (FLOAT32* pfDest, FLOAT32 fSource, UINT32 nCount);
	void	Mul (FLOAT32* pfDest, const FLOAT32* pfSource, UINT32 nCount);
	void	Mul (FLOAT32* pfDest, const FLOAT32* pfSource1, const FLOAT32* pfSource2, UINT32 nCount);

	void	Div (FLOAT32* pfDest, FLOAT32 fSource, UINT32 nCount);
	void	Div (FLOAT32* pfDest, const FLOAT32* pfSource, UINT32 nCount);
	void	Div (FLOAT32* pfDest, const FLOAT32* pfSource1, const FLOAT32* pfSource2, UINT32 nCount);

	void	Abs (FLOAT32* pfDest, UINT32 nCount);
	void	Abs (FLOAT32* pfDest, const FLOAT32* pfSource, UINT32 nCount);

	void	Sqrt (FLOAT32* pfDest, UINT32 nCount);
	void	Sqrt (FLOAT32* pfDest, const FLOAT32* pfSource, UINT32 nCount);

	void	Max (FLOAT32* pfDest, FLOAT32 fMax, UINT32 nCount);
	void	Max (FLOAT32* pfDest, const FLOAT32* pfSource, FLOAT32 fMax, UINT32 nCount);

	void	Min (FLOAT32* pfDest, FLOAT32 fMax, UINT32 nCount);
	void	Min (FLOAT32* pfDest, const FLOAT32* pfSource, FLOAT32 fMax, UINT32 nCount);

	void	Clip (FLOAT32* pfDest, FLOAT32 fMin, FLOAT32 fMax, UINT32 nCount);
	void	Clip (FLOAT32* pfDest, const FLOAT32* pfSource, FLOAT32 fMin, FLOAT32 fMax, UINT32 nCount);

	FLOAT32 Convolve (const FLOAT32* pfx, const FLOAT32* pfy, UINT32 nCount);
	void	Convolve (FLOAT32* pfDest, const FLOAT32* pfx, const FLOAT32* pfy,UINT32 nCount);
	
	FLOAT32 Freq2Pitch(FLOAT32 fFreq);

	
	
	// CTOR / DTOR
	CGAMath();
	~CGAMath();

protected:




};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAMath_
// --------------------------------------------------------------------------------------------------------
 