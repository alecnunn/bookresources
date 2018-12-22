#ifndef __BIGNUMBER_H__818888F4_942C_45e8_BC96_19D8D47C1D62
#define __BIGNUMBER_H__818888F4_942C_45e8_BC96_19D8D47C1D62

#define USE_GMP	1 //Make it 0 if you do not need arbitrary precision arithmetic. In that case unsigned long would be used.

#if USE_GMP

	#include "Gmp-4.1.4/Gmp.h"	// GMP is an arbitrary precision arithmetic Library for BigNumber manipulations.
	#pragma comment(lib,"Gmp")

class MPZ
{
	mpz_t mpzNumber;
public:
	inline MPZ()
	{
		mpz_init(mpzNumber);
	}
	inline ~MPZ()
	{
		mpz_clear(mpzNumber);
	}
	inline MPZ(const mpz_t& other)
	{
		mpz_init_set(mpzNumber, other);
	}
	inline MPZ(const MPZ& other)
	{
		mpz_init_set(mpzNumber, other.mpzNumber);
	}
	inline MPZ(signed long lNumber)
	{
		mpz_init_set_si(mpzNumber, lNumber);
	}
	inline void operator*=(unsigned long ulNumber)
	{
		mpz_mul_ui(mpzNumber, mpzNumber, ulNumber);
	}
	inline void operator*=(const mpz_t& other)
	{
		mpz_mul(mpzNumber, mpzNumber, other);
	}
	inline void operator*=(const MPZ& other)
	{
		mpz_mul(mpzNumber, mpzNumber, other.mpzNumber);
	}
	inline void operator+=(unsigned long ulNumber)
	{
		mpz_add_ui(mpzNumber, mpzNumber, ulNumber);
	}
	inline void operator+=(const mpz_t& other)
	{
		mpz_add(mpzNumber, mpzNumber, other);
	}
	inline void operator+=(const MPZ& other)
	{
		mpz_add(mpzNumber, mpzNumber, other.mpzNumber);
	}
	inline void operator-=(const mpz_t& other)
	{
		mpz_sub(mpzNumber, mpzNumber, other);
	}
	inline void operator-=(unsigned long ulNumber)
	{
		mpz_sub_ui(mpzNumber, mpzNumber, ulNumber);
	}
	inline void operator-=(const MPZ& other)
	{
		mpz_sub(mpzNumber, mpzNumber, other.mpzNumber);		
	}
	inline void operator/=(unsigned long ulNumber)
	{
		mpz_tdiv_q_ui(mpzNumber, mpzNumber, ulNumber);
	}
	inline void operator/=(const mpz_t& other)
	{
		mpz_tdiv_q(mpzNumber, mpzNumber, other);
	}
	inline void operator/=(const MPZ& other)
	{
		mpz_tdiv_q(mpzNumber, mpzNumber, other.mpzNumber);
	}
	inline void operator++()
	{
		mpz_add_ui(mpzNumber, mpzNumber, 1);
	}
	inline void operator--()
	{
		mpz_sub_ui(mpzNumber, mpzNumber, 1);
	}
	inline void operator=(signed long lNumber)
	{
		mpz_set_si(mpzNumber, lNumber);
	}
	inline void operator=(const MPZ& other)
	{
		mpz_set(mpzNumber, other.mpzNumber);
	}
	inline void operator=(const mpz_t& other)
	{
		mpz_set(mpzNumber, other);
	}
	inline bool operator >(const MPZ& other) const
	{
		return mpz_cmp(mpzNumber, other.mpzNumber) > 0;
	}
	inline bool operator >=(const MPZ& other) const
	{
		return mpz_cmp(mpzNumber, other.mpzNumber) >=0;
	}
	inline bool operator <(const MPZ& other) const
	{
		return mpz_cmp(mpzNumber, other.mpzNumber) < 0;
	}
	inline bool operator <=(const MPZ& other) const
	{
		return mpz_cmp(mpzNumber, other.mpzNumber) <=0;
	}
	inline bool operator !=(const MPZ& other) const
	{
		return mpz_cmp(mpzNumber, other.mpzNumber) != 0;
	}
	inline bool operator==(const MPZ& other) const
	{
		return mpz_cmp(mpzNumber, other.mpzNumber) == 0;
	}
	inline bool operator == (const mpz_t& other) const
	{
		return mpz_cmp(mpzNumber, other) == 0;
	}
	inline char* GetNumberIntoString(char* pBuf) const
	{
        return mpz_get_str(pBuf, 10, mpzNumber); //Base10
	}
	inline signed long GetNumberIntoLong() const
	{
		return mpz_get_si(mpzNumber);
	}
	inline mpz_t& GetNumber()
	{
		return mpzNumber;
	}
	inline MPZ operator+(const MPZ& other) const
	{
		static MPZ Temp;
		mpz_add(Temp.mpzNumber, mpzNumber, other.mpzNumber);
		return Temp;
	}
	inline MPZ operator-(const MPZ& other) const
	{
		static MPZ Temp;
		mpz_sub(Temp.mpzNumber, mpzNumber, other.mpzNumber);
		return Temp;
	}
	inline MPZ operator*(const MPZ& other) const
	{
		static MPZ Temp;
		mpz_mul(Temp.mpzNumber, mpzNumber, other.mpzNumber);
		return Temp;
	}
	inline MPZ operator/(const MPZ& other) const
	{
		static MPZ Temp;
		mpz_tdiv_q(Temp.mpzNumber, mpzNumber, other.mpzNumber);
		return Temp;
	}
	inline MPZ operator%(unsigned long ulNumber) const
	{
		static MPZ Temp;
		mpz_mod_ui(Temp.mpzNumber, mpzNumber, ulNumber);
		return Temp;
	}
	inline MPZ operator%(const MPZ& other) const
	{
		static MPZ Temp;
		mpz_mod(Temp.mpzNumber, mpzNumber, other.mpzNumber);
		return Temp;
	}
	inline friend MPZ operator+(unsigned long ulNumber, const MPZ& mpz)
	{
		static MPZ Temp;
		mpz_add_ui(Temp.mpzNumber, mpz.mpzNumber, ulNumber);
		return Temp;
	}
	inline friend MPZ operator-(unsigned long ulNumber, const MPZ& mpz)
	{
		static MPZ Temp;
		mpz_sub_ui(Temp.mpzNumber, mpz.mpzNumber, ulNumber);
		return Temp;
	}
	inline friend MPZ operator*(unsigned long ulNumber, const MPZ& mpz)
	{
		static MPZ Temp;
		mpz_mul_ui(Temp.mpzNumber, mpz.mpzNumber, ulNumber);
		return Temp;
	}
	inline friend MPZ operator/(unsigned long ulNumber, const MPZ& mpz)
	{
		static MPZ Temp;
		mpz_tdiv_q_ui(Temp.mpzNumber, mpz.mpzNumber, ulNumber);
		return Temp;
	}
};

	typedef MPZ BIGNUMBER;

#else	//if USE_GMP is 0

	typedef unsigned long BIGNUMBER;

#endif

#endif
