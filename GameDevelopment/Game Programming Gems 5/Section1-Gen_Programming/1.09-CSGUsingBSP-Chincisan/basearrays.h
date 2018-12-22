/*
	Csg by Bsp.
	Author:			Marius Chincisan, 
	Developed for	Game Programming Gems 5
*/

#ifndef __BASEARRAYS_H__
#define __BASEARRAYS_H__

#include "basemath.h"

//------------------------------------------------------------------------------------
// add's a index member to the class definition
//------------------------------------------------------------------------------------
typedef int (*PFNSORT)(const void* e1, const void* e2);

//------------------------------------------------------------------------------------
// Regular stack indexed parray.
template <class T, int sizesz = 2048>
class parray
{
public:
    parray():_elems(0){}
    ~parray(){}
    void ClearPtr(){
        while(--_elems>=0){
            delete t[_elems];
            t[_elems]=0;
        }
        _elems=0;
    }
    T& operator[](int idx){
        return t[idx];
    }
    T& At(int idx){
        return t[idx];
    }
    void Set(T& tt, int idx){
        assert(idx < _elems);
        t[idx] = tt;
        tt->_index =idx;
    }
    T* Push(T el){
        if(_elems < sizesz){
            el->_index = _elems;
            t[_elems] = el;
            return &t[_elems++];
        }
        return 0;
    }
    T& Pop(){
        return t[--_elems];
    }
    T* GetBuff(){return t;}

    void Remove(T pt)
    {
        assert(_elems>0);
        int index = pt->_index;
        if(index != _elems-1)
        {
            t[index] = t[_elems-1];
            t[index]->_index = index;
        }
        _elems--;
    }

    void RemoveIdx(int idx){
        assert(_elems>0);
        if(idx != _elems-1)
        {
            t[idx] = t[_elems-1];
            t[idx]->_index = idx;
        }
        _elems--;
    }
    void Reset(){
        _elems=0;
    }
    INLN int Count() {
        return _elems;
    }
    int ElemSize(){
        return sizeof(T);
    }
    void FullSort(PFNSORT sfn){
        qsort((void*)t, _elems, ElemSize(), sfn);
    }
    BOOL PartialSort(PFNSORT sfn){
        for(int i=1; i<_elems; i++){
            if(sfn(t[i-1], t[i]))
            {
                SWITCHI((DWORD)t[i-1],(DWORD)t[i]);
                return TRUE;
            }
        }
        return FALSE;
    }
protected:
    T	t[sizesz];
    int _elems;
};

//------------------------------------------------------------------------------------
// simpla non indexed stack parray
template <class T, int sizesz = 128>
class rarray
{
public:
    rarray():_elems(0){}
    ~rarray(){DeleteElements();}
    void DeleteElements(){
    }
    T& operator[](int idx){
        return t[idx];
    }
    T& At(int idx){
        return t[idx];
    }
    int Set(T& tt , int idx){
        assert(idx < _elems);
        t[idx] = tt;
    }
    void Push(T el){
        if(_elems < sizesz)
            t[_elems++] = el;
    }
    T& Pop(){
        return t[--_elems];
    }
    T& Last(){
        return t[_elems-1];
    }

    T* GetBuff(){
        return t;
    }
    void Remove(int idx){
        assert(_elems);
        t[idx] = t[_elems-1];
        _elems--;
    }
    void Remove(T& e){
        for(int i=0;i<_elems;i++){
            if(t[i] == e){
                Remove(i);
                return;
            }
        }
    }

    void Reset(){
        _elems=0;
    }
    INLN int Size(){
        return _elems;
    }
    int ElemSize(){
        return sizeof(T);
    }
protected:
    T	t[sizesz];
    int _elems;
};
typedef rarray<DWORD,128>	Dw128;
//------------------------------------------------------------------------------------
//  pooling object
template <class T>
class Pool
{

public:
    static void CreatePool(UINT cap)
    {
        _pvect = new vector<T*>();
        _pvect->reserve(cap);

        _pbhead = (T*) ::malloc(cap * sizeof(T));
        T* pw	= _pbhead;

        for(UINT i=0; i< cap; i++)
        {
            _pvect->push_back(pw++);
        }
    }

    static void DestroyPool(){
        if(0==_pvect)return;
        _pvect->clear();
        delete _pvect;
        ::free((void*)_pbhead);
        _pbhead=0;
    }

    void* operator new(size_t sz)
    {
        if(T::_pvect->size() > 0)
        {
            int sz = _pvect->size();
            void* pv = (void*)_pvect->back();
            _pvect->pop_back();
            sz = _pvect->size();
            _inUse++;
            return (pv);
        }
        return 0;
    }
    void operator delete(void* pv)
    {
        _inUse--;
        _pvect->push_back((T*)pv);
    }

    static T*				_pbhead;
    static vector<T*>*		_pvect;
    static UINT			    _nCapacity;
    static UINT			    _inUse;
};

template <class T> T*				Pool<T>::_pbhead;
template <class T> vector<T*>*		Pool<T>::_pvect;
template <class T> UINT				Pool<T>::_nCapacity;
template <class T> UINT				Pool<T>::_inUse;


class ByArr
{

public:
    ByArr() : _byarr(0), _size(0) {}
    ~ByArr() {
        delete[] _byarr;
        _byarr = NULL;
    }

    void Reserve(int count) {
        _size = count/32 + 1;
        delete[] _byarr;
        _byarr = new DWORD[_size];
        Reset();
    }

    void operator = (int i){
        _byarr[i >> 5] |= (1 << (i & 31));
    }

    BOOL operator == (int i){
        return _byarr[i >> 5] & (1 << (i & 31 ));
    }

    void Reset(int i) {
        _byarr[i >> 5] &= ~(1 << (i & 31));
    }

    void Reset()
    {
        memset(_byarr, 0, sizeof(unsigned int) * _size);
    }


private:
    DWORD*	_byarr;
    int		_size;
};


template <class T>
struct rbuffer
{
    T*      _ptr;
    long    _size;
    rbuffer():_ptr(0),_size(0){}
    rbuffer(const rbuffer& r)
    {
        _size = r._size;
        _ptr = new T[_size];
        memcpy(_ptr,r._ptr,_size*sizeof(T));
    }
    ~rbuffer(){
        delete[] _ptr;
        _ptr=0;
        _size=0;
    }
    rbuffer<T>& operator= (const rbuffer<T>& r)
    {
        Destroy();
        _size = r._size;
        _ptr = new T[_size];
        memcpy(_ptr,r._ptr,_size*sizeof(T));
        return *this;
    }
    void Destroy()
    {
        delete[] _ptr;
        _ptr=0;
        _size=0;
    }
    T* operator&(){
        return _ptr;
    }

    T& operator[](int i){
        return _ptr[i];
    }

    void ByteReserve(int sz){
        _size = sz/sizeof(T);
        _ptr = new T[_size];
        //	memset(_ptr,0,_size);
    }
    void ObjReserve(int sz){
        _size = sz;
        _ptr = new T[_size];
        int sizeoft = sizeof(T);
        //		memset(_ptr, 0, _size);
    }
    long ByteSize(){return _size*sizeof(T);}
};

struct Pair
{
    int _start;
    int	_count;
};


template <class T, size_t SZ>
class vring
{
public:
    vring(){
        _p   = new T[SZ];
        _gp  = _pp = _cnt = 0;
    }
    ~vring(){
        delete[] _p;
    }
    void put(T& t){
        _p[_pp]=t;
        _pp = ++_pp % SZ;
        if(_gp == _pp)
            _gp = ++_gp % SZ;
        else
            _cnt++;
    }
    T* get(){
        if(_cnt){
            T* pret = &_p[_gp];
            _gp = ++_gp % SZ;
            --_cnt;
            return pret;
        }
        return 0;
    }
    int size(){return _cnt;}

private:
    T*  _p;
    int _gp;    // put position
    int _pp;    // get position
    int _cnt;   // elems in riung
};

#endif //__BASEARRAYS_H__

