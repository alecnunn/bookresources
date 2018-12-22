#ifndef __RANGEINDEXSEQUENCER_H__10F1B931_E2F4_4b3a_9462_1953254AC3A8
#define __RANGEINDEXSEQUENCER_H__10F1B931_E2F4_4b3a_9462_1953254AC3A8

#include "SequenceIndexer.h"

namespace SequenceIndexer
{

typedef unsigned long RANGE;
//
// Unlike Frequency, Range Values cannot be Zero ( should be >0)
//
// This is because TextLength == No. Of Symbols
//
class CRangeSequenceIndexer
{
	SYMBOLINDEX	m_nSymbolCount;	//Number of Symbols
	POSITION*	m_SeqLengths;	//Holds the SeqLengths for each symbol
	POSITION	m_nCompleteSeqLength;	//Total Seq Length
		
	SYMBVECTOR	m_SymbVector;	//Useful for GetAt()
	POSITION	m_Index;		//Useful for GetIndexOf()

	void ComputeSequenceLengths(const RANGE* pRange, const SYMBOLINDEX lSymbolCount)
	{
		Clear();
        
		if(NULL != (m_SeqLengths = new POSITION[lSymbolCount+1]))
		{
			m_nSymbolCount = lSymbolCount;
		
			m_SymbVector.reserve(m_nSymbolCount + 16);

			static POSITION Product = 1;

			m_SeqLengths[m_nSymbolCount] = 1;	//We need this as Sentinel

			for(long s = m_nSymbolCount - 1; s >= 0; --s)
			{
				m_SeqLengths[s] = Product * pRange[s];
				Product = m_SeqLengths[s];

				m_SymbVector.push_back(s);
			}

			m_nCompleteSeqLength = m_SeqLengths[0];
		}
	}

	inline void Clear()
	{
		if(m_SeqLengths)
		{
			delete[] m_SeqLengths;
			m_SeqLengths = NULL;
		}
		m_SymbVector.clear();
		m_nSymbolCount = 0;
	}

	inline SYMBOLINDEX Modulate(MPZ& Numerator, MPZ& Denominator) const
	{
		static MPZ Quot;
		mpz_tdiv_qr(Quot.GetNumber(), Numerator.GetNumber(), Numerator.GetNumber(), Denominator.GetNumber());
		return Quot.GetNumberIntoLong();
	}

	inline SYMBOLINDEX Modulate(unsigned long& Numerator, const unsigned long& Denominator) const
	{
        unsigned long Quot = Numerator / Denominator;
		Numerator %= Denominator;
		return Quot;
	}

public:
	inline CRangeSequenceIndexer()
	{
		m_SeqLengths = NULL;
	}

	// Note: pRange[i] should not be Zero for any i
	inline CRangeSequenceIndexer(const RANGE* pRange, const SYMBOLINDEX lSymbolCount)
	{
		m_SeqLengths = NULL;
		SetRanges(pRange, lSymbolCount);
	}

	inline ~CRangeSequenceIndexer(void)
	{
		Clear();
	}

	// Note: pRange[i] should not be Zero for any i
	inline void SetRanges(const RANGE* pRange, const SYMBOLINDEX lSymbolCount)
	{
		ComputeSequenceLengths(pRange, lSymbolCount);
	}

	SYMBVECTOR GetAt(POSITION SeqPosIndex)
	{
		if(SeqPosIndex <0 || SeqPosIndex >= m_nCompleteSeqLength)
			return SYMBVECTOR();

        for(SYMBOLINDEX s=0; s < m_nSymbolCount; ++s)
			m_SymbVector[s] = Modulate(SeqPosIndex, m_SeqLengths[s+1]);

		return m_SymbVector;
	}

	POSITION GetIndexOf(const SYMBVECTOR& SymbolVector)
	{
		if(SymbolVector.size() <= 0 || SymbolVector.size() != m_nSymbolCount)
			return POSITION(-1);

		m_Index = 0;

		for(SYMBOLINDEX s = 0; s < m_nSymbolCount; ++s)		
			m_Index += SymbolVector[s] * m_SeqLengths[s+1];
		
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