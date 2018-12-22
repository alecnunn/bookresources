/*============================================================================
	Title: Containers.h
	Module: Pi/Core
	Author: Ignacio Castaño
	Description: Some usefull containers. Doesn't have external dependencies.
	Changes:
		05/05/2002 - Ignacio Castaño
			Mother's day new file.

============================================================================*/

#ifndef _PI_CONTAINERS_H_
#define _PI_CONTAINERS_H_


/*----------------------------------------------------------------------------
	Doc:
----------------------------------------------------------------------------*/

/**
 * @file Containers.h
 *
 * This containers are based on Thatcher Ulrich <tu@tulrich.com> containers,
 * donated to the Public Domain.
 *
 * I've also borrowed some methods from the Qt toolkit and other sources.
 *
**/



/*----------------------------------------------------------------------------
	Headers:
----------------------------------------------------------------------------*/

//#include "Core.h"
//#include "Debug.h"
//#include "Templates.h"	// for piSwap

#include "PAR.h"

//#include "Integer.h"

#include <new.h>		// for placement new



/*----------------------------------------------------------------------------
	Definitions:
----------------------------------------------------------------------------*/

/**
 * Initial size of the Hash table.
**/
#define PI_BASE_TABLE_SIZE			256



/*----------------------------------------------------------------------------
	Forward decl.:
----------------------------------------------------------------------------*/

template <class T> class PiArray;

template <class K> struct PiDefHash;

template <class K, class V, class F = PiDefHash<K> > class PiMap;


/*----------------------------------------------------------------------------
	Classes:
----------------------------------------------------------------------------*/

/** 
 * Comparator interface. An object that is used to compare two objects.
**/
struct IComparer {
	virtual int operator () (const uint a, const uint b) const =0;
};


/** A generic comparation functor. */
template <class T>
struct TComparer : public IComparer {

	TComparer( PiArray<T> &a ) : array(a) {}

	int operator() ( const uint x, const uint y ) const {
		//return array[x] - array[y];
		if( array[x] < array[y] ) return -1;
		if( array[x] > array[y] ) return 1;
		return 0;
	}

private:
	const PiArray<T> &array;

	PI_DISABLE_COPY(TComparer);
};


/**
 * Container Tag.
**/
class CORE_API PiContainer { };


/**
 * Container by value Tag.
**/
class CORE_API PiValueContainer : public PiContainer { };



/**
 * PiArray is a value-based template class that maintain a resizable array of
 * simple objects, default constructor are called when data is allocated, and
 * destructor is called when freed. The data may be relocated as it gets 
 * resized, so don't keep the address of an element. Copy constructors aren't
 * called during relocation, the data is directly copied instead.
 *
 * The data of the array isn't shared between instances. Copy constructor and 
 * copy method duplicates the data of the array. To share data of the different
 * arrays use the SetRawData and ResetRawData methods, but use them with care!
 */
template<class T>
class CORE_API PiArray : public PiValueContainer {
public:

	/** Ctor. */
	PiArray( void ) 
		: buffer(NULL)
		, size(0)
		, buffer_size(0) { }


	/** Ctor with initial size hint. */
	PiArray( int size_hint ) 
		: buffer(NULL)
		, size(0)
		, buffer_size(0) { Resize(size_hint); }


	/** Copy ctor. */
	PiArray( const PiArray & a ) 
		: buffer(NULL)
		, size(0)
		, buffer_size(0) { Copy( a.buffer, a.size); }


	/** Ctor that initializes the array with the given elements. */
	PiArray( T * ptr, int num ) 
		: buffer(NULL)
		, size(0)
		, buffer_size(0) { Copy( ptr, num); }


	/** Dtor. */
	~PiArray( void ) {
		Empty();
	}


	/** Const and save array access. */
	const T & operator[]( uint index ) const { 
		piLightCheck( index<size ); 
		return buffer[index]; 
	}
	
	/** Safe array access. */
	T & operator[] ( uint index ) { 
		piLightCheck( index<size ); 
		return buffer[index]; 
	}


	/** Get array size. */
	uint Size( void ) const { return size; }

	/** Get const array pointer. */
	const T * Buffer( void ) const { return buffer; }

	/** Get array pointer. */
	T * UnsecureBuffer( void ) { return buffer; }

	/** Is array empty. */
    bool IsEmpty( void ) const { return Size() == 0; }

	/** Is a null array. */
    bool IsNull( void ) const	{ return Buffer() == NULL; }


	/** Push an element at the end of the array. */
	void PushBack( const T & val ) {
		// DO NOT pass elements of your own vector into
		// push_back()!  Since we're using references,
		// resize() may munge the element storage!
		piLightCheck( &val < &buffer[0] || &val > &buffer[size] );

		int	new_size = size + 1;
		Resize( new_size );
		buffer[new_size-1] = val;
	}


	/** Pop and return element at the end of the array. */
	T PopBack( void ) {
		piLightCheck( size > 0 );
		T t = buffer[size-1];
		Resize( size-1 );
		return t;
	}

	/** Get back element. */
	const T & Back( void ) const {
		piLightCheck( size > 0 );
		return buffer[size-1];
	}

	/** Get back element. */
	T & Back( void ) {
		piLightCheck( size > 0 );
		return buffer[size-1];
	}

	/** Get back element. */
	const T & Front( void ) const {
		piLightCheck( size > 0 );
		return buffer[0];
	}

	/** Get back element. */
	T & Front( void ) {
		piLightCheck( size > 0 );
		return buffer[0];
	}


	/** 
	 * Preserve existing elements. Newly created elements are unintialized.
	 * @TODO should not be inline! No reason to bloat the code.
	 */
	void Resize( uint new_size ) {
		piLightCheck( new_size >= 0 );
		uint i;
		uint old_size = size;
		size = new_size;

		// Destruct old elements (if we're shrinking).
		for( i=new_size; i<old_size; i++ ) {
			(buffer+i)->~T();							// Explicit call to the destructor
		}

		if( size == 0 ) {
			buffer_size = 0;
			Reserve(0);
		} 
		else if( size <= buffer_size && size > buffer_size >> 1) {
			// don't compact yet.
		} 
		else {
			if( buffer_size==0 ) buffer_size = size;	// first allocation
			else buffer_size = size + (size >> 2);		// growing
			Reserve( buffer_size );
		}

		// Call default constructors
		for( i=old_size; i<new_size; i++ ) {
			new(buffer+i) T();							// placement new
		}
	}


	/** Empty the buffer. */
	void Empty( void ) {
		Resize(0);
	}


	/** Copy memory to our array. Resizes the array if needed. */
	void Copy( const T * ptr, uint num ) {
		Resize( num );
		for( uint i=0; i<size; i++ ) {
			buffer[i] = ptr[i];
		}		
	}

	/** Assignment operator. */
	void operator=( const PiArray<T> & a ) {
		Copy( a.buffer, a.size );
	}

	/** 
	 * Sets raw data. Do not attempt to resize or re-assign the array data 
	 * when raw data has been set.  This method is usefull to pass data to
	 * methods that take a PiArray as argument without the need of duplicate
	 * or copy the data.
	 *
	 * Example of intended use:
	 *
	 * @code
	 * static uint8 bindata[] = { 231, 1, 44, ... };
     * PiArray<uint8> a;
     * a.SetRawData( bindata, sizeof(bindata) );	// a points to bindata
     * PiStream s( a );								// open on a's data
     * s >> <something>;							// read raw bindata
     * s.close();
     * a.ResetRawData( bindata, sizeof(bindata) );	// finished
	 * @endcode
	 * 
	 * @warning You should call ResetRawData after using the raw data.
	 * If you do not call ResetRawData(), PiArray will attempt 
	 * to deallocate or reallocate the raw data, which might not be too 
	 * good. Be careful.
	 *
	 * @sa ResetRawData
	 */
	void SetRawData( const T * ptr, uint num ) {
		buffer = (T *)ptr;	// warning constness removed!!
		size = num;
		buffer_size = num;
	}

	/**
	 * Resets raw data, that has been set on a previous SetRawData call.
	 *
	 * @sa SetRawData
	 */
	void ResetRawData( const T * ptr, uint num ) {
		piLightCheck( ptr==buffer && size==num && buffer_size==num );
		buffer = NULL;
		size = 0;
		buffer_size = 0;
	}


	/** 
	 * This function sorts the array according to the given comparer functor, using
	 * the qsort algorithm. This breaks when the number of elements is very large, due
	 * to stack overflows.
	 */
	void QSort( const IComparer &cmp ) {
		if( size < 2 ) return;
		QSort_r( cmp, 0, size-1 );
	}

	/** 
	 * This is a replacement for the above, much slower, but it's speed is predictable
	 * and is not recursive.
	**/
	void SelectionSort( const IComparer &cmp ) {
		SelectionSort( cmp, 0, size-1 );
	}


	/* 
	 * Binary search. Search for an element using a binary search, and returns
	 * the index to the first occurrence. 
	 * Previously I was using the Compare functor provided at template time, 
	 * but that was generating too long names and didn't allow me to debug, so 
	 * now I'm using those horrible callbacks.
	 * @warning The array must be sorted, or the result will be undefined.
	 * @sa piDefaultCompare, Sort
	 */
/*	int BSearch( const T & e ) const {
		if( size==0  ) return -1;
		
		uint8 * r = (uint8 *)bsearch( &e, buffer, size, sizeof(T), (int (_CDECL *)(const void *, const void *))F );
		if( r==0 ) return -1;
		
		// search the first occurrence; bsearch is undef 
		while( (r >= (uint8 *)buffer + sizeof(T)) && (F( (T *)(r - sizeof(T)), &e ) == 0) )
			r -= sizeof(T);

		return (int)(( r - (uint8 *)buffer ) / sizeof(T));
	}
*/

/*private:*/

	/** @internal
	 * Unsafe and low-level utility function to replace this array's
	 * members with a's members. 
	 * Should be private and PiMap should be friend, but there's no way to
	 * make it work on both msvc and gcc at the same time.
	 */
	void TransferMembers( PiArray<T> * a ) {
		buffer = a->buffer;
		size = a->size;
		buffer_size = a->buffer_size;

		a->buffer = 0;
		a->size = 0;
		a->buffer_size = 0;
	}


private:

	/** Change buffer size. */
	void Reserve( uint rsize ) {
		piLightCheck( rsize >= 0 );
		
		buffer_size = rsize;

		// free the buffer.
		if( buffer_size == 0 ) {
			if( buffer ) {
				free( buffer );
				buffer = NULL;
			}
		} 

		// realloc the buffer
		else {
			if( buffer ) buffer = (T *) realloc( buffer, sizeof(T) * buffer_size );
			else buffer = (T *) malloc( sizeof(T) * buffer_size );
		}			
	}


	/** Internal sort function. */
	void QSort_r( const IComparer &cmp, int l, int r ) {

		if( r-l < 18 ) { 
			SelectionSort( cmp, l, r );
			return;
		}

		int i, j;
		int num_left, num_right;

		while( l < r ) {

			i = l;
			j = r + 1;

			while( true ) {
				do {
				  i++;
				} while( cmp( i, l) < 0 );
            
				do {
					j--;
				} while( cmp( j, l ) > 0 );
            
				if (i >= j) break;

				piSwap( buffer[i], buffer[j] );
			};
        
			piSwap( buffer[l], buffer[j] );
        
			num_left  = (j-1) - l;
			num_right = r - (j+1);
        
			if( num_left <= num_right ) {
				QSort_r(cmp, l, j-1);
				l = j+1;
			}
			else {
				QSort_r(cmp, j+1, r);
				r = j-1;
			}
		}
	}


	/** Internal selection sort. */
	void SelectionSort( const IComparer &cmp, uint first, uint last ) {
		if( first <= last ) 
			return;

		for( uint a=first; a<last-1; a++ ) {
			
			uint small = a;
			for( uint b=a+1; b<last; b++ ) {
				if( cmp(b, small) < 0 )	small = b;
			}

			piSwap( buffer[a], buffer[small] );
		}
	}


private:
	
	T * buffer;
	uint size;
	uint buffer_size;

};



/** @internal
 * Default hash functor. Computes a hash function of a fixed size object.
**/
template<class T>
struct CORE_API PiDefHash {

	static uint32 GetHash( const T & data ) {
		uint8 * p = (uint8 *) &data;
		int size = sizeof(T);

		uint32 h = 0;
		while( size > 0 ) {
			h = (h << 5) + *p * 101;	// hm, replace with a real hash function.
			p++;
			size--;
		}
		return h;
	}
};


/** @internal
 * Own hash functor. Delegate the hash computation to the own object.
**/
template<class T>
struct CORE_API PiOwnHash {

	static uint32 GetHash( const T & data ) {
		return data.GetHash();
	}
};


/**
 * A functor that does nothing.
**/
template<class T>
struct CORE_API ForEach_Null {
	void operator() ( T & t ) const {
	}
};


/** A functor that frees a pair of the map. */
template<class T>
struct CORE_API ForEach_Free {
	void operator() ( T & t ) const {
		free((void *)t);
	}
};


/** 
 * We don't really need very fast insertion times, so our Map is faked with
 * a hash table, that provides faster access at the cost of probably higher
 * memory cost. PiMap is value-based, it mantains copies of its elements. 
 * See PiDict for a reference-based implementation.
 *
 * Never shrinks, unless you explicitly Empty() or Resize() it.
 * Expands on demand, though. For best results, if you know roughly
 * how big your table will be, default it to that size when you create
 * it.
 *
 * @warning You cannot add two values with the same keys, or will throw an
 * assertion. We could add mechanism to add multiple keys and to get them.
 *
 * @TODO Remove keys. To do this we need a remove method in the array, or we
 * can also use a list instead of an array of arrays.
 *
 * @sa PiDict
 */
template <class K, class V, class F>
class CORE_API PiMap : public PiValueContainer {
public:

	/** Ctor. */
	PiMap() 
		: entry_count(0)
		, size_mask(0) { }

	/** Ctor with size hint. */
	PiMap( int size_hint ) 
		: entry_count(0)
		, size_mask(0) { Resize(size_hint); }

	/** Dtor clears the map. */
	~PiMap() {
		Empty();
	}



	/** Remove all entries from the hash table. */
	void Empty( void ) {
		for( uint i = 0; i < table.Size(); i++ ) {
			table[i].Empty();
		}
		table.Empty();
		entry_count = 0;
		size_mask = 0;
	}

	/** 
	 * Returns true if the map is empty, i.e. Count()==0, in other case 
	 * returns false. 
	**/
	bool IsEmpty( void ) const {
		return entry_count==0;
	}


	/** Returns the number of items in the map. */
	int Count( void ) const {
		return entry_count;
	}


	/** 
	 * Insert a new key-value pair in the hash table. 
	 *
	 * The key does not have to be unique. If multiple items are inserted with 
	 * the same key, only the last item will be visible. 
	**/
	void Insert( const K &key, const V &value ) {

		entry_count++;
		
		CheckExpand();

		int	hash_value = F::GetHash(key);
		E e;
		e.key = key;
		e.value = value;

		int	index = hash_value & size_mask;		// % table.size();
		table[index].PushBack(e);
	}


	/* *
	 * Remove the item with the given key. If multiple items with that key exist
	 * only the last one is removed.
	 *
	 * @warning For optimization reasons you should never call this when the map
	 * is empty. Use IsEmpty() to determine that.
	**/
	/*bool Remove( const K &key, V * value ) {
		
		int	hash_value = F::GetHash(key);
		int	index = hash_value & size_mask;			// % table.size();

		for( int i=0; i<table[index].Size(); i++ ) {
			if( table[index][i].key == key ) {

				for( int j=i; j<table[index].Size(); j++ ) {
				}

				return true;
			}
		}
		return false;
	}*/
	



	/**
	 * Get the value under the given key.
	 *
	 * If there's no value under the key, then return false and leave
	 * *value alone.
	 *
	 * If there is a value, return true, and set *value to the entry's
	 * value. If there are multiple values, only considers the last inserted
	 * value.
	 *
	 * If value == NULL, just returns true when the first ocurrence is found.
	**/
	bool Get( const K &key, V * value ) {

		if( table.Size() == 0 )
			return false;

		int	hash_value = F::GetHash(key);
		int	index = hash_value & size_mask;			// % table.size();

		int i;
		int size = table[index].Size();
		int last = -1;

		for( i=0; i<size; i++ ) {
			if( table[index][i].key == key ) {
				if( value ) last = i;
				else return true;
			}
		}

		if( last!=-1 ) {
			*value = table[index][last].value;
			return true;
		}
		else
			return false;
	}


	/** 
	 * Replaces the value of the @a key, key with @a value. 
	 *
	 * If the item does not already exist, it will be inserted.
	 *
	 * If there are two or more items with equal keys, then the last item 
	 * that was inserted will be replaced. 
	**/
	void Replace( const K &key, const V &value ) {

		if( table.Size() == 0 )
			Insert( key, value );

		int	hash_value = F::GetHash(key);
		int	index = hash_value & size_mask;			// % table.size();

		int i;
		int size = table[index].Size();
		int last = -1;

		for( i=0; i<size; i++ ) {
			if( table[index][i].key == key )
				last = i;
		}
		
		if( last!=-1 )
			table[index][last].value = value;
		else
			Insert( key, value );

	}


	/**
	 * Resize the hash table to the given size (Rehash the contents of
	 * the current table).
	**/
	void Resize( uint new_size ) {
		uint i, j;

		if( new_size <= 0 ) {
			// Special case.
			Empty();
			return;
		}

		// Force new_size to be a power of two.
		uint bits = Log2(new_size - 1) + 1;
		piLightCheck( uint(1<<bits) >= new_size );

		new_size = 1 << bits;			// Next power of two.
		size_mask = new_size - 1;

		PiArray< PiArray<E> >	new_table( new_size );

		// Init entries in new_table, since constructors aren't called.
		for( i=0; i<new_size; i++ ) {
			PiArray<E> nullentry(0);
			new_table[i].TransferMembers(&nullentry);
		}

		// Copy all entries to the new table.
		for( i=0; i<table.Size(); i++ ) {
			for( j=0; j<table[i].Size(); j++ ) {

				E & e = table[i][j];
				int	hash_value = F::GetHash( e.key );
				
				int	index = hash_value & size_mask;	// % new_table.size();
				new_table[index].PushBack( e );
			}
			table[i].Empty();
		}

		table.Empty();

		// Replace old table data with new table's data.
		table.TransferMembers( &new_table );
	}


	/** Remove all entries from the hash table. */
	template<class fk, class fv>
	void ForEach( const fk & FK, const fv & FV=ForEachNull<V> ) {
		for( uint h = 0; h < table.Size(); h++ ) {
			for( uint i = 0; i < table[h].Size(); i++ ) {
				FK( table[h][i].key );
				FV( table[h][i].value );
			}
		}
	}

protected:

	/**
	 * Resize the hash table, if it looks like the size is too small
	 * for the current number of entries.
	**/
	void CheckExpand( void ) {
		uint	new_size = table.Size();

		if( table.Size()==0 && entry_count>0 ) {
			// Need to expand.  Let's make the base table size 
			// PI_BASE_TABLE_SIZE elements.
			new_size = PI_BASE_TABLE_SIZE;
		} 
		else if( table.Size()*2 < entry_count ) {
			// Expand.
			new_size = (entry_count * 3) >> 1;
		}

		if( new_size != table.Size() ) {
			Resize( new_size );
		}
	}



private:

	/** @internal An entry in the hash table. */
	struct E {
		K	key;
		V	value;
	};
	
	/** Number of entries in the hash. */
	uint	entry_count;

	/** This is the size-1. */
	uint	size_mask;

	/** An array of arrays */
	PiArray< PiArray<E> >	table;

};





/**
 * The PiPtrContainer is the base class for all the pointer-based containers.
 *
 * The PiPtrContainer only provides a counter for the number of objects 
 * contained and the deletion strategy. (see SetAutoDelete()).
 *
**/
class PiPtrContainer : public PiContainer {
public:

	/** 
	 * Set autodeletion.
	 * @TODO finish documentation.
	**/
	void SetAutoDelete( bool b ) {
		auto_delete = b;
	}


	/**
	 * Returns true if auto deletion is true, and false in other case.
	 * @sa SetAutoDelete()
	**/
	bool AutoDelete( void ) const {
		return auto_delete;
	}


	/** Get the number of items in the container. */
	int Count( void ) const { return count; }



protected:

	PiPtrContainer() 
		: auto_delete(false)
		, count(0) { }

	PiPtrContainer( const PiPtrContainer & c ) 
		: auto_delete(false)
		, count(c.count) { }

	virtual ~PiPtrContainer() { }


private:

	bool auto_delete;			///< @internal
	uint32 count;				///< @internal

};








#endif // _PI_CONTAINERS_H_
