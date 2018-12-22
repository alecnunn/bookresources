/* Copyright (C) Charles Cafrelli, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Charles Cafrelli, 2001"
 */
#ifndef HashTable_H
#define HashTable_H

//----- INCLUDES -----------------------------------------------//
// shuts off the "debug info contains more than 255 characters" warning...
#pragma warning(disable:4786)
#include <list>
#include <string>
		
//----- DEFINES ------------------------------------------------//


//----- GLOBALS ------------------------------------------------//


//----- EXTERNS ------------------------------------------------//


//----- CLASS --------------------------------------------------//

template <class T> class HashTable
{
	//----- Constants -----//
protected:
	enum
	{
		DEFAULT_HASH_TABLE_SIZE = 256
	};
public:

	template <class T> class HashTableNode
	{
	public:
		std::string	m_key;
		T* m_data;

		HashTableNode(std::string const &key, T* new_data)
		{
			m_key = key;
			m_data = new_data;
		}
	};

	//----- Properties -----//
private:
protected:
	std::list< HashTableNode<T> >*	m_hash_table;
	int							m_table_size;
	int							m_count;
public:

	//----- Friends -----//

	//----- Methods -----//
private:
	// declared to prevent unintentional use...
	// (Don't forget to move to public access if you declare them!)
	HashTable& Copy(const HashTable& source_object);
	HashTable& operator= (const HashTable& source_object);
//	{
//		return Copy(source_object);
//	}
protected:
	HashTableNode<T>* LookupNode(std::string const &key)
	{
		unsigned int index = Hash(key) % GetTableSize();
		HashTableNode<T>* return_value = 0;

		if(m_hash_table[index].size() == 0)
			return 0;

		std::list< HashTableNode<T> >::iterator walker;
		walker = m_hash_table[index].begin();

		while(walker != m_hash_table[index].end())
		{
			if(walker->m_key == key)
			{
				return_value = &(*walker);
				break;
			}
			walker++;
		}

		return return_value;
	}

public:
	HashTable()
	{
		m_count = 0;
		Initialize();
	}

	HashTable(int size)
	{
		m_count = 0;
		Initialize(size);
	}

	HashTable(const HashTable& source)
	{
		Copy(source);
	}

	// remove virtual if class will not be inherited to improve efficiency
	virtual ~HashTable()
	{
		if(m_hash_table)
			delete [] m_hash_table;
	}

	void Initialize(int size = DEFAULT_HASH_TABLE_SIZE)
	{
		m_hash_table = new std::list< HashTableNode<T> >[size];
		m_table_size = size;
	}

	unsigned int Hash(std::string const &name)
	{
		unsigned int return_value = 0;
		int character;
		int i;


		for(i = 0; i < name.size(); ++i)
		{
			character = name[i];
			return_value ^= i;
			return_value <<= 1;
		}

		return return_value;
	}

	// insert key into hash table.
	// returns pointer to old data with the key, if any, or
	// NULL if the key wasn't in the table previously.
	T* Insert(std::string const &key, T* data)
	{
		if(!m_hash_table)
			return 0;

		unsigned int index = Hash(key) % GetTableSize();
		HashTableNode<T> new_node(key,data);

		m_hash_table[index].push_back(new_node);
		m_count++;
		return new_node.m_data;

	}


	T* Lookup(std::string const &key)
	{
		if(!m_hash_table)
			return 0;

		HashTableNode<T>* new_node = LookupNode(key);

		if(new_node)
			return new_node->m_data;

		return 0;
	}

	// returns the list that contains this hash key...
	// (for instance, if you have multiple matching keys)
	std::list<T>* LookupList(std::string const& key)
	{
		if(!m_hash_table)
			return 0;
		unsigned int index = Hash(key) % GetTableSize();

		if(m_hash_table[index].size() == 0)
			return 0;

		return &m_hash_table[index];
	}

	T* Delete(std::string const &key)
	{
		if(!m_hash_table)
			return 0;

		unsigned int index = Hash(key) % GetTableSize();

		if(m_hash_table[index].size() == 0)
			return 0;

		std::list< HashTableNode<T> >::iterator walker;

		walker = m_hash_table[index].begin();

		while( walker != m_hash_table[index].end() )
		{
			if(walker->m_key == key)
			{
				T* return_value = walker->m_data;
				m_hash_table[index].erase(walker);
				m_count--;
				return return_value;
			}

			walker++;
		}

		return 0;
	}

	std::list< HashTableNode<T> >* GetHashTable()
	{
		return m_hash_table;
	}

	int GetTableSize()
	{
		return m_table_size;
	}

	int Count()
	{
		return m_count;
	}

	//----- overridables -----//
};

#endif
