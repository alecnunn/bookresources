// ----------------------------------------------------------------------------
// 
// This file is part of the XDS Toolkit, Copyright (C) 2003 Mark T. Price
// For conditions of distribution and use, see XdsLicense.h
//
// ----------------------------------------------------------------------------

#include <stdlib.h>

#include "Trie.h"

// ***** Internal Utility Routines - - - - - - - - - - - - - - - - - - - - - -

// Bitmap helper functions

enum { BITSPERBYTE = 8 };

inline int bit_check(unsigned char *bmdata, int bitnum)
{
	return (int)(( *((bmdata) + ((bitnum)-1)/BITSPERBYTE) >>
	  ((BITSPERBYTE-1) - ((bitnum)-1)%BITSPERBYTE) )&1);
}

inline void bit_set(unsigned char *bmdata, int bitnum)
{
	( *((bmdata) + ((bitnum)-1)/BITSPERBYTE)  |=  
	  (1<<((BITSPERBYTE-1) - ((bitnum)-1)%BITSPERBYTE)) );
}

inline void bit_clear(unsigned char *bmdata, int bitnum)
{
	( *((bmdata) + ((bitnum)-1)/BITSPERBYTE)  &= 
	  ~(1<<((BITSPERBYTE-1) - ((bitnum)-1)%BITSPERBYTE)) );
}

/*
**  KeyCmp() -- compare keys
**
**  key1, len1  -- key number 1 (&length in bytes)
**  key2, len2  -- key number 2 (&length in bytes)
**
**  RETURNS:
**   <0 = (degenerate case) one or both of the keys has 0 length
**    0 = keys are the same
**   >0 = 1st bit number where keys differ
*/
static int KeyCmp(unsigned char *key1, int len1, unsigned char *key2, int len2)
{
	int ii, bitnum;
	int bv1, bv2;

	if(!len1 || !len2)
		return -1;

	for(ii = 0; ii < len1 && ii < len2; ++ii)
	{
		if(key1[ii] != key2[ii])
		{
			for(bitnum = ii*BITSPERBYTE+1; bitnum < (ii+1)*BITSPERBYTE; ++bitnum)
			{
				bv1 = bit_check(key1, bitnum);
				bv2 = bit_check(key2, bitnum);
				if((bv1 && !bv2) || (!bv1 && bv2))
					break;
			}
			return bitnum;
		}
	}

	if(len1 == len2)
	{
		return 0;
	}

	return ii*BITSPERBYTE+1;
}


// ***** Internal Trie Node class implementation - - - - - - - - - - - - - - -

class trie_node {
public:
	trie_node::trie_node();
	trie_node::trie_node(trie_node &node2);
	trie_node::~trie_node();

	unsigned short  m_nextbit;	// decision bit for next node
	trie_node      *m_parent;	// NULL in trie root only
	trie_node      *m_child0;	// child trie for 0/1 bits respectively
	trie_node      *m_child1;	//   NOTE: if one is set both are set unless
	void           *m_data;		//         m_data is also set

//	void Traverse(void (*nodefunc)(void *data, void *userdata), void *userdata);
	void *Insert(int keydiff, unsigned char *key, int keylen, void *data);
};

trie_node::trie_node()
{
	m_nextbit = 0;
	m_parent  = NULL;
	m_child0  = NULL;
	m_child1  = NULL;
	m_data    = NULL;
}

trie_node::trie_node(trie_node &node2)
{
	m_nextbit = node2.m_nextbit;
	m_parent  = node2.m_parent;
	m_child0  = node2.m_child0;
	m_child1  = node2.m_child1;
	m_data    = node2.m_data;
}

trie_node::~trie_node()
{
	if(m_child0)
		delete(m_child0);
	if(m_child1)
		delete(m_child1);
}

/*
**  trie_node::Insert() -- used by trie_add() to actually add the key to the trie
*/
void *trie_node::Insert(int keydiff, unsigned char *key, int keylen, void *data)
{
	trie_node *cur, *prev, *tmptn;

	cur = this;

	if(keydiff == 0)
		return NULL; // key already exists!

	else if(keydiff < cur->m_nextbit)
	{
		// locate correct insertion point
		for(prev = cur, cur = cur->m_parent;
		  cur != NULL && keydiff < cur->m_nextbit;
		  prev = cur, cur = cur->m_parent)
			;

		// add new decision node
		if((tmptn = new trie_node) == NULL)
			return NULL;

		if(cur == NULL) // insert before top of trie?
		{
			// copy trietop to new node
			*tmptn = *prev;

			// overwrite trietop with new data
			cur = prev;
			cur->m_data    = NULL;
			cur->m_nextbit = keydiff;
			if(bit_check(key, keydiff))
			{
				cur->m_child0 = tmptn;
				cur->m_child1 = NULL;
			}
			else
			{
				cur->m_child0 = NULL;
				cur->m_child1 = tmptn;
			}

			// link parent pointers to/from new node
			tmptn->m_parent = cur;
			if(tmptn->m_child0)
				tmptn->m_child0->m_parent = tmptn;
			if(tmptn->m_child1)
				tmptn->m_child1->m_parent = tmptn;
		}
		else
		{
			// insert node between trie & prev
			if(bit_check(key, keydiff))
				tmptn->m_child0 = prev;
			else
				tmptn->m_child1 = prev;
			prev->m_parent = tmptn;

			if(cur->m_child0 == prev)
				cur->m_child0 = tmptn;
			else
				cur->m_child1 = tmptn;
			tmptn->m_parent = cur;

			cur = tmptn;
			cur->m_data    = NULL;
			cur->m_nextbit = keydiff;
		}
	}

	// need new child node?
	if(keylen*BITSPERBYTE+1 == keydiff)
	{
		cur->m_data = data;
	}
	else
	{
		if((tmptn = new trie_node) == NULL)
		{
			return NULL;
		}
		// set key in new leaf node
		tmptn->m_nextbit = keylen*BITSPERBYTE+1;
		tmptn->m_data    = data;

		if(bit_check(key, keydiff))
			cur->m_child1 = tmptn;
		else
			cur->m_child0 = tmptn;
		tmptn->m_parent = cur;
	}
	return data;
}

// ***** Advertised Functions  - - - - - - - - - - - - - - - - - - - - - - - -

CTrieIterator::CTrieIterator(CTrie &trie)
{
	m_trie = &trie;
	Rewind();
}

void CTrieIterator::Rewind()
{
	m_node = m_trie->m_root;

	// handle null tries
	if(m_node->m_child0 == NULL && m_node->m_child1 == NULL && m_node->m_data == NULL)
	{
		m_node = NULL;
		return;
	}

	// search down to left hand child (1st in order)
	if(m_node)
	{
		while(m_node->m_child0)
			m_node = m_node->m_child0;
	}
}

bool CTrieIterator::operator --(void)
{
	// at end? 
	if(!m_node)
	{
		m_node = m_trie->m_root;

		if(m_node)
		{
			while(m_node->m_child1)
				m_node = m_node->m_child1;
			return true;
		}
		else
			// trie is empty
			return false;
	}

	if(m_node->m_child0)
	{
		// there's more under this subtree, go left and
		// then to the rightmost sub-subtree
		m_node = m_node->m_child0;
		while(m_node->m_child1)
		{
			m_node = m_node->m_child1;
		}
		return true;
	}

	// no more under this subtree, go up until we find something
	trie_node *oldnode;
	while(oldnode = m_node, m_node = m_node->m_parent)
	{
		if(oldnode == m_node->m_child1)
		{
			// data at current level?
			if(m_node->m_data)
				return true;
			// data under left subtree?
			else if(m_node->m_child0)
			{
				// there's more under this subtree, go left and
				// then to the rightmost sub-subtree
				m_node = m_node->m_child0;
				while(m_node->m_child1)
				{
					m_node = m_node->m_child1;
				}
				return true;
			}
		}
		// no, go up again
	}

	return false;
}

bool CTrieIterator::operator ++(void)
{
	// already at end? 
	if(!m_node)
	{
		return false;
	}

	if(m_node->m_child1)
	{
		// there's more under this subtree, go right and
		// then to the leftmost sub-subtree
		m_node = m_node->m_child1;
		while(m_node->m_child0)
		{
			m_node = m_node->m_child0;
		}
		return true;
	}

	// no more under this subtree, go up
	trie_node *oldnode;
	while(oldnode = m_node, m_node = m_node->m_parent)
	{
		if(m_node->m_child0 == oldnode)
		{
			// data at current level?
			if(m_node->m_data)
				return true;
			// data under right subtree?
			else if(m_node->m_child1)
			{
				// there's more under this subtree, go right and
				// then to the leftmost sub-subtree
				m_node = m_node->m_child1;
				while(m_node->m_child0)
				{
					m_node = m_node->m_child0;
				}
				return true;
			}
		}
		// no, go up again
	}

	return false;
}

void *CTrieIterator::GetData(void) const
{
	if(!m_node)
		return NULL;
	return m_node->m_data;
}

bool CTrieIterator::AtBegin(void) const
{
	// scan to find first node
	trie_node *pnode = m_trie->m_root;
	if(!pnode)
		return true;	// REALLY empty trie (should not happen)
	while(pnode->m_child0)
		pnode = pnode->m_child0;

	return pnode == m_node;
}
bool CTrieIterator::AtEnd(void) const
{
	return !m_node;
}


CTrie::CTrie()
{
	m_root = new trie_node;
}

CTrie::~CTrie(void)
{
	delete m_root;
}

/*
**  CTrie::Find() -- perform key lookup in trie
**
**  key     - key to search for
**  keylen  - length of key in bytes
**
**  RETURNS: pointer to user data (NULL if key not in trie)
**
**  PROCESS:
**    set current node to top of trie
**    while current node is not NULL
**        if a key exists at current level
**            get bit difference to current node
**            if no difference
**                return user data at current node
**            if bit difference < node bit number
**                return key not found
**        if keylen < next bit
**                return key not found
**        compare next bit & switch to correct subtree
**    return key not found
*/
void *CTrie::Find(unsigned char *key, int keylen)
{
	unsigned char *curkey;
	int curkeylen;
	int keydiff;
	trie_node *cur;

	cur = this->m_root;
	while(cur != NULL)
	{
		if(cur->m_data != NULL)
		{
			GetKey(cur->m_data, &curkey, &curkeylen);
			keydiff = KeyCmp(key, keylen, curkey, curkeylen);

			if(keydiff == 0)
				return cur->m_data;
			else if(keydiff < cur->m_nextbit)
				return NULL;
		}

		if(keylen < (cur->m_nextbit+BITSPERBYTE-1)/BITSPERBYTE)
			return NULL;

		if(bit_check(key, cur->m_nextbit))
			cur = cur->m_child1;
		else
			cur = cur->m_child0;
	}

	return NULL;
}

/*
**  CTrie::Add() -- add key to trie
**
**  Add a key to the trie.  If the key is already in the trie, the add will
**  fail with a NULL return (it will also do so if a new node could not be
**  created).
**
**  key     - key to add
**  keylen  - length of key in bytes
**  data    - data for key
**
**  RETURNS: pointer to user data (NULL if add failed)
**
**  PROCESS:
**    set current node to top of trie
**    if trie is empty (next bit is 0)
**        set key at current level & return success
**    while current node is not NULL
**        if a key exists at current level
**            get bit difference to current node
**            if no difference
**                return add failure (already exists)
**            if bit difference < node bit number
**                back up trie until difference > bit number
**                insert new node between current, prev node
**                add child node to new node for user key & return success
**        if keylen < next bit
**            scan down link0 until a key is found
**            get bit difference to current node
**            back up trie until difference > bit number
**            insert new node between current, prev node
**            add child node to new node for user key & return success
**        compare next bit & switch to correct subtree
**    add child node to prev node for user key & return success
*/
//void *CTrie::Add(unsigned char *key, int keylen, void *data)
void *CTrie::Add(void *data)
{
	trie_node *prev, *cur;
	unsigned char *curkey;
	int curkeylen;
	int keydiff;

	if((cur = this->m_root) == NULL)
		return NULL;

	unsigned char *key;
	int keylen;
	GetKey(data, &key, &keylen);

	if(cur->m_nextbit == 0) // empty trie?
	{
		cur->m_nextbit = keylen*BITSPERBYTE+1;
		cur->m_data    = data;
		return data;
	}

	prev = NULL;
	while(cur != NULL)
	{
		if(cur->m_data != NULL)
		{
			GetKey(cur->m_data, &curkey, &curkeylen);
			keydiff = KeyCmp(key, keylen, curkey, curkeylen);

			if(keydiff == 0)
				return NULL;
			else if(keydiff < cur->m_nextbit)
				return cur->Insert(keydiff, key, keylen, data);
		}

		if(keylen < (cur->m_nextbit+BITSPERBYTE-1)/BITSPERBYTE)
		{
			for(cur = cur->m_child0; !cur->m_data;
			  cur = cur->m_child0)
				;

			GetKey(cur->m_data, &curkey, &curkeylen);
			keydiff = KeyCmp(key, keylen, curkey, curkeylen);

			return cur->Insert(keydiff, key, keylen, data);
		}

		prev = cur;
		if(bit_check(key, cur->m_nextbit))
			cur = cur->m_child1;
		else
			cur = cur->m_child0;
	}

	GetKey(prev->m_data, &curkey, &curkeylen);
	keydiff = KeyCmp(key, keylen, curkey, curkeylen);

	return prev->Insert(keydiff, key, keylen, data);
}

/*
**  CTrie::Del() -- remove key to trie
**
**  Remove a key from the trie.  If the key is not in the trie, the delete
**  will fail with a NULL return.
**
**  key     - key to remove
**  keylen  - length of key in bytes
**
**  RETURNS: pointer to user data (NULL if del failed)
**
**  PROCESS:
**    set current node to top of trie
**    while current node is not NULL
**        if a key exists at current level
**            get bit difference to current node
**            if no difference
**                point parent node's parent to sibling node
**                free current, parent nodes
**                return success
**        if keylen < next bit
**                return key not found
**        compare next bit & switch to correct subtree
**    return key not found
*/
void *CTrie::Del(void *data)
{
	unsigned char *key;
	int keylen;

	GetKey(data, &key, &keylen);
	return Del(key, keylen);
}
void *CTrie::Del(unsigned char *key, int keylen)
{
	trie_node *prev, *child, *cur;
	unsigned char *curkey;
	int curkeylen;
	int keydiff;
	void *data;

	cur = this->m_root;

	while(cur != NULL)
	{
		if(cur->m_data != NULL)
		{
			GetKey(cur->m_data, &curkey, &curkeylen);
			keydiff = KeyCmp(key, keylen, curkey, curkeylen);

			if(keydiff == 0)
			{
				data = cur->m_data; // save for return
				cur->m_data = NULL;

				while(cur->m_data == NULL)
				{
					// fully populated interior node?
					if(cur->m_child0 && cur->m_child1)
						break;

					// 0 or 1 m_childX entries are !=0
					prev = cur->m_parent;
					if(cur->m_child0)
						child = cur->m_child0;
					else
						child = cur->m_child1;

					// top-level node?
					if(!prev)
					{
						// empty trie?
						if(!child)
						{
							cur->m_nextbit = 0;
							cur->m_child0  = NULL;
							cur->m_child1  = NULL;
							break;
						}

						// copy child to current
						*cur = *child;
						cur->m_parent = NULL;
						// point children to trie
						if(cur->m_child0)
							cur->m_child0->m_parent = cur;
						if(cur->m_child1)
							cur->m_child1->m_parent = cur;
						free(child);
						break;
					}

					// sparsely populated interior node
					if(prev->m_child0 == cur)
						prev->m_child0 = child;
					else
						prev->m_child1 = child;
					if(child)
						child->m_parent = prev;
					free(cur);
					cur = prev;
				}

				return data;
			}

			if(keydiff < cur->m_nextbit)
				return NULL;
		}

		if(keylen < (cur->m_nextbit+BITSPERBYTE-1)/BITSPERBYTE)
			return NULL;

		if(bit_check(key, cur->m_nextbit))
			cur = cur->m_child1;
		else
			cur = cur->m_child0;
	}

	return NULL;
}
