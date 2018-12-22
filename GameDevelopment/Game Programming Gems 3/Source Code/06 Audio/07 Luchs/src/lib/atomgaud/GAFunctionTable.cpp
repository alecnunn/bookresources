// GAFunctionTable Implementation
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

#include "stdafx.h"
#include "GAFunctionTable.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAFunctionTable() { return( new CGAFunctionTable); }
const CClass CGAFunctionTable::classCGAFunctionTable(CID_GAFunctionTable, CID_GAObject, "CGAFunctionTable", CreateCGAFunctionTable);
const IClass*	CGAFunctionTable::GetClass() { return((const IClass*)&classCGAFunctionTable); }


// CTOR
CGAFunctionTable::CGAFunctionTable()
{
	m_nSize = 4096;
}

// DTOR
CGAFunctionTable::~CGAFunctionTable()
{
	Terminate();
}

// explicit terminate baseclass
void CGAFunctionTable::Terminate()
{

	CGAObject::Terminate();
}

void CGAFunctionTable::Initialize(UINT32 nType)
{
	int i=0;
	switch(nType)
	{
	case IGAFunctionTable::Sin:
		for( i = 0; i < m_nSize; i++)
			m_fData[i] = sinf(ATOM_2PI * (FLOAT32)i/(FLOAT32)m_nSize);
		break;
	case IGAFunctionTable::Cos:
		for( i = 0; i < m_nSize; i++)
			m_fData[i] = cosf(ATOM_2PI * (FLOAT32)i/(FLOAT32)m_nSize);
		break;
	case IGAFunctionTable::NDivSin:
		for( i = 0; i < m_nSize; i++)
			m_fData[i] = 1.0f / sinf(ATOM_2PI * (FLOAT32)i/(FLOAT32)m_nSize);
		break;
	default:
		for( i = 0; i < m_nSize; i++)
			m_fData[i] = 0;

	}
}

// lookup using 32 bit integer phase 
FLOAT32 CGAFunctionTable::GetValue(UINT32 nPhase)
{
  int    pos1 = nPhase >> (32 - 12);
  int    pos2 = (pos1+1) & (m_nSize - 1);

  double frac = (1.0/4294967296.0) * (nPhase << 12);
  return m_fData[pos1] + frac*(m_fData[pos2] - m_fData[pos1]);

}

/*
double lut_sin(unsigned phase)
{
  int    pos1 = phase >> (32 - LUT_LOG);
  int    pos2 = (pos1+1) & (LUT_SIZE - 1);
  double frac = (1.0/4294967296.0) * (phase << LUT_LOG);
  return sintab[pos1] + frac*(sintab[pos2] - sintab[pos1]);
}

// 1/sin lookup table using 32 bit fixed point phase 
double lut_inv(unsigned phase) 
{
  int    pos1 = phase >> (32 - LUT_LOG);
  int    pos2 = (pos1+1) & (LUT_SIZE - 1);
  double frac = (1.0/4294967296.0) * (phase << LUT_LOG);
  return invtab[pos1] + frac*(invtab[pos2] - invtab[pos1]);
}
*/


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
