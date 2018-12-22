// by Cuban.

// threw this together to ease some memory problems.

template< class T >
class tinyQueue
{
	struct node
	{
		T data;
		node* pNext;
		node* pPrev;
	};

	node head;
	node tail;

	tinyQueue()
	{
		head.pNext = &tail;
		tail.pPrev = &head;
	}

	void clear()
	{
		// delete everything
		while( head.pNext != &tail )
		{
			node* pTemp = head.pNext.pNext;
			delete head.pNext;
			head.pNext = pTemp;
		}
	}

	void push_back(
};