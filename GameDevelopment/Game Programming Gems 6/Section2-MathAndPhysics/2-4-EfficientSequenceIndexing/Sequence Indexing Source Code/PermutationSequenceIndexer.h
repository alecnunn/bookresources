#ifndef __CPERMUTATIONSEQUENCEINDEXER_H___1D3555D9_D1CA_4253_B0AA_D820F103255C
#define __CPERMUTATIONSEQUENCEINDEXER_H___1D3555D9_D1CA_4253_B0AA_D820F103255C
#include "SequenceIndexer.h"

namespace SequenceIndexer
{
typedef std::vector<TEXTLENGTH> POSVECTOR;
//
// Treats SYMBOLINDEX and TEXTLENGTH as same type.
// This is because No.ofSymbols == TextLength
// 
// TextLength could be any value in the range [0,MAXINT]
//
class CPermutationSequenceIndexer
{
	TEXTLENGTH	m_nSymbolCount;
	POSITION	m_nCompleteSeqLength;	//Sum of All Seq Lengths

	SYMBVECTOR	m_SymbVector;	//Useful for GetAt()
	POSITION	m_Index;		//Useful for GetIndexOf()
	POSVECTOR	m_Pos;			//Useful for holding the position of each symbol

	inline SYMBOLINDEX ToSymbolIndex(const MPZ& mpz)
	{
		return mpz.GetNumberIntoLong();
	}
	inline SYMBOLINDEX ToSymbolIndex(SYMBOLINDEX  nIndex)
	{
		return nIndex;
	}
public:

	CPermutationSequenceIndexer(TEXTLENGTH lLength)
	{
		m_Pos.reserve(lLength + 16);
		m_SymbVector.reserve(lLength + 16);

		for(TEXTLENGTH l=0; l < lLength; ++l)
		{
			m_Pos.push_back(l);
			m_SymbVector.push_back(l);
		}
		m_nSymbolCount = lLength;
		m_nCompleteSeqLength = CBigNumberFactorial::Factorial(lLength);
	}

	~CPermutationSequenceIndexer(void)
	{
	}

	SYMBVECTOR GetAt(const POSITION& SeqPosIndex )
	{
		if(SeqPosIndex <0 || SeqPosIndex >= m_nCompleteSeqLength)
			return SYMBVECTOR();

		SYMBOLINDEX lSwapVal;
		TEXTLENGTH	lLength = (TEXTLENGTH) m_nSymbolCount;

        for(long j = lLength - 1; j >=0; --j)
		{
			lSwapVal = ToSymbolIndex((SeqPosIndex % CBigNumberFactorial::Factorial((lLength - j))) / CBigNumberFactorial::Factorial(lLength - (j+1)));
			lSwapVal += j;

			m_Pos[j] = j;

			m_SymbVector[j]	=	lSwapVal;
			m_SymbVector[m_Pos[lSwapVal]] = j;

			m_Pos[j]		=	m_Pos[lSwapVal];
			m_Pos[lSwapVal] =	j;
		}
		return m_SymbVector;
	}

    POSITION GetIndexOf(SYMBVECTOR SymbolVector)
	{
		if(SymbolVector.size() <= 0 || SymbolVector.size() != m_nSymbolCount)
			return POSITION(-1);

		TEXTLENGTH	lLength = (TEXTLENGTH) m_nSymbolCount;

		for(long j = 0; j < lLength; ++j)
			m_Pos[SymbolVector[j]] = j;

		m_Index = 0;

		for(TEXTLENGTH j = 0; j < lLength; ++j)
		{
			m_Index += ((SymbolVector[j]-j) * CBigNumberFactorial::Factorial(lLength - (j+1)));

			SymbolVector[m_Pos[j]] = SymbolVector[j];
			m_Pos[SymbolVector[j]] = m_Pos[j];
			m_Pos[j] = j;
			SymbolVector[j] = j;
		}

		return m_Index;
	}

	inline SYMBVECTOR operator[](const POSITION& SeqPosIndex)
	{
		return GetAt(SeqPosIndex);
	}
	inline POSITION operator[](const SYMBVECTOR& SymbolVector)
	{
		return GetIndexOf(SymbolVector);
	}
	
	inline const SYMBOLINDEX SymbolCount() const	{	return m_nSymbolCount;	}	
	inline const POSITION&	 TotalSequenceLength() const	{	return m_nCompleteSeqLength;	}
};

}

#endif