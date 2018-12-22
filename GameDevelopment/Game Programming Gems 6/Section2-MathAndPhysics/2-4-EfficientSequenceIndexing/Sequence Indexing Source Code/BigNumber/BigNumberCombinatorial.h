#ifndef __COMBINATIONCALCULATOR_H__EE53FD49_F1DF_4a1f_8C73_4C0F87B6108A_
#define __COMBINATIONCALCULATOR_H__EE53FD49_F1DF_4a1f_8C73_4C0F87B6108A_

#include "BigNumberFactorial.h"
#include <vector>
//
// NcR = n!/(r! * (n-r)!) values would be computed with intermediate cache
//
class CBigNumberCombinatorial
{
	typedef std::vector<BIGNUMBER> BIGNUMVEC;
	typedef std::vector<BIGNUMVEC> BIGNUMVECARRAY;

	BIGNUMVECARRAY	m_Comb;

	long	m_NValue;

	inline const BIGNUMBER& ComputeNCR(long n, long r) const
	{
		static BIGNUMBER Val;

		Val = CBNFACT::Factorial(n);
		Val /= (CBNFACT::Factorial(r) * CBNFACT::Factorial(n-r));

		return Val;
	}

public:

	CBigNumberCombinatorial()
	{
		m_NValue = -1;
	}

	CBigNumberCombinatorial(long ulNumber)
	{
		m_NValue = -1;
		SetNValue(ulNumber);
	}

	~CBigNumberCombinatorial(void)
	{
	}

	inline const BIGNUMBER& operator()(long n, long r)
	{
		SetNValue(n);

		return (m_Comb.at(n)).at(r);
	}

	inline void SetNValue(long ulNumber)
	{
		if(ulNumber > NValue())
		{
			m_Comb.reserve(ulNumber + 16);

			for(long i = (long)m_Comb.size(); i <= ulNumber; ++i)
			{
				m_Comb.push_back(BIGNUMVEC());

				BIGNUMVEC& Vec = m_Comb[i];

				Vec.reserve(i + 16);

				for(long j=0; j <= i; ++j)
				{
					Vec.push_back(ComputeNCR(i, j));
				}
			}

			m_NValue = ulNumber;
		}
	}

	inline const long NValue() const {	return m_NValue;	}
};

#endif