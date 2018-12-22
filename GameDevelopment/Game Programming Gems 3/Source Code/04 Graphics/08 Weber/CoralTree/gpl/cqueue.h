/******************************************************************************
A circular queue can be used as a LIFO (stack), FIFO, etc.  This structure is
dynamic in that its allocated size adapts to the number of elements.

keywords: c++, template, data, blackbox_system

******************************************************************************/
#define GPL_INDEX_INCR(i) if(++(i) == SizeAllocated) i = 0
#define GPL_INDEX_DECR(i) if((i)-- == 0) i = SizeAllocated - 1

class GPL_Core_CircularQueue
{
#ifdef GPL_PROBE_CIRCULAR_QUEUE
	public:
#else
	private:
#endif

		void	**Array;
		long 	Size;
		long 	SizeAllocated;
		long	SizeBlock;
		long 	Head;
		long 	Tail;  

	public:

				GPL_Core_CircularQueue(long block_size);
virtual			~GPL_Core_CircularQueue(void);

		long	AddTail(void *entry);
		long	AddHead(void *entry);

		void	*RemoveHead(void);
		void	*RemoveTail(void);
	
		long	GetSize(void)	 { return Size; }
};

template<class Entry>
class GPL_CircularQueue : public GPL_Core_CircularQueue
{
	public:
				GPL_CircularQueue(long block_size) : GPL_Core_CircularQueue(block_size){}

		long	AddTail(Entry *entry){ return GPL_Core_CircularQueue::AddTail((void *)entry); }
		long	AddHead(Entry *entry){ return GPL_Core_CircularQueue::AddHead((void *)entry); }

		Entry	*RemoveHead(){ return (Entry *)GPL_Core_CircularQueue::RemoveHead(); }
		Entry	*RemoveTail(){ return (Entry *)GPL_Core_CircularQueue::RemoveTail(); }
};

