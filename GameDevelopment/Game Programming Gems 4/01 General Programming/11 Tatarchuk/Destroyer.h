//-----------------------------------------------------------------
// Author: Natalya Tatarchuk
//-----------------------------------------------------------------
#ifndef DESTROYER_H
#define DESTROYER_H

//------------------------------------------------------------------------------------
// IDestroyer class
//
// Interface for the Destroyer class. 
//------------------------------------------------------------------------------------

class IDestroyer 
{
public:
    IDestroyer () {};

    virtual ~IDestroyer () {};

};   // End of class IDestroyer declaration


//------------------------------------------------------------------------------------
// Destroyer class.
//
// The same as the reverse of a ClassFactory - destroys an object only 
// knowing its data type at run-time.
//------------------------------------------------------------------------------------
template <class T> class Destroyer : public IDestroyer
{
public:
    Destroyer() 
    {  
        m_pData = NULL;
    }

    Destroyer ( T* pValue )
    {
        m_pData = pValue;
    }

	virtual ~Destroyer()
    {
        if ( m_pData )
            delete ( m_pData );
    }

private:
    T* m_pData;

};  // End of class Destroyer declaration

#endif 
