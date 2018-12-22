#ifndef __COMBINATIONALSEQUENCEINDEXER_H__28253DCC_FE53_4b3b_8A90_2799F214B554
#define __COMBINATIONALSEQUENCEINDEXER_H__28253DCC_FE53_4b3b_8A90_2799F214B554

#include "SequenceIndexer.h"
#include "BigNumber/BigNumberCombinatorial.h"

namespace SequenceIndexer
{

class CCombinationalSequenceIndexer
{
	SYMBOLINDEX	m_nSymbolCount;
	SYMBOLINDEX	m_nChoice;
	POSITION	m_nCompleteSeqLength;//Total Seq Length

	SYMBVECTOR	m_SymbVector;
	POSITION	m_Index;

	CBigNumberCombinatorial	m_Comb;	// Stores the Results of n c r = n!/(r!*(n-r)!) values

public:
	inline CCombinationalSequenceIndexer(){}

	inline CCombinationalSequenceIndexer(SYMBOLINDEX NoofSymbolsExisting, SYMBOLINDEX NoofSymbolsToChoose)
	{
		SetChoiceValues(NoofSymbolsExisting, NoofSymbolsToChoose);
	}

	inline ~CCombinationalSequenceIndexer(void)
	{
	}

	inline void SetChoiceValues(SYMBOLINDEX NoofSymbolsExisting, SYMBOLINDEX NoofSymbolsToChoose)
	{
		m_nSymbolCount = NoofSymbolsExisting;

		m_nChoice = NoofSymbolsToChoose;

		m_Comb.SetNValue(NoofSymbolsExisting);

		m_SymbVector.clear();

		m_SymbVector.reserve(m_nChoice + 16);

		for(SYMBOLINDEX s=0; s < m_nChoice; ++s)
			m_SymbVector.push_back(s);

		m_nCompleteSeqLength = m_Comb(m_nSymbolCount, m_nChoice);
	}

	SYMBVECTOR GetAt(POSITION SeqPosIndex)
	{
		if(SeqPosIndex <0 || SeqPosIndex >= m_nCompleteSeqLength)
			return SYMBVECTOR();

		static POSITION CumulativeSeqLength;

		SYMBOLINDEX r = m_nChoice;
		SYMBOLINDEX n = m_nSymbolCount;

		for(SYMBOLINDEX s=0; s < m_nChoice; ++s)
		{
            CumulativeSeqLength = 0;

			for(SYMBOLINDEX i=0, nMax = n - r; i <= nMax; ++i)
			{
				const POSITION& SeqLength = m_Comb(n-(i+1), r-1);

                CumulativeSeqLength += SeqLength;

				if(SeqPosIndex < CumulativeSeqLength)
				{
					SeqPosIndex += SeqLength;
					SeqPosIndex -= CumulativeSeqLength;	//Index-=(CSL-SeqLength);

					m_SymbVector[s] = (m_nSymbolCount - n) + i;

					r -= 1;
					n -= (i+1);	

					break;
				}
			}
		}
		return m_SymbVector;
	}

	POSITION GetIndexOf(const SYMBVECTOR& SymbolVector)
	{
		if(SymbolVector.size() <= 0 || SymbolVector.size() != m_nChoice)
			return POSITION(-1);

		SYMBOLINDEX r = m_nChoice;
		SYMBOLINDEX n = m_nSymbolCount;

        m_Index = 0;

		for(SYMBOLINDEX s=0; s < m_nChoice; ++s)
		{            
			const SYMBOLINDEX& SelectedIndex = SymbolVector[s] - (m_nSymbolCount - n);

			for(SYMBOLINDEX i=0 ; i < SelectedIndex; ++i)
                m_Index += m_Comb(n-(i+1), r-1);
			
			n -= (SelectedIndex + 1);
			r -= 1;
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
	
	inline const SYMBOLINDEX Choice() const	{	return m_nChoice;	}
	inline const SYMBOLINDEX SymbolCount() const	{	return m_nSymbolCount;	}	
	inline const POSITION&	 TotalSequenceLength() const	{	return m_nCompleteSeqLength;	}
};

}
#endif