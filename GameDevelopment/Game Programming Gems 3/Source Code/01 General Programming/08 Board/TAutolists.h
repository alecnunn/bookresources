// TAutolists.h: interface for the TAutolists class.
//
////////////////////////////////////////////////////

#ifndef _TAUTOLISTS_H_
#define _TAUTOLISTS_H_

#include <list>

template <class T>
class TAutolists
{
public:
    TAutolists()
    { 
        ms_List.push_front(static_cast<T*>(this));
    }

    virtual ~TAutolists()
    { 
        list<T>::iterator pos = ms_List.find( 
            static_cast<T*>(this) ); 
        ms_List.remove_at( pos ); 
    }

public:
    static T* GetAutolistFirst()
    {
        ASSERT(ms_ListIter==NULL);
        ms_ListIter = ms_List.begin (); 
        if (ms_ListIter==NULL)
            return NULL;
        else
            return ms_List.get_next(ms_ListIter);
    }

    static T* GetAutolistNext()
    { 
        if (ms_ListIter==NULL)
            return NULL;
        else
            return ms_List.get_next(ms_ListIter);
    }

    static unsigned int GetAutolistCount()
    {
        return ms_List.size ();
    }
        
private:

    // The actual list and list pointer
    static list<T>            ms_List;
    static list<T>::iterator    ms_ListIter;

};

// Declarations of static variables
template<class T> list<T>    TAutolists<T>::ms_List;

template<class T> list<T>::iterator        TAutolists<T>::ms_ListIter;

#endif



