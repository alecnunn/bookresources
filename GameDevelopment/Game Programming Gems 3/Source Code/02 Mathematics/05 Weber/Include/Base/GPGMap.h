/******************************************************************************
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.

By downloading, copying, installing or using the software you agree to this
license. If you do not agree to this license, do not download, install,
copy or use the software.

Intel Open Source License for "Improved Bones Deformation" and
"Constrained Inverse Kinematics" sample code

Copyright (c) 2002, Intel Corporation.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
	be used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR
JURISDICTION. It is each licensee's responsibility to comply with any export
regulations applicable in each licensee's jurisdiction.
******************************************************************************/

//TEMP
typedef U32 UWORD;

#define GPGMAP_HASH(a,b) ((a) ? (a)((b)) : 0)

extern UWORD GPG_PRIMES[];
#define GPG_MAX_PRIME_CNT	8


template <class K, class V>
class GPGMap
{
	class Pair
	{
		public:
			Pair(void) {}
			~Pair(void) {}
			K	m_key;
			V	m_value;
	};
	class Context
	{
		public:
			Context(void) { m_bucketIndex = 0; }
			UWORD			m_bucketIndex;
			GPGListContext	m_listContext;
	};
	public:
		void	Rewind(void) { Rewind(m_context); }
		void	Rewind(Context &context)
		{
			if(!m_pBuckets) { return; }
			m_pBuckets->GetElement(0).ToHead(context.m_listContext);
			context.m_bucketIndex = 0;
		}
		bool	Next(K &key, V &value) { return Next(key, value, m_context); }
		bool	Next(K &key, V &value, Context &context)
		{
			Pair *pPair;
			if(!m_pBuckets) { return FALSE; }
			while(context.m_bucketIndex < m_pBuckets->GetNumberElements())
			{
				pPair = m_pBuckets->GetElement(context.m_bucketIndex)
					.PostIncrement(context.m_listContext);
				if(pPair)
				{
					key = pPair->m_key;
					value = pPair->m_value;
					return TRUE;
				}
				context.m_bucketIndex++;
				if(context.m_bucketIndex < m_pBuckets->GetNumberElements())
				{
					m_pBuckets->GetElement(context.m_bucketIndex)
						.ToHead(context.m_listContext);
				}
			}
			return FALSE;
		}
		GPGMap(UWORD (*h) (const K & k) = NULL)
		{
			if(h)
			{
				m_pHashFunction = h;
			}
			else
			{
				m_pHashFunction = DefaultHash;
			}
			m_bucketThreshold = 2;
			m_primeIndex = 0;
			m_pBuckets = NULL;
			m_size = 0;
		}
virtual	~GPGMap(void)
		{
			Clear();
		}

		void	Grow(void)
		{
			if(m_pBuckets)
			{
				m_primeIndex++;
				UWORD size;
				if(m_primeIndex >= GPG_MAX_PRIME_CNT)
				{
					size = m_pBuckets->GetNumberElements() * 2;
				}
				else
				{
					size = GPG_PRIMES[m_primeIndex];
				}
				GPGArray< GPGList<Pair> > *newBuckets =
					new GPGArray< GPGList<Pair> >;
				newBuckets->ResizeToAtLeast(size);
				for(UWORD i = 0; i < m_pBuckets->GetNumberElements(); i++)
				{
					GPGListContext context;
					(*m_pBuckets)[i].ToHead(context);
					Pair *p;
					while( ( p = (*m_pBuckets)[i].PostIncrement(context) ) )
					{
						UWORD hash = GPGMAP_HASH(m_pHashFunction, p->m_key)
							% newBuckets->GetNumberElements();
						(*newBuckets)[hash].Append(p);
					}
				}
				if(m_pBuckets) delete m_pBuckets;
				m_pBuckets = newBuckets;
			}
			else
			{
				m_pBuckets = new GPGArray< GPGList<Pair> >;
				m_primeIndex = 0;
				m_pBuckets->ResizeToAtLeast(GPG_PRIMES[m_primeIndex]);
				m_size = 0;
			}
		}

		GPGMap<K, V> &operator=(const GPGMap<K, V> & map)
		{
			throw GPGException("map assignment operator not implemented");
		}

		bool	Lookup(const K & key, V & value) const
		{
			if(!m_pBuckets) { return FALSE; }
			UWORD hash = GPGMAP_HASH(m_pHashFunction, key)
				% m_pBuckets->GetNumberElements();
			GPGListContext context;
			(*m_pBuckets)[hash].ToHead(context);
			Pair *p;
			while( ( p = (*m_pBuckets)[hash].PostIncrement(context) ) )
			{
				if(key == p->m_key)
				{
					value = p->m_value;
					return TRUE;
				}
			}
			return FALSE;
		}

		bool	Insert(const K & key, const V & value)
		{
			if(!m_pBuckets) { Grow(); }
			UWORD hash = GPGMAP_HASH(m_pHashFunction, key)
				% m_pBuckets->GetNumberElements();
			if((*m_pBuckets)[hash].GetNumberElements() == m_bucketThreshold)
			{
				Grow();
				hash = GPGMAP_HASH(m_pHashFunction, key)
					% m_pBuckets->GetNumberElements();
			}
			GPGListContext context;
			(*m_pBuckets)[hash].ToHead(context);
			Pair *p;
			while( ( p = (*m_pBuckets)[hash].PostIncrement(context) ) )
			{
				if(key == p->m_key)
				{
					return FALSE;
				}
			}


			// TODO: do this with a fast allocator
			p = new Pair;
			m_size++;


			p->m_key = key;
			p->m_value = value;
			(*m_pBuckets)[hash].Append(p);
			return TRUE;
		}

		bool	Remove(const K & key)
		{
			if(!m_pBuckets) { return FALSE; }
			UWORD hash = GPGMAP_HASH(m_pHashFunction, key)
				% m_pBuckets->GetNumberElements();
			GPGListContext context;
			(*m_pBuckets)[hash].ToHead(context);
			Pair *p;
			while( ( p = (*m_pBuckets)[hash].PostIncrement(context) ) )
			{
				if(key == p->m_key)
				{
					(*m_pBuckets)[hash].Remove(p);
					delete p;
					m_size--;
					return TRUE;
				}
			}
			return FALSE;
		}

		void	Clear(void)
		{
			if(!m_pBuckets) return;
			for(UWORD i = 0; i < m_pBuckets->GetNumberElements(); i++)
			{
				GPGListContext context;
				(*m_pBuckets)[i].ToHead(context);
				Pair *p;
				while( ( p = (*m_pBuckets)[i].PostIncrement(context) ) )
				{
					(*m_pBuckets)[i].Remove(p);
					delete p;
				}
			}
			delete m_pBuckets;
			m_primeIndex = 0;
			m_pBuckets = NULL;
			m_size = 0;
		}

		UWORD	Size(void) { return m_size; }

static	UWORD	DefaultHash(const K & key)
		{
			return *(unsigned char *)&key;
		}

	private:
		GPGArray< GPGList<Pair> >	*m_pBuckets;
		UWORD (*m_pHashFunction) (const K & k);
		UWORD	m_primeIndex;
		I32		m_bucketThreshold;
		UWORD	m_size;
		Context	m_context;
};

// TODO: iterator



