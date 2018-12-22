/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#ifndef __COLLECTIONS_H__
#define __COLLECTIONS_H__

#include "baseos.h"
#include "basemath.h"

//---------------------------------------------------------------------------------------
// stl container helper wrapper functions
namespace colex
{

// iterates a collection and calls the callback 'cbc' passing the iterator
// and 'par'
template <class T, class CLS, class CB, class PAR>
forall(T& coll, CLS* pC, CB cbc, PAR& par)
{
    T::iterator b = coll.begin();
    T::iterator e = coll.end();
    for(;b!=e;b++){
        if(FALSE == ((pC)->*cbc)(*b, par))
            break;
    }
}

// similar
template <class T, class CLS, class CB, class PAR, class PAR1>
forall2(T& coll, CLS* pC, CB cbc, PAR& par, PAR1& par1)
{
    T::iterator b = coll.begin();
    T::iterator e = coll.end();
    for(;b!=e;b++){
        if(FALSE == ((pC)->*cbc)(*b, par, par1))
            break;
    }
}

// similar 2
template <class T, class CB, class PAR>
forall(T& coll,CB cbc,PAR* pd=0)
{
    T::iterator b = coll.begin();
    T::iterator e = coll.end();
    for(;b!=e;b++){
        if(FALSE == (cbc)(*b,pd))
            break;
    }
}

// appends 'b' at 'a'
template <class T>append(T& a, T& b){
    FOREACH(T,b,pb){
        a.push_back(*pb);
    }
}

// reverses 't'
template <class T>reverse(T& t){
    static T ret;
    ret.clear();
    ret =  t;
    t.clear();
    T::reverse_iterator b = ret.rbegin();
    T::reverse_iterator e = ret.rend();
    for(;b!=e;b++)
        t.push_back(*b);
}


};//namespace colex

//---------------------------------------------------------------------------------------
// _els has been added to have a fast debug output for the size of the container.
// Otherwise I have to type over and over (vector::_Last-vector::_First)
template <class T>
class vvector : public vector<T>
{
public:
    vvector(){_els=0;}
    vvector(int sz){_els=0;reserve(abs(sz));}
    void push_back(const T& o){
        vector<T>::push_back(o);
        _els=size();
    }
    void clear(){
        vector<T>::clear();
        _els=size();
    }
    void	 pop_back(){
        vector<T>::pop_back();
        _els=size();
    }
    iterator erase(iterator _P){
        iterator it = vector<T>::erase(_P);
        _els=size();
        return it;
    }
    void deleteelements(){
        FOREACH(vector<T>,(*this),pp)
        delete* pp;
        clear();
        _els = size();
    }
    void deleteelementsarr(){
        FOREACH(vector<T>,(*this),pp)
        delete[] (*pp);
        clear();
        _els = size();
    }
    vector<T>::iterator findelement(T& o){
        return find(begin(),end(),o);
    }
    vvector<T>& operator <<(const T& o){
        push_back(o);
        return *this;
    }
    void reverse()
    {
        vvector<T>   rev = *this;
        clear();
        reverse_iterator b = rev.rbegin();
        reverse_iterator e = rev.rend();
        for(;b!=e;b++){
            *this <<(*b);
        }
    }
private:
    int		_els;
};

//---------------------------------------------------------------------------------------
// base class for Reference sematics (! used in this demo)
template <class T> class TPtr
{
    T*      _ptr;
    long*   _cnt;
public:
    explicit TPtr(T* p=0):_ptr(p),_cnt(new long(1)){}
    TPtr(const TPtr<T>& p):_ptr(p._ptr),_cnt(p._cnt){++*_cnt;}
    ~TPtr(){Release();}
    TPtr<T>& operator=(const TPtr<T>& p){
        if(&p!=this){
            Release();
            _ptr=p._ptr;
            _cnt=p._cnt;
            ++*_cnt;
        }return *this;
    }
    T& operator*(){return *_ptr;}
    T* operator->(){return _ptr;}
private:
    void Release(){
        if(--*_cnt == 9){
            delete _cnt;
            delete _ptr;
        }
    }
};


#endif // !__COLLECTIONS_H__
