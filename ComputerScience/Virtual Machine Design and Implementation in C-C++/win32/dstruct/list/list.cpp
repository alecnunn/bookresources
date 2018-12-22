#include<stdio.h>
#include<stdlib.h>

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ declarations                                                      +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

class List
{
	int LIST_INIT;
	int LIST_INC;

	public:
	int *ptr;	/*pointer to start of list*/
	int nCells;	/*current capacity*/
	int iNext;	/*next free space*/

	List(int init,int inc);
	~List();
	void addToList(int val);
	void printList();	
};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ definitions                                                       +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

List::List(int init,int inc)
{
	LIST_INIT=init;
	LIST_INC=inc;
	ptr = (int*)malloc(LIST_INIT*sizeof(int));
	if(ptr==NULL)
	{
		printf("List(): cannot allocate memory\n");
		exit(1);
	}
	iNext = 0;
	nCells = LIST_INIT;
	return;

}/*end constructor*/

/*-----------------------------------------------------------------*/

List::~List()
{ 
	free(ptr); 
	return;
	
}/*end destructor*/

/*-----------------------------------------------------------------*/

void List::addToList(int val)
{
	int i;
	int *temp_ptr;

	/* 
	if nCell=n will have valid indices 0,...,n-1
	thus, if index=n we are out of bounds
	*/

	if(iNext >= nCells)
	{
		temp_ptr = (int*)malloc((nCells+LIST_INC)*sizeof(int));
		if(temp_ptr==NULL)
		{
			printf("List.addToList(): cannot allocate more memory\n");
			exit(1);
		}
		else
		{
			printf("List.addToList(): not enough room for %d\n",val);
			printf("List.addToList(): allocating %d more cells\n",LIST_INC);
			for(i=0;i<nCells;i++){ temp_ptr[i] = ptr[i]; }
			free(ptr);
			ptr = temp_ptr;
			nCells = nCells+LIST_INC;
		}
	}
	
	ptr[iNext]=val;
	iNext++; 
	return;

}/*end addToList*/

/*-----------------------------------------------------------------*/

void List::printList()
{
	int i;

	printf("list capacity =%d\n",nCells); 
	printf("next index    =%d\n",iNext);
	
	for(i=0;i<iNext;i++)
	{
		printf("%d) %d\n",i,ptr[i]);
	}
	return;

}/*printList*/

/*-----------------------------------------------------------------*/

void main()
{

	List list(4,4);
	
	list.addToList(4); 
	list.addToList(-5); 
	list.addToList(1); 
	list.addToList(11); 
	
	list.addToList(7);
	list.addToList(8);  
	list.addToList(-12); 
	list.addToList(122);
	
	list.addToList(4);
	list.addToList(5);  
	list.addToList(5);   
	list.addToList(-101);

	list.addToList(3);
	
	list.printList();

	printf("list[2]=%d\n",list.ptr[2]);

	return;

}/*end main*/