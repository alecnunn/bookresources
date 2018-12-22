
/******************************************************************************

Coral Tree gpl/hash.cc
Copyright (C) 1998 Infinite Monkeys Incorporated

This program is free software; you can redistribute it and/or modify
it under the terms of the Artistic License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Artistic License for more details.

You should have received a copy of the Artistic License along with
this program (see meta/ART_LICE); if not, write to coral@imonk.com.

******************************************************************************/

#include "gpl.h"

#undef GPL_CORE_HASH_CLASSNAME
#undef GPL_HASH_CLASSNAME
#undef GPL_HASH_LISTTYPE
#undef GPL_HASH_NODETYPE
#undef GPL_HASH_COMPTYPE
#undef GPL_HASH_ITERATOR
#define GPL_CORE_HASH_CLASSNAME     GPL_Core_IHashTable
#define GPL_HASH_CLASSNAME          GPL_IHashTable
#define GPL_HASH_LISTTYPE           GPL_IDoubleList
#define GPL_HASH_NODETYPE           GPL_DoubleNode
#define GPL_HASH_COMPTYPE           void
#define GPL_HASH_ITERATOR           GPL_IHashTableIterator

#include "hash2.cc"

#undef GPL_CORE_HASH_CLASSNAME
#undef GPL_HASH_CLASSNAME
#undef GPL_HASH_LISTTYPE
#undef GPL_HASH_NODETYPE
#undef GPL_HASH_COMPTYPE
#undef GPL_HASH_ITERATOR
#define GPL_CORE_HASH_CLASSNAME     GPL_Core_HashTable 
#define GPL_HASH_CLASSNAME          GPL_HashTable 
#define GPL_HASH_LISTTYPE           GPL_DoubleList 
#define GPL_HASH_NODETYPE           void
#define GPL_HASH_COMPTYPE           void
#define GPL_HASH_ITERATOR           GPL_HashTableIterator

#include "hash2.cc"
