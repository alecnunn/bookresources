/************************************************************
 * stack.h - Header file de clase stack V0.01.006
 * 
 * (c) Copyright 1996-2000 Sabarasa Entertainment
 * For internal use only.
 ************************************************************/
  
#ifndef _STACK_H_
#define _STACK_H_

// Clase Stack
class c_stack
{
private:
	void *start;
	int elemsize;

public:
	c_stack( int elemsize );
	~c_stack();

	void Push( void *element );
	void Pop( void *element );

	int StackIsEmpty();
	int StackDepth();
	void GetStackElement( int index, void *element );
};

#endif