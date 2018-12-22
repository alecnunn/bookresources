// GASignalRandomizer - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
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

#ifndef _atomosGASignalRandomizer_
#define _atomosGASignalRandomizer_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGASignalRandomizer : public IGAObject
{
	virtual FLOAT32 InterpolateLinear( FLOAT32 a, FLOAT32 b, FLOAT32 x)=0;
	virtual FLOAT32 InterpolateCosine( FLOAT32 a, FLOAT32 b, FLOAT32 x)=0;
	virtual FLOAT32 InterpolateCubic( FLOAT32 v0, FLOAT32 v1, FLOAT32 v2, FLOAT32 v3, FLOAT32 x)=0;

	virtual FLOAT32 BipolarRandom(UINT32 nx)=0;
	virtual void	BipolarRandom(ISignal& samples)=0;
	virtual void	BipolarPeaks(ISignal& samples, UINT32 nMaxGap)=0;
	virtual void	BipolarImpulses(ISignal& samples, UINT32 nMaxGap)=0;

	virtual FLOAT32 SmoothedNoise(UINT32 nx)=0;
	virtual FLOAT32 InterpolatedNoise(FLOAT32 fx)=0;
	virtual FLOAT32 PerlinNoise1D(FLOAT32 x)=0;



}; // interface


class ATOMGAUD_DLLCLASS CGASignalRandomizer : public CGAObject  
{
public:

	// IObject
	enum { CID = CID_GASignalRandomizer };
	static const CClass classCGASignalRandomizer;
	virtual const IClass*		GetClass();

	virtual void Terminate();

	// IGASignalRandomizer
	virtual FLOAT32 InterpolateLinear( FLOAT32 a, FLOAT32 b, FLOAT32 x);
	virtual FLOAT32 InterpolateCosine( FLOAT32 a, FLOAT32 b, FLOAT32 x);
	virtual FLOAT32 InterpolateCubic( FLOAT32 v0, FLOAT32 v1, FLOAT32 v2, FLOAT32 v3, FLOAT32 x);

	virtual FLOAT32 BipolarRandom(UINT32 nx);
	virtual void	BipolarRandom(FLOAT32* pSamples, UINT32 nCount);
	virtual void	BipolarPeaks(FLOAT32* pSamples, UINT32 nCount, UINT32 nMaxGap);
	virtual void	BipolarImpulses(FLOAT32* pSamples, UINT32 nCount, UINT32 nMaxGap);

	virtual FLOAT32 SmoothedNoise(UINT32 nx);
	virtual FLOAT32 InterpolatedNoise(FLOAT32 fx);
	virtual FLOAT32 PerlinNoise1D(FLOAT32 x, UINT32 nOctaves=8);


	// CTOR / DTOR
	CGASignalRandomizer();
	~CGASignalRandomizer();

protected:




};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGASignalRandomizer_
// --------------------------------------------------------------------------------------------------------
