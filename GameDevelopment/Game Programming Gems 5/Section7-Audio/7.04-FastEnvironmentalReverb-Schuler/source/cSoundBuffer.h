
#ifndef SOUNDBUFFER_INCLUDED
#define SOUNDBUFFER_INCLUDED

template< typename DATATYPE >
class cSoundBuffer  
{
	unsigned m_Size;
	DATATYPE *m_Data;

public:
	unsigned GetSize() const { return m_Size; }
	DATATYPE *GetData() const { return m_Data; }
	void Subtract( const cSoundBuffer< DATATYPE > &ref );
	void Resize( unsigned numsamples );
	void Zero();
	cSoundBuffer< DATATYPE > &operator =( const cSoundBuffer< DATATYPE > &ref );
	cSoundBuffer( unsigned numsamples = 64 );
	~cSoundBuffer();
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

template< typename DATATYPE >
cSoundBuffer<DATATYPE>::cSoundBuffer( unsigned numsamples )
{
	m_Data = new DATATYPE[ numsamples ];
	m_Size = numsamples;

	for( unsigned i = 0; i < m_Size; i++ )
		m_Data[i] = 0;
}

template< typename DATATYPE >
cSoundBuffer<DATATYPE>::~cSoundBuffer()
{
	delete[] m_Data;
}

template< typename DATATYPE >
cSoundBuffer<DATATYPE> &cSoundBuffer<DATATYPE>::operator =( const cSoundBuffer<DATATYPE> &ref )
{
	if( m_Size != ref.m_Size ) 
		Resize( ref.GetSize() );

	for( unsigned i = 0; i < m_Size; ++i ) 
		m_Data[i] = ref.m_Data[i];

	return *this;
}

template< typename DATATYPE >
void cSoundBuffer<DATATYPE>::Subtract( const cSoundBuffer<DATATYPE> &ref )
{
	if( m_Size == ref.m_Size )
	{
		for( unsigned i = 0; i < m_Size; ++i )
			m_Data[i] -= ref.m_Data[i];
	}
}


template< typename DATATYPE >
void cSoundBuffer<DATATYPE>::Resize( unsigned numsamples )
{
	if( m_Size < numsamples )
	{
		delete[] m_Data;
		m_Data = new DATATYPE[ numsamples ];
	}

	m_Size = numsamples;

	for( unsigned i = 0; i < m_Size; i++ )
		m_Data[i] = 0;
}


template< typename DATATYPE >
void cSoundBuffer<DATATYPE>::Zero()
{
	for( unsigned i = 0; i < m_Size; i++ )
		m_Data[i] = 0;
}

#endif
