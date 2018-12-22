#ifndef __BIGNUMBERFACTORIAL_H__EA8AA1A1_12A2_40dd_A250_1CAE3171D1F6_
#define __BIGNUMBERFACTORIAL_H__EA8AA1A1_12A2_40dd_A250_1CAE3171D1F6_

#include "BigNumber.h"
#include <vector>
//
// Factorial would be computed with intermediate cache
//
class CBigNumberFactorial
{
	static std::vector<BIGNUMBER> m_FactArray;

	friend void LoadFactorials();
	friend void UnloadFactorials();

	class Loader
	{		
	public:	
		Loader()	{ 	LoadFactorials();		}
		~Loader()	{	UnloadFactorials();		}
	};
	static Loader m_Loader;

	CBigNumberFactorial()	{	}

public:
	static const BIGNUMBER& Factorial(long lNumber)
	{
		if(lNumber <= 1) return m_FactArray[0];

        if(lNumber >= (long)m_FactArray.size())
		{
			static BIGNUMBER Product;
			Product = m_FactArray[m_FactArray.size() - 1];
			for(long l = (long)m_FactArray.size(); l <= lNumber; ++l)
			{
				Product *= l;
				m_FactArray.push_back(Product);
			}
		}

		return m_FactArray[lNumber];
	}	
};

typedef CBigNumberFactorial CBNFACT;

#endif