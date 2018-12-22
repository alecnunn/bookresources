#include "stdafx.h"
#include "BigNumberFactorial.h"

std::vector<BIGNUMBER> CBigNumberFactorial::m_FactArray;
CBigNumberFactorial::Loader CBigNumberFactorial::m_Loader;

void LoadFactorials()
{			
	CBigNumberFactorial::m_FactArray.reserve(16384);
	BIGNUMBER One = 1;
	CBigNumberFactorial::m_FactArray.push_back(One);
}

void UnloadFactorials()
{	
}