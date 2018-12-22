/*===========================================================================
	Title: Array.h
	Author: Oscar Blasco
	Description: Growable array template
	Changes:
===========================================================================*/


#ifndef _NM_ARRAY_H
#define _NM_ARRAY_H

/** 
 * Array class I have been using for some long time...it is far from perfect but
 * does the work correctly
 */
template <class T, int gf = 0> class nmArray
{
protected:
	BOOL InitArray;

public:
	nmArray(): Elements(0), Allocated(0), InUse(0), growfactor(gf), InitArray(0){}
	nmArray(int initsize, void *initdata = 0, int datasize = 0 )
	: Allocated(initsize)
	, InUse(0)
	, Elements(0)
	, growfactor(gf)
	, InitArray(0)
	{
		Elements = new T[ initsize ];
		memset( Elements, 0, sizeof(T) * initsize );
		if( initdata ){
			if( datasize > Allocated ) Grow( Allocated - datasize );
			// Grow reajust Allocated variable!
			memcpy( Elements, initdata, sizeof(T) * Allocated );
		}
			
	}
	~nmArray()
	{
		if(!InitArray) Free();
	}

	void InitialArray(int size, void *initdata){
		InitArray = TRUE;
		Allocated = size;
		InUse = 0;
		Elements = (T*) initdata;
	}
	_inline void Grow(int size){				// Grow the array (size+growfactor)
		T *tmp = new T[ Allocated + size + growfactor ];
		memcpy( tmp, Elements, sizeof(T) * Allocated );
		delete Elements;
		Elements = tmp;
		Allocated += size + growfactor;
	}
	_inline int Add(const T *v ) 					// Only one value; returns index
	{
		if( Allocated == InUse )
			Grow( (Allocated+1)*2 );
		memcpy(&Elements[InUse++], v, sizeof(T));
		return (InUse-1);
	}
	_inline int Add(const T *v, int size )
	{
		int s = size;
		if(!size) return 0xFFFF;
		if( size > Allocated - InUse ) 
			Grow( Allocated - InUse + size );
		memcpy(&Elements[InUse], v, sizeof(T)*size); 
		InUse+=size;
		return InUse;
	}
	__inline int Size(){ return InUse; }
	__inline int nAllocated(){ return Allocated; }
	int FreeSpace(){ return ( Allocated - InUse ); }
	void Clear(){							// Dont't appFree array
		InUse = 0;
	}
	void Free(){							// appFree it
		if(!InitArray)
			if(Elements) delete [] Elements;
		Elements = NULL;
		Allocated = 0;
	}
	__inline T* Get(int index)				// Pointer to an element
	{
		if((index+1) > Allocated || index < 0) return NULL;
		return &Elements[index];
	}
	__inline T* Get()						// Pointer to elemnts list
	{
		return &Elements[0];
	}
	__inline void Deselect(int index)
	{
		Elements[index] = NULL;
	}
	void SetGrowFactor(int gwf){
		growfactor = gwf;
	}

	int operator<< (T *data){
		return Add(data);
	}  

	void operator >> (T *data){
		data = Get(InUse--);
		Deselect(InUse);
	}

	T operator [] (int id){
		return *Get(id);
	}
	
protected:
	int growfactor;
	int Allocated;
	int InUse;
	T *Elements;
};


/**
 * This is an specialized list of int's with an add function wich tests if
 * the same value is already added
 * I know, it is not a child class from the nmArray
 */
class TiIDlist
{
private:
	int Allocated;
	int InUse;
	int *Elements;

public:
	TiIDlist(): Elements(0), Allocated(0), InUse(0){}
	~TiIDlist()
	{
		if(Elements) free(Elements);
		Elements = 0;
	}
	_inline void Grow(int size){			
		int *tmp = (int*) realloc( Elements, sizeof(int) * ( Allocated + size ) );
		Elements = tmp;
		Allocated += size;
	}
	_inline int Add(const int v ) 				// Only one value; returns index
	{
		if( Allocated == InUse )
			Grow( (Allocated + 10)*10 );
		memcpy(&Elements[InUse], &v, sizeof(int));
		return InUse++;
	}
	_inline int AddWithCheck(const int v ) 				
	{
		for(int i = 0; i < InUse; i++)
			if(Elements[i] == v) return -1;
		if( Allocated == InUse )
			Grow( (Allocated + 10)*10 );
		memcpy(&Elements[InUse], &v, sizeof(int));
		return InUse++;
	} 
	__inline int Size(){ return InUse; }
	int nAllocated(){ return Allocated; }
	int FreeSpace(){ return ( Allocated - InUse ); }
	void Clear(){								// Dont't free array
		InUse = 0;
	}
	void Free(){								// Free it
		if(Elements)	
			free(Elements);
		Elements = 0;
	}
	__inline int* Get(int index = 0)		
	{
		if(((int)index+1) > Allocated || index < 0) return NULL;
		return &Elements[index];
	}
	int Find(int id){
		for(int i = 0; i < InUse; i++)
			if(Elements[i] == id) return i;
		return -1;
	}

	int operator<< (int data){
		return Add(data);
	}  

	void operator >> (int data){
		data = *Get(InUse--);
	}

	int operator [] (int id){
		return *Get(id);
	}
};

#endif // _TI_ARRAY_H
