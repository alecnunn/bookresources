
/******************************************************************************

Coral Tree gpl/xgpl.cc
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

#define LINKED_LIST_LEN		20

#define CQ_LEN				10
#define CQ_BLOCK_SIZE		3

#define DARRAY_LEN			10
#define DARRAY_BLOCK_SIZE	3

#define HASH_LEN			10
#define HASH_SIZE			3


class xcountnode
{
	public:
		long id;
		static long next_id;

		xcountnode()
		{
			id = next_id;
			next_id++;
		}
};
long xcountnode::next_id = 0;

class xlinkednode : public GPL_DoubleNode
{
	public:
		long id;
		static long next_id;

		xlinkednode()
		{
			id = next_id;
			next_id++;
		}
};
long xlinkednode::next_id = 0;

class xihashnode : public GPL_DoubleNode
{
	public:
		long id;
		static long next_id;

		xihashnode()
		{
			id = next_id;
			next_id++;
		}
};
long xihashnode::next_id = 0;

class xhashnode
{
	public:
		long id;
		static long next_id;

		xhashnode()
		{
			id = next_id;
			next_id++;
		}
};
long xhashnode::next_id = 0;

class xcqnode
{
	public:
		long id;
		static long next_id;

		xcqnode()
		{
			id = next_id;
			next_id++;
		}
};
long xcqnode::next_id = 0;

class xdarraynode
{
	public:
		long id;
		static long next_id;

		xdarraynode()
		{
			id = next_id;
			next_id++;
		}
};
long xdarraynode::next_id = 0;

class xihashtable : public GPL_IHashTable<xihashnode, long>
{
	public:
		xihashtable(long a_size) : GPL_IHashTable<xihashnode, long>(a_size) {}
		long HashFunction(void *element);
		long Compare(GPL_DoubleNode *node, void *element);
};

long xihashtable::HashFunction(void *element)
{
	return (*(long *)element)%GetSize();
}

long xihashtable::Compare(GPL_DoubleNode *node, void *element)
{
	if(((xihashnode *)node)->id == *(long *)element) return 0;
	else return 1;
}

class xhashtable : public GPL_HashTable<xhashnode, long>
{
	public:
		xhashtable(long a_size) : GPL_HashTable<xhashnode, long>(a_size) {}
		long HashFunction(void *element);
		long Compare(void *node, void *element);
};

long xhashtable::HashFunction(void *element)
{
	return (*(long *)element)%GetSize();
}

long xhashtable::Compare(void *node, void *element)
{
	if(((xhashnode *)node)->id == *(long *)element) return 0;
	else return 1;
}

class tarray_entry
{
	public:
		static long count;
		long id;
		tarray_entry()
		{
			id = count++;
		}
};

long tarray_entry::count = 0;

/* TODO fix GPL_Array */
#if 0
void test_tarray()
{
	GPL_Array<tarray_entry>	tarray;
	long i,j;
	char string[1000];

	for(i = 0;i < 10; i++)
	{
		j = tarray.Make();
		sprintf(string,"made index %d",j);
		gplPrint(GPL_USER,GPL_VLOG,string);
	}

	for(i = 0;i < 10; i++)
	{
		sprintf(string,"index %d id %d",i,tarray.Base()[i].id);
		gplPrint(GPL_USER,GPL_VLOG,string);
	}
}
#endif

void test_idlist()
{
	xlinkednode						*linked_node,*prev_node,*next_node;
	GPL_IDoubleList<xlinkednode>	linked_list;
	long i;
	char string[1000];

	gplPrint(GPL_USER,GPL_VLOG,"Intrusive Linked List Test");

	/* BUILD LIST */
	gplPrint(GPL_USER,GPL_VLOG,"Build List");
	for(i = LINKED_LIST_LEN; i > 0; i--)
	{
		linked_node = new xlinkednode;
		linked_list.AppendNode(linked_node);
	}

	/* PRINT LIST */
	gplPrint(GPL_USER,GPL_VLOG,"Print List");
	strcpy(string,"List: ");
	linked_list.Rewind();
	while(linked_node = linked_list.Next())
	{
		sprintf((char *)(&string[strlen(string)]), ".%d", linked_node->id);
	}
	gplPrint(GPL_USER,GPL_VLOG,string);

	/* REMOVE EVERY OTHER NODE */
	gplPrint(GPL_USER,GPL_VLOG,"Remove every other node");
	long toggle = FALSE;
	char prev_id[20],next_id[20];
	linked_list.Rewind();
	while(linked_node = linked_list.Next())
	{
		if( prev_node=(xlinkednode *)linked_node->Prev() )
			sprintf(prev_id,"%d",prev_node->id);
		else
			strcpy(prev_id,"NULL");

		if( next_node=(xlinkednode *)linked_node->Next() )
			sprintf(next_id,"%d",next_node->id);
		else
			strcpy(next_id,"NULL");

		sprintf(string, "Node %d prev=%s next=%s",
										linked_node->id, prev_id, next_id);

		gplPrint(GPL_USER,GPL_VLOG,string);

		if(toggle=!toggle)
			{
			gplPrint(GPL_USER,GPL_VLOG,"Remove and Delete");
			linked_list.RemoveNode(linked_node);
			delete linked_node;
			}
	}

	/* PRINT LIST */
	gplPrint(GPL_USER,GPL_VLOG,"Print List");
	strcpy(string,"List: ");
	linked_list.Rewind();
	while(linked_node = linked_list.Next())
	{
		sprintf((char *)(&string[strlen(string)]), ".%d", linked_node->id);
	}
	gplPrint(GPL_USER,GPL_VLOG,string);


	gplPrint(GPL_USER,GPL_VLOG,"Intrusive Linked List Test Done");
}

void test_dlist()
{
	xcountnode						*linked_node;
	GPL_DoubleList<xcountnode>		linked_list;
	GPL_DoubleList<xcountnode>		linked_list2;
	long i;
	char string[1000];

	gplPrint(GPL_USER,GPL_VLOG,"NON Intrusive Linked List Test");

	/* BUILD LISTS */
	for(i = LINKED_LIST_LEN; i > 0; i--)
	{
		linked_node = new xcountnode;
		linked_list.AppendNode(linked_node);
	}
	for(i = LINKED_LIST_LEN; i > 0; i--)
	{
		linked_node = new xcountnode;
		linked_list2.AppendNode(linked_node);
	}

	/* PRINT LIST */
	gplPrint(GPL_USER,GPL_VLOG,"using iterator to print half");
	GPL_DoubleListIterator<xcountnode> *iterator = new GPL_DoubleListIterator<xcountnode>(&linked_list);
	string[0] = 0;
	iterator->ToHead();
	for(i = LINKED_LIST_LEN/2; i > 0; i--)
	{
		linked_node = iterator->PostIncrement();
		sprintf((char *)(&string[strlen(string)]), ".%d", linked_node->id);
	}
	gplPrint(GPL_USER,GPL_VLOG,string);

	gplPrint(GPL_USER,GPL_VLOG,"using list to print");
	string[0] = 0;
	linked_list.Rewind();
	while(linked_node = linked_list.Next())
	{
		sprintf((char *)(&string[strlen(string)]), ".%d", linked_node->id);
	}
	gplPrint(GPL_USER,GPL_VLOG,string);

	gplPrint(GPL_USER,GPL_VLOG,"using iterator to print remainder");
	string[0] = 0;
	while(linked_node = iterator->PostIncrement())
	{
		sprintf((char *)(&string[strlen(string)]), ".%d", linked_node->id);
	}
	gplPrint(GPL_USER,GPL_VLOG,string);

	/* REMOVE EVERY OTHER NODE */
	long toggle = FALSE;
	linked_list.Rewind();
	while(linked_node = linked_list.Next())
	{
		if(toggle)
		{
			toggle=FALSE;
		}
		else
		{
			toggle=TRUE;
			linked_list.RemoveNode(linked_node);
			delete linked_node;
		}
	}

	/* PRINT LIST */
	string[0] = 0;
	linked_list.Rewind();
	while(linked_node = linked_list.Next())
	{
		sprintf((char *)(&string[strlen(string)]), ".%d", linked_node->id);
	}
	gplPrint(GPL_USER,GPL_VLOG,string);

	/* INSERT LIST 2, 2 positions in */
	linked_list.ToHead();
	linked_list.PostIncrement();
	linked_list.PostIncrement();
	linked_list.InsertBefore(&linked_list2);

	/* PRINT LIST */
	gplPrint(GPL_USER,GPL_VLOG,"Linked List after insertion of another list");
	string[0] = 0;
	linked_list.Rewind();
	while(linked_node = linked_list.Next())
	{
		sprintf((char *)(&string[strlen(string)]), ".%d", linked_node->id);
	}
	gplPrint(GPL_USER,GPL_VLOG,string);
}

void print_cq_state(GPL_CircularQueue<xcqnode> *cq)
{
#ifdef GPL_PROBE_CIRCULAR_QUEUE
	long 						i;
	char 						h,t;
	char 						string[1000];

	string[0] = 0;
	if(cq->SizeAllocated)
	{
		sprintf(string,"[%3d]",cq->SizeAllocated);
		for(i=0;i<cq->SizeAllocated;i++)
		{
			if(i == cq->Head) h = 'h';else h = ' ';
			if(i == cq->Tail) t = 't';else t = ' ';
			if(cq->Array)
			{
				if(cq->Array[i])
				{
					sprintf((char *)(&string[strlen(string)]), ".%2d%c%c", ((xcqnode *)cq->Array[i])->id,h,t);
				}
				else
				{
					sprintf((char *)(&string[strlen(string)]), ".NL%c%c",h,t);
				}
			}
			else
			{
				gplPrint(GPL_SOFTWARE,GPL_ERROR,"no space allocated but SizeAllocated is not zero");
			}
		}
		gplPrint(GPL_USER,GPL_VLOG,string);
	}
	else
	{
		gplPrint(GPL_USER,GPL_VLOG,"no space allocated");
	}
#endif
}

void print_darray_state(GPL_DynamicArray<xdarraynode> *darray)
{
#ifdef GPL_PROBE_DYNAMIC_ARRAY
	long 						i;
	char 						string[1000];

	string[0] = 0;
	if(darray->SizeAllocated)
	{
		sprintf(string,"[%3d]",darray->SizeAllocated);
		for(i=0;i<darray->SizeAllocated;i++)
		{
			if(darray->Array)
			{
				if(darray->Array[i])
				{
					sprintf((char *)(&string[strlen(string)]), ".%2d", ((xdarraynode *)darray->Array[i])->id);
				}
				else
				{
					sprintf((char *)(&string[strlen(string)]), ".NL");
				}
			}
			else
			{
				gplPrint(GPL_SOFTWARE,GPL_ERROR,"no space allocated but SizeAllocated is not zero");
			}
		}
		gplPrint(GPL_USER,GPL_VLOG,string);
	}
	else
	{
		gplPrint(GPL_USER,GPL_VLOG,"no space allocated");
	}
#endif
}

void print_ihash_state(xihashtable *hash_table)
{
	long 						i;
	char 						string[1000];
	xihashnode					*linked_node;

	if(hash_table->GetCount())
	{
		sprintf(string,"[%3d] HASH TABLE",hash_table->GetCount());
		gplPrint(GPL_USER,GPL_VLOG,string);
		for(i=0;i<hash_table->GetSize();i++)
		{
			if(hash_table->GetArray())
			{
				if((hash_table->GetArray())[i])
				{
					/* PRINT LIST */
					string[0] = 0;
					(hash_table->GetArray())[i]->Rewind();
					while(linked_node = (xihashnode *)((hash_table->GetArray())[i]->Next()))
					{
						sprintf((char *)(&string[strlen(string)]), ".%d", linked_node->id);
					}
					gplPrint(GPL_USER,GPL_VLOG,string);
				}
				else
				{
					gplPrint(GPL_SOFTWARE,GPL_ERROR,"List not allocated");
				}
			}
			else
			{
				gplPrint(GPL_SOFTWARE,GPL_ERROR,"Array not allocated");
			}
		}
		gplPrint(GPL_USER,GPL_VLOG,"MEMBER LIST");
		string[0] = 0;
		GPL_DoubleList<void> *member_list;
		if(member_list = hash_table->MemberList())
		{
			member_list->Rewind();
			while(linked_node = (xihashnode *)member_list->PostIncrement())
			{
				sprintf((char *)(&string[strlen(string)]), ".%d", linked_node->id);
			}
			gplPrint(GPL_USER,GPL_VLOG,string);
			delete member_list;
		}
	}
	else
	{
		gplPrint(GPL_USER,GPL_VLOG,"no elements in hash table");
	}
}

void print_hash_state(xhashtable *hash_table)
{
	long 						i;
	char 						string[1000];
	xhashnode					*linked_node;

	if(hash_table->GetCount())
	{
		sprintf(string,"[%3d] HASH TABLE",hash_table->GetCount());
		gplPrint(GPL_USER,GPL_VLOG,string);
		for(i=0;i<hash_table->GetSize();i++)
		{
			if(hash_table->GetArray())
			{
				if((hash_table->GetArray())[i])
				{
					/* PRINT LIST */
					string[0] = 0;
					(hash_table->GetArray())[i]->Rewind();
					while(linked_node = (xhashnode *)((hash_table->GetArray())[i]->Next()))
					{
						sprintf((char *)(&string[strlen(string)]), ".%d", linked_node->id);
					}
					gplPrint(GPL_USER,GPL_VLOG,string);
				}
				else
				{
					gplPrint(GPL_SOFTWARE,GPL_ERROR,"List not allocated");
				}
			}
			else
			{
				gplPrint(GPL_SOFTWARE,GPL_ERROR,"Array not allocated");
			}
		}
		gplPrint(GPL_USER,GPL_VLOG,"MEMBER LIST");
		string[0] = 0;
		GPL_DoubleList<void> *member_list;
		if(member_list = hash_table->MemberList())
		{
			member_list->Rewind();
			while(linked_node = (xhashnode *)member_list->PostIncrement())
			{
				sprintf((char *)(&string[strlen(string)]), ".%d", linked_node->id);
			}
			gplPrint(GPL_USER,GPL_VLOG,string);
			delete member_list;
		}
	}
	else
	{
		gplPrint(GPL_USER,GPL_VLOG,"no elements in hash table");
	}
}

void test_cqueue()
{
	GPL_CircularQueue<xcqnode>	cq(CQ_BLOCK_SIZE);
	xcqnode						*cq_node;
	long 						i;

	gplPrint(GPL_USER,GPL_VLOG,"circular queue test");

	print_cq_state(&cq);

	/* ADD NODES TO TAIL */
	gplPrint(GPL_USER,GPL_VLOG,"add nodes to tail");
	for(i=0;i<CQ_LEN;i++)
	{
		cq_node = new xcqnode;
		cq.AddTail(cq_node);
		print_cq_state(&cq);
	}

	/* REMOVE NODES FROM TAIL AND ADD TO HEAD */
	gplPrint(GPL_USER,GPL_VLOG,"move nodes from tail to head");
	for(i=0;i<CQ_LEN;i++)
	{
		cq_node = cq.RemoveTail();
		cq.AddHead(cq_node);
		print_cq_state(&cq);
	}

	/* REMOVE NODES FROM HEAD */
	gplPrint(GPL_USER,GPL_VLOG,"remove nodes from head");
	for(i=0;i<CQ_LEN;i++)
	{
		cq_node = cq.RemoveHead();
		delete cq_node;
		print_cq_state(&cq);
	}
	
	/* ADD NODES TO HEAD */
	gplPrint(GPL_USER,GPL_VLOG,"add nodes to head");
	for(i=0;i<CQ_LEN;i++)
	{
		cq_node = new xcqnode;
		cq.AddHead(cq_node);
		print_cq_state(&cq);
	}

	/* REMOVE NODES FROM TAIL */
	gplPrint(GPL_USER,GPL_VLOG,"remove nodes from tail");
	for(i=0;i<CQ_LEN;i++)
	{
		cq_node = cq.RemoveTail();
		delete cq_node;
		print_cq_state(&cq);
	}
}

void test_darray()
{
	GPL_DynamicArray<xdarraynode>		darray(DARRAY_BLOCK_SIZE);
	xdarraynode							*darray_node;
	long								i;
	long								index;
	char								string[100];

	gplPrint(GPL_USER,GPL_VLOG,"test dynamic array");

	/* ADD DARRAY_LEN NODES */
	gplPrint(GPL_USER,GPL_VLOG,"add nodes");
	for(i=0;i < DARRAY_LEN; i++)
	{
		sprintf(string,"add %d", i);
		gplPrint(GPL_USER,GPL_VLOG,string);
		darray_node = new xdarraynode;
		darray.Add(&index, darray_node);
		print_darray_state(&darray);
	}

	/* REMOVE EVERY OTHER NODE */
	gplPrint(GPL_USER,GPL_VLOG,"remove every other node");
	for(i=0;i < DARRAY_LEN; i+=2)
	{
		sprintf(string,"remove %d", i);
		gplPrint(GPL_USER,GPL_VLOG,string);
		darray_node = darray.Remove(i);
		delete darray_node;
		print_darray_state(&darray);
	}

	/* ADD HALF DARRAY_LEN BACK IN */
	gplPrint(GPL_USER,GPL_VLOG,"add more nodes in");
	for(i=0;i < DARRAY_LEN; i+=2)
	{
		sprintf(string,"add %d", i);
		gplPrint(GPL_USER,GPL_VLOG,string);
		darray_node = new xdarraynode;
		darray.Add(&index, darray_node);
		print_darray_state(&darray);
	}
	
	/* REMOVE ALL NODES */
	gplPrint(GPL_USER,GPL_VLOG,"remove nodes");
	for(i=DARRAY_LEN-1;i >= 0; i--)
	{
		sprintf(string,"remove %d", i);
		gplPrint(GPL_USER,GPL_VLOG,string);
		darray_node = darray.Remove(i);
		delete darray_node;
		print_darray_state(&darray);
	}

}

void test_ihash()
{
	xihashnode							*hash_node;
	xihashtable							hash_table((long)HASH_SIZE);
	long								i;

	gplPrint(GPL_USER,GPL_VLOG,"intrusive hash table");
	for(i=0;i < HASH_LEN; i++)
	{
		hash_node = new xihashnode;
		hash_table.Insert(hash_node,&hash_node->id);
		print_ihash_state(&hash_table);
	}
	for(i=0;i < HASH_LEN; i++)
	{
		hash_node = hash_table.LookUp(&i);
		if(hash_node->id != i)
		{
			gplPrint(GPL_SYSTEM,GPL_ERROR,"looked up node not correct");
		}
	}
	gplPrint(GPL_USER,GPL_VLOG,"lookup test done");
}

void test_hash()
{
	xhashnode							*hash_node;
	xhashtable							hash_table((long)HASH_SIZE);
	long								i;

	gplPrint(GPL_USER,GPL_VLOG,"non intrusive hash table");
	for(i=0;i < HASH_LEN; i++)
	{
		hash_node = new xhashnode;
		hash_table.Insert(hash_node,&hash_node->id);
		print_hash_state(&hash_table);
	}
	for(i=0;i < HASH_LEN; i++)
	{
		hash_node = hash_table.LookUp(&i);
		if(hash_node->id != i)
		{
			gplPrint(GPL_SYSTEM,GPL_ERROR,"looked up node not correct");
		}
	}
	gplPrint(GPL_USER,GPL_VLOG,"lookup test done");
}

void child_function(void *arg)
{
	GPL_LocalLock *lock;
	
	lock = (GPL_LocalLock *)arg;

	lock->StartRead();
	gplPrint(GPL_USER,GPL_VLOG,"start child reading");
	gplPrint(GPL_USER,GPL_VLOG,"stop child reading");
	lock->StopRead();
	lock->StartWrite();
	gplPrint(GPL_USER,GPL_VLOG,"start child writing");
	osdSleep(4);
	gplPrint(GPL_USER,GPL_VLOG,"stop child writing");
	lock->StopWrite();
}

void test_rw_lock()
{
#if OSD_THREADS!=OSD_NO_THREADS
	GPL_LocalLock	lock;
	lock.StartRead();
	gplPrint(GPL_USER,GPL_VLOG,"start parent reading");
	osdThreadSpawn(child_function,(void *)&lock);
	osdSleep(3);
	gplPrint(GPL_USER,GPL_VLOG,"stop parent reading");
	lock.StopRead();
	osdSleep(1);
	lock.StartWrite();
	gplPrint(GPL_USER,GPL_VLOG,"start parent writing");
	osdSleep(1);
	gplPrint(GPL_USER,GPL_VLOG,"stop parent writing");
	lock.StopWrite();
#endif
}

GPL_Boolean test_set_function(const GPL_Pair<int, int> & b,
								const GPL_Pair<int, int> & c)
{
	return b.first == c.first;
}

void test_set(void)
{
	GPL_Set< GPL_Pair<int, int> > set1;
	int i;
	GPL_Pair<int, int> pair(0, 0);
	set1.Insert(pair);
	pair.first = 1;
	pair.second = 1;
	set1.Insert(pair);
	for (i = 0; set1.GetCount() > i; ++i)
	{
		fprintf(stderr, "%d: %d,%d\n", i, set1[i].first, set1[i].second);
	}
	pair.first = 1;
	pair.second = 2;
	set1.Insert(pair);
	for (i = 0; set1.GetCount() > i; ++i)
	{
		fprintf(stderr, "%d: %d,%d\n", i, set1[i].first, set1[i].second);
	}
}

int main()
{
#if 0
	GPL_Block chunk;
	GPL_Boolean r_v;
	chunk.Set("ABCDEF0GHIJ0KHGJHG", 18);
	unsigned char chptr[100]; unsigned long chlen;
	chlen = 5;
	r_v = chunk.CopyNext(chptr, &chlen);
	chptr[chlen] = 0;
	fprintf(stderr, "[%s] %d %d\n", chptr, chlen, r_v);
	chlen = 5;
	r_v = chunk.CopyNext(chptr, &chlen);
	chptr[chlen] = 0;
	fprintf(stderr, "[%s] %d %d\n", chptr, chlen, r_v);

	chunk.Rewind();
	chlen = 100;
	r_v = chunk.CopyTo('0', chptr, &chlen);
	chptr[chlen] = 0;
	fprintf(stderr, "[%s] %d %d\n", chptr, chlen, r_v);
	chlen = 100;
	r_v = chunk.CopyTo('0', chptr, &chlen);
	chptr[chlen] = 0;
	fprintf(stderr, "[%s] %d %d\n", chptr, chlen, r_v);
	chlen = 100;
	r_v = chunk.CopyTo('0', chptr, &chlen);
	chptr[chlen] = 0;
	fprintf(stderr, "[%s] %d %d\n", chptr, chlen, r_v);
	chlen = 100;
	r_v = chunk.CopyTo('0', chptr, &chlen);
	chptr[chlen] = 0;
	fprintf(stderr, "[%s] %d %d\n", chptr, chlen, r_v);


	osdMalloc(1000);
	GPL_String q;
	q = (const char *)"TEMP";
	q.Append((const char *)" LOG");
	fprintf(stderr,"[%s]\n", (const char *)q);
#endif

	/* GPL_File */
	GPL_File testfile;
	if(!testfile.Open("GPL_Message.yo", OSD_FILE_READ))
	{
		gplLog("GPL ERROR", "could not open file");
	}
	char *filebuf;
	unsigned long filelen;
	long loadtime;
	GPL_CharPtr fileptr("dummy");

	loadtime = osdTimer();
	testfile.Read(fileptr);
	fprintf(stderr,"GPL_CharPtr took %d\n", osdTimer() - loadtime);

	testfile.SetPointer(0, OSD_FILE_BEGIN);
	loadtime = osdTimer();
	filebuf = testfile.Read(&filelen, 1024);
	fprintf(stderr,"1024 took %d\n", osdTimer() - loadtime);
	osdFree(filebuf);

	testfile.SetPointer(0, OSD_FILE_BEGIN);
	loadtime = osdTimer();
	filebuf = testfile.Read(&filelen, 2048);
	fprintf(stderr,"2048 took %d\n", osdTimer() - loadtime);
	osdFree(filebuf);

	testfile.SetPointer(0, OSD_FILE_BEGIN);
	loadtime = osdTimer();
	filebuf = testfile.Read(&filelen, 4096);
	fprintf(stderr,"4096 took %d\n", osdTimer() - loadtime);
	osdFree(filebuf);

	testfile.SetPointer(0, OSD_FILE_BEGIN);
	loadtime = osdTimer();
	filebuf = testfile.Read(&filelen, 65536);
	fprintf(stderr,"65536 took %d\n", osdTimer() - loadtime);
	osdFree(filebuf);

	testfile.SetPointer(0, OSD_FILE_BEGIN);
	loadtime = osdTimer();
	filebuf = testfile.Read(&filelen);
	fprintf(stderr,"default took %d\n", osdTimer() - loadtime);
	osdFree(filebuf);


	testfile.Close();

#define BIG 1000000
	char blob[BIG];
	GPL_Message big1(0);
	big1.AppendBlock(blob, (long)BIG);
	long blittime;
	blittime = osdTimer();
	big1.InsertLong(1, 0);
	fprintf(stderr,"time for no fore %d\n", osdTimer() - blittime);

	GPL_Message big2(4);
	big2.AppendLong(2);
	big2.AppendBlock(blob, (long)BIG);
	blittime = osdTimer();
	big2.InsertLong(1, 0);
	fprintf(stderr,"time for fore %d\n", osdTimer() - blittime);

	GPL_Message forset(4);
	forset.AppendLong(1);
	forset.AppendLong(2);
	forset.AppendLong(3);
	forset.InsertLong(4,0);
	forset.Rewind();
	fprintf(stderr,"forset %d\n", forset.ReadLong());
	fprintf(stderr,"forset %d\n", forset.ReadLong());
	fprintf(stderr,"forset %d\n", forset.ReadLong());
	fprintf(stderr,"forset %d\n", forset.ReadLong());
	forset.Rewind();
	forset.ReadLong();
	forset.ReadLong();
	forset.ResetHome();
	forset.Rewind();
	fprintf(stderr,"forset %d\n", forset.ReadLong());
	fprintf(stderr,"forset %d\n", forset.ReadLong());


	/* CharPtr */
	GPL_CharPtr cptr1;
	GPL_CharPtr *cptr;
	cptr = new GPL_CharPtr();
	*cptr = "hello";
	cptr1 = "there";
	*cptr = cptr1;

	/* MESSAGE */
	GPL_Message *message;
	message = new GPL_Message();
	message->AppendCharPtr(*cptr);

	/* VALUE */
	GPL_Value value(*message);
	GPL_Message m;
	m = value.GetMessage();
	value.SetReal(1.1);
	m.Rewind();
	cptr1 = m.ReadCharPtr();
	fprintf(stderr,"%s\n", (const char *)cptr1);
	delete cptr;
	delete message;

	GPL_Tuple tuple;
	GPL_Message m1,m2,msg;
	m1 = m;
	m2 = m;
	m1.AppendChar('A');
	m2.AppendChar('B');
	tuple.Set("IMI", 3, &m1, 4);
	tuple.Get("iMi", &msg);
	fprintf(stderr,"%d\n", msg.Length());
	msg.Rewind();
	cptr1 = msg.ReadCharPtr();
	fprintf(stderr,"%s\n", (const char *)cptr1);

	fprintf(stderr,"%d\n", tuple.CheckMatch("IMI", 3, &m1, 4));
	fprintf(stderr,"%d\n", tuple.CheckMatch("IMI", 3, &m1, 2));
	msg.AppendChar('X');
	fprintf(stderr,"%d\n", tuple.CheckMatch("IMI", 3, &m2, 4));

#if 0
	/* TRUE ARRAY */
	test_tarray();

	/* INTRUSIVE DOUBLY LINKED LIST */
	test_idlist();

	/* UNINTRUSIVE DOUBLY LINKED LIST */
	test_dlist();

	/* CIRCULAR QUEUE */
	test_cqueue();

	/* DYNAMIC ARRAY */
	test_darray();

	/* INTRUSIVE HASH TABLE */
	test_ihash();

	/* NON INTRUSIVE HASH TABLE */
	test_hash();

	/* LOCAL R/W LOCK */
	test_rw_lock();

	/* SET */
	test_set();
#endif

	return 0;
}
