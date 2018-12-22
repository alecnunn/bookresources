/*
s_p_oneil@hotmail.com
Copyright (c) 2000, Sean O'Neil
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of this project nor the names of its contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include "Impostor.h"

#define NUM_BYTES(nBits)	(nBits>>3)
#define NUM_WORDS(nBits)	(nBits>>4)
#define NUM_DWORDS(nBits)	(nBits>>5)
#define NUM_QWORDS(nBits)	(nBits>>6)

#define BIT_BYTES(nBit)		(7 - (nBit & 7))
#define BIT_WORDS(nBit)		(15 - (nBit & 15))
#define BIT_DWORDS(nBit)	(31 - (nBit & 31))

#define MASK_BYTES(nBit)	(1 << BIT_BYTES(nBit))
#define MASK_WORDS(nBit)	(1 << BIT_WORDS(nBit))
#define MASK_DWORDS(nBit)	(1 << BIT_DWORDS(nBit))

class CBitBucket
{
protected:
	unsigned long m_nBits;
	unsigned long *m_pBucket;

public:
	// Constructors and copy methods
	CBitBucket()					{ m_pBucket = NULL; }
	CBitBucket(unsigned long nBits)	{ m_pBucket = NULL; Init(nBits); }
	CBitBucket(const CBitBucket &b)	{ m_pBucket = NULL; *this = b; }
	~CBitBucket()					{ Cleanup(); }
	void operator=(const CBitBucket &b)
	{
		Init(b.m_nBits);
		for(unsigned long i=0; i<NUM_DWORDS(m_nBits); i++)
			m_pBucket[i] = b.m_pBucket[i];
	}

	// Init and cleanup methods
	void Init(unsigned long nBits)
	{
		Cleanup();
		m_nBits = nBits;
		m_pBucket = new unsigned long[NUM_DWORDS(m_nBits)];
	}
	void Cleanup()
	{
		if(m_pBucket)
		{
			delete m_pBucket;
			m_pBucket = NULL;
		}
	}

	// Direct bit access methods
	operator unsigned long *()		{ return (unsigned long *)m_pBucket; }
	operator unsigned short *()		{ return (unsigned short *)m_pBucket; }
	operator unsigned char *()		{ return (unsigned char *)m_pBucket; }
	bool operator[](int n)			{ return GetBit(n); }
	bool operator()(int n)			{ return GetBit(n); }
	bool GetBit(int n)				{ return ((unsigned long)n >= m_nBits) ? false : (m_pBucket[NUM_DWORDS(n)] & MASK_DWORDS(n)) != 0; }
	void SetBit(int n)				{ m_pBucket[NUM_DWORDS(n)] |= MASK_DWORDS(n); }
	void ClearBit(int n)			{ m_pBucket[NUM_DWORDS(n)] &= ~MASK_DWORDS(n); }
	void FlipBit(int n)				{ m_pBucket[NUM_DWORDS(n)] ^= MASK_DWORDS(n); }
	unsigned long GetBitCount()		{ return m_nBits; }
	unsigned long GetByteCount()	{ return NUM_BYTES(m_nBits); }
	unsigned long GetWordCount()	{ return NUM_WORDS(m_nBits); }
	unsigned long GetDWordCount()	{ return NUM_DWORDS(m_nBits); }

	// Bitwise arithmetic operators (with a fixed DWORD)
	CBitBucket operator&(unsigned long n)
	{
		CBitBucket bRet(m_nBits);
		for(unsigned long i=0; i<NUM_DWORDS(m_nBits); i++)
			bRet.m_pBucket[i] = m_pBucket[i] & n;
		return bRet;
	}
	CBitBucket operator|(unsigned long n)
	{
		CBitBucket bRet(m_nBits);
		for(unsigned long i=0; i<NUM_DWORDS(m_nBits); i++)
			bRet.m_pBucket[i] = m_pBucket[i] | n;
		return bRet;
	}
	CBitBucket operator^(unsigned long n)
	{
		CBitBucket bRet(m_nBits);
		for(unsigned long i=0; i<NUM_DWORDS(m_nBits); i++)
			bRet.m_pBucket[i] = m_pBucket[i] ^ n;
		return bRet;
	}
	void operator&=(unsigned long n)
	{
		for(unsigned long i=0; i<NUM_DWORDS(m_nBits); i++)
			m_pBucket[i] &= n;
	}
	void operator|=(unsigned long n)
	{
		for(unsigned long i=0; i<NUM_DWORDS(m_nBits); i++)
			m_pBucket[i] |= n;
	}
	void operator^=(unsigned long n)
	{
		for(unsigned long i=0; i<NUM_DWORDS(m_nBits); i++)
			m_pBucket[i] ^= n;
	}

	// Bitwise arithmetic operators (with a CBitBucket object)
	CBitBucket operator&(CBitBucket b)
	{
		CBitBucket bRet(m_nBits);
		for(unsigned long i=0; i<NUM_DWORDS(m_nBits); i++)
			bRet.m_pBucket[i] = m_pBucket[i] & b.m_pBucket[i];
		return bRet;
	}
	CBitBucket operator|(CBitBucket b)
	{
		CBitBucket bRet(m_nBits);
		for(unsigned long i=0; i<NUM_DWORDS(m_nBits); i++)
			bRet.m_pBucket[i] = m_pBucket[i] | b.m_pBucket[i];
		return bRet;
	}
	CBitBucket operator^(CBitBucket b)
	{
		CBitBucket bRet(m_nBits);
		for(unsigned long i=0; i<NUM_DWORDS(m_nBits); i++)
			bRet.m_pBucket[i] = m_pBucket[i] ^ b.m_pBucket[i];
		return bRet;
	}
	void operator&=(CBitBucket b)
	{
		for(unsigned long i=0; i<NUM_DWORDS(m_nBits); i++)
			m_pBucket[i] &= b.m_pBucket[i];
	}
	void operator|=(CBitBucket b)
	{
		for(unsigned long i=0; i<NUM_DWORDS(m_nBits); i++)
			m_pBucket[i] |= b.m_pBucket[i];
	}
	void operator^=(CBitBucket b)
	{
		for(unsigned long i=0; i<NUM_DWORDS(m_nBits); i++)
			m_pBucket[i] ^= b.m_pBucket[i];
	}

	// Shifting and rotating functions and operators (n must be between 0 and 31)
	CBitBucket ShiftLeft(int n, unsigned long nEdge=0)
	{
		CBitBucket bRet;
		int nOff = 32-n;
		for(unsigned long i=0; i<NUM_DWORDS(m_nBits)-1; i++)
			bRet.m_pBucket[i] = (m_pBucket[i]<<n) | (m_pBucket[i+1]>>nOff);
		bRet.m_pBucket[i] = (m_pBucket[i]<<n) | nEdge;
		return bRet;
	}
	CBitBucket ShiftRight(int n, unsigned long nEdge=0)
	{
		CBitBucket bRet;
		int nOff = 32-n;
		for(unsigned long i=NUM_DWORDS(m_nBits)-1; i>0; i--)
			bRet.m_pBucket[i] = (m_pBucket[i]>>n) | (m_pBucket[i-1]<<nOff);
		bRet.m_pBucket[i] = (m_pBucket[i]>>n) | nEdge;
		return bRet;
	}
	CBitBucket RotateLeft(int n)	{ return ShiftLeft(n, m_pBucket[0] >> (32-n)); }
	CBitBucket RotateRight(int n)	{ return ShiftRight(n, m_pBucket[NUM_DWORDS(m_nBits)-1] << (32-n)); }
	CBitBucket operator<<(int n)	{ return ShiftLeft(n); }
	CBitBucket operator>>(int n)	{ return ShiftRight(n); }

	void Fill()
	{
		for(unsigned long i=0; i < NUM_WORDS(m_nBits); i++)
			((unsigned short *)m_pBucket)[i] = 0xFFFF;
	}
};

/*
** Problem!!!
** Unable to quickly shift in x, y, and z for quick bit manipulation.
** If you create classes hardcoded in dimensions of 8, 16, or 32, quick hardcoded
** shifts would be possible by shifting bits, bytes, words, or dwords.
**
** For clouds, there is still going to be a problem of moving between parent cells.
** This will be a problem both for interpolating/updating cells and moving due to wind.
*/
class C3DBitBucket : public CBitBucket
{
protected:
	unsigned long nx, ny, nz;

public:
	// Constructors and copy methods
	C3DBitBucket() : CBitBucket()	{}
	C3DBitBucket(unsigned long x, unsigned long y, unsigned long z)
	{
		Init(x, y, z);
	}
	C3DBitBucket(const C3DBitBucket &b)
	{
		*this = b;
	}
	void operator=(const C3DBitBucket &b)
	{
		nx = b.nx;
		ny = b.ny;
		nz = b.nz;
		CBitBucket::operator=(b);
	}

	void Init(unsigned long x, unsigned long y, unsigned long z)
	{
		nx = x;
		ny = y;
		nz = z;
		CBitBucket::Init(x*y*z);
	}

	bool operator()(int x, int y, int z){ return GetBit(x, y, z); }

	int GetBitPos(int x, int y, int z)
	{
		if((unsigned long)x >= nx || (unsigned long)y >= ny || (unsigned long)z >= nz)
			return -1;
		return ((z * ny + y) * nx) + x;
	}
	bool GetBit(int x, int y, int z)	{ return CBitBucket::GetBit(GetBitPos(x, y, z)); }
	void SetBit(int x, int y, int z)	{ CBitBucket::SetBit(GetBitPos(x, y, z)); }
	void ClearBit(int x, int y, int z)	{ CBitBucket::ClearBit(GetBitPos(x, y, z)); }
	void FlipBit(int x, int y, int z)	{ CBitBucket::FlipBit(GetBitPos(x, y, z)); }

	void NoiseFill(int nSeed)
	{
		CFractal fr(3, nSeed, 0.5f, 2.0f);
		srand(nSeed);
		for(unsigned long x=0; x < nx; x++)
		for(unsigned long y=0; y < ny; y++)
		for(unsigned long z=0; z < nz; z++)
		{
			float f[3] = {(float)x/32, (float)y/32, (float)z/16};
			float fNoise = fr.fBm(f, 8.0f);
			if(fNoise > 0.25f)
				SetBit(x, y, z);
			else
				ClearBit(x, y, z);
		}
	}
};

class CCloudCell
{
public:
	unsigned char m_cDensity, m_cBrightness;
};

class CCloudBlock : public CImpostor
{
protected:
	unsigned short nx, ny, nz;
	CCloudCell *m_pGrid;

public:
	CCloudBlock()
	{
		m_pGrid = NULL;
	}
	void Cleanup()
	{
		if(m_pGrid)
		{
			delete m_pGrid;
			m_pGrid = NULL;
		}
	}
	void Init(unsigned short x, unsigned short y, unsigned short z)
	{
		Cleanup();
		nx = x;
		ny = y;
		nz = z;
		m_vDimension = CVector((x+3)*0.5f, (y+3)*0.5f, (z+3)*0.5f);
		m_pGrid = new CCloudCell[nx*ny*nz];
	}

	int GetCellPos(unsigned short x, unsigned short y, unsigned short z)
	{
		if(x >= nx || y >= ny || z >= nz)
			return -1;
		return ((z * ny + y) * nx) + x;
	}
	CCloudCell *GetCloudCell(unsigned short x, unsigned short y, unsigned short z)
	{
		int nPos = GetCellPos(x, y, z);
		if(nPos < 0)
			return NULL;
		return &m_pGrid[nPos];
	}

	void NoiseFill(int nSeed);
	void Light(CVector &vLight);
	void Draw(C3DObject *pCamera, float fHalfSize, bool bImpostor=false);
};
