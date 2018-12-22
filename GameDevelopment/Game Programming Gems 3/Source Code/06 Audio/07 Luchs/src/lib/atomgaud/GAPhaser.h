// GAPhaser - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0202
// Author:		Frank Luchs
// History:
//				2001-10-03	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#ifndef _atomosGAPhaser_
#define _atomosGAPhaser_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAPhaser : public IGAProcessor
{



}; // interface


class ATOMGAUD_DLLCLASS CGAPhaser : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GAPhaser };
	static const CClass classCGAPhaser;
	virtual const IClass*		GetClass();

	
	virtual void Terminate();
	virtual UINT32			Process(IObject* pContext=NULL);

	// IGAPhaser
float ProcessSample( float inSamp );


/*
    void Range( FLOAT32 fMin, FLOAT32 fMax );
    void Rate( FLOAT32 rate );
    void Feedback( FLOAT32 fb );
    void Depth( FLOAT32 depth );

    FLOAT32 Process( FLOAT32 inSamp );
*/
     
	// CTOR / DTOR
	CGAPhaser();
	~CGAPhaser();


private:
	/*
	class CGAAllpassDelay{
    public:
    	CGAAllpassDelay() : m_a1( 0.0f ), m_zm1( 0.0f ) {}

        void Delay( FLOAT32 fDelay ) //sample delay time
		{ 
        	m_a1 = (1.0f - fDelay) / (1.0f + fDelay);
        }

        FLOAT32 Process( FLOAT32 fIn )
		{
        	FLOAT32 fOut = fIn * - m_a1 + m_zm1;
        	m_zm1 = fOut * m_a1 + fIn;

            return fOut;
        }

    private:
    	FLOAT32 m_a1;
		FLOAT32 m_zm1;
    };
*/

		class CGAAllpassDelay{
    public:
    	CGAAllpassDelay()
        	: _a1( 0.f )
            , _zm1( 0.f )
            {}

        void Delay( ATOM_FLOAT delay ){ //sample delay time
        	_a1 = (1.f - delay) / (1.f + delay);
        }

        ATOM_FLOAT Process( ATOM_FLOAT inSamp ){
        	ATOM_FLOAT y = inSamp * -_a1 + _zm1;
        	_zm1 = y * _a1 + inSamp;

            return y;
        }
    private:
    	ATOM_FLOAT _a1, _zm1;
    };


protected:

    CGAAllpassDelay m_alps[6];

    FLOAT32 m_dmin;
	FLOAT32 m_dmax; //range

    FLOAT32 m_fb; //feedback
    FLOAT32 m_lfoPhase;
    FLOAT32 m_lfoInc;
    FLOAT32 m_depth;

    FLOAT32 m_zm1;


};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAPhaser_
// --------------------------------------------------------------------------------------------------------
