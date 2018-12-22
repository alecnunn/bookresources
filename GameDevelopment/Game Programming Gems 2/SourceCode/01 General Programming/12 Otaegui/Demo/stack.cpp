/************************************************************
 * Stack.cpp - Stack Class V0.01.006
 *
 * (c) Copyright 2001 Sabarasa Entertainment
 * For internal use only.
 ************************************************************/               

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "mymalloc.h"
#include "fplog.h"
#include "stack.h"

typedef struct cellT
{
	void *element;
	struct cellT *link;
} cellT;

c_stack::c_stack( int elemsize )
{
	this->start = NULL;
	this->elemsize = elemsize;
}

c_stack::~c_stack()
{
	cellT *cp, *next;

	cp = (cellT *) this->start;
	while( cp !=NULL )
	{
		next = cp->link;

		myfree( cp->element, "Stack->Data" );
		myfree( cp, "Stack->Cell" );

		cp=next;
	}
}

void
c_stack::Push( void *element )
{
	cellT *cp;

	cp = (cellT *) mymalloc( sizeof(cellT), "Stack->NextCell" );
	cp->element = (char *) mymalloc( elemsize, "Stack->Data" );

	// Copio el elemento en el nuevo espacio
	memcpy( cp->element, element, elemsize );

	cp->link = (cellT *) this->start;
	this->start = (void *) cp;
}

void
c_stack::Pop( void *element )
{
	cellT *cp;

	if( StackIsEmpty() )
	{
		fplog( "STACK IS EMPTY!\n" );
		exit( 1 );
	}

	cp = (cellT *) this->start;

	memcpy( element, cp->element, elemsize );
	this->start = cp->link;

	myfree( cp->element, "Stack->Data" );
	myfree( cp, "Stack->NextCell" );
}

int
c_stack::StackIsEmpty()
{
	return ((cellT *)this->start==NULL);
}

int
c_stack::StackDepth()
{
	int n;
	cellT *cp;

	n=0;
	for( cp = (cellT *)this->start; cp!=NULL; cp = cp->link )
		n++;

	return n;
}

void
c_stack::GetStackElement( int index, void *element )
{
	int i;
	cellT *cp;

	if( index<0 || index>StackDepth() )
	{
		fplog( "STACK: INDEX OUT OF BOUNDARIES\n" );
		exit( 1 );
	}

	cp = (cellT *) this->start;
	for( i=0; i<index; i++ )
			cp = cp->link;

	memcpy( element, cp->element, elemsize );
}
