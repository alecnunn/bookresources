#include "microthreads.h"
#pragma hdrstop

#include <vector>
using namespace std;

// When this is defined the micro threads are copied to a shared location
// before running. This lets them temporarily use lots of stack and it means
// that exactly the right amount of stack will always be allocated for threads.
// Thread swapping will be slightly faster without this - but not much. I don't
// recommend turning this off, and I haven't tested it much. If you turn off
// micro thread stack copying, don't use dprintf from within micro threads,
// and don't do anything that might use stack.
#define	COPY_MICRO_THREAD_STACKS

#ifdef	COPY_MICRO_THREAD_STACKS
	// When this is defined you are sharing your main thread stack
	// with the micro threads. You will waste a bit of memory for
	// the unused stack buffer between the two uses.
	// When this is not defined you can not use C++ exceptions, OutputDebugString
	// or structured exception handling inside micro threads. This also means
	// that exceptions from micro threads will not propagate to handlers defined
	// outside of them - unless this is defined.
	#define	PUT_MICRO_THREADS_ON_MAIN_THREAD_STACK
#else
	#pragma message("\tWarning - this option is not recommended. Some types of stack overwriting")
	#pragma message("\tmay be detected, but not all. Your code may crash in very hard to")
	#pragma message("\tdiagnose ways. Use stack copying. It's very fast.")
	// Put your micro-thread stack size here. There is no safe size.
	// It takes a value of several thousand to make SimpleGame run properly.
	// At that point, the memory savings of micro threads have disappeared.
	const int k_stackSize = 5000;
	// I put this at the bottom of each micro-thread stack, and watch for it. If it
	// goes missing, it means we've had a stack overwrite.
	const int k_magicNumber = 0xDEADBEEF;
#endif

namespace MicroThread
{
	// Stacks on Win32 are almost always DWORD aligned and DWORD size. However,
	// this is not actually guaranteed. It is possible to push two bytes onto
	// the stack, which is why I'm not using an uint32_t for my stack type.
	// Since there is no cost for this flexibility, I might as well use it.
	typedef uint8_t StackType;

	class MicroThreadDetails : public MicroThreadInfo
	{
	public:
		friend void MicroThreadSleep();
		friend bool SwitchToMicroThread(MicroThreadInfo* const pScript);
		virtual size_t MicroThreadStackSize() const
		{
			if (GetActiveMicroThread() == (const MicroThreadInfo *)this)
				return 0;	// Doesn't make sense.
			return m_stack.size() * sizeof(StackType);
		}

		std::vector<StackType>	m_stack;	// Copy of the script's stack at sleep time.
		#ifndef COPY_MICRO_THREAD_STACKS
			MicroThreadDetails()
				: m_stack(k_stackSize)
			{
				uint32_t* pStackBottom = reinterpret_cast<uint32_t*>(&m_stack[0]);
				*pStackBottom = k_magicNumber;	// Stack overwrite detection.
			}
			StackType* s_scriptStack;
		#endif
	};

	#ifdef	COPY_MICRO_THREAD_STACKS
		#ifdef	PUT_MICRO_THREADS_ON_MAIN_THREAD_STACK
			static StackType*	s_scriptStackTop = 0;
			// Specify how much memory you expect to need for your main thread stack.
			// The main disadvantage to having this large is that you are effectively
			// allocating that much memory. Don't specify a number larger than the
			// amount of address space reserved for your stack - one megabyte by default.
			const int k_mainThreadStackSize = 120000;
		#else
			static StackType	s_scriptStackBottom[16384];	// Must be big enough for dprintf(), etc.
			static StackType*	s_scriptStackTop = s_scriptStackBottom + NUMELEMENTS(s_scriptStackBottom);
		#endif
	#endif
	static StackType*	s_globalStackPointer;

	// Global record of what script is currently running.
	static MicroThreadDetails* s_activeScript;

	MicroThreadInfo* GetActiveMicroThread()
	{
		return s_activeScript;
	}

	void MicroThreadInfo::MarkAsDead()
	{
		m_dead = true;
	}

	void DeleteMicroThread(MicroThreadInfo* pMicroThread)
	{
		// Right now this function doesn't do anything special, but requiring
		// it to be called gives us some extra flexibility.
		delete pMicroThread;
	}

	void __declspec(naked) MicroThreadSleep()
	{
		// Six registers need to be preserved and restored, and this routine
		// is twelve instructions long. Therefore, without using multi-register
		// move instructions, this routine is probably as short as it can be.
		// We could use the multi-register push/pop instructions pushad and popad
		// but that would mean pushing and popping three extra registers. I'm
		// not sure if that would be faster.
		__asm
		{
			// Preserve all of the registers that VC++ insists we preserve.
			// VC++ does not care if we preserve eax, ecx and edx.
			push	ebx
			push	ebp
			push	esi
			push	edi

			// Swap esp and s_globalStackPointer
			mov		eax, esp
			mov		esp, s_globalStackPointer
			mov		s_globalStackPointer, eax

			// Restore all of the registers that we previously preserved.
			// Yes, they're coming off of a different stack - they were
			// carefully placed there earlier.
			pop		edi
			pop		esi
			pop		ebp
			pop		ebx
			ret
		}
	}

	void MicroThreadSleep(int Frames)
	{
		while (Frames--)
			MicroThreadSleep();
	}

	void MicroThreadReturn()
	{
		s_activeScript->MarkAsDead();
		// We either need to loop forever here or we need to tell the scheduler
		// to not schedule dead threads. It is wasteful to check before scheduling
		// each thread whether it is dead, since this will also need to be checked
		// at a higher level (to see if it should be deleted), so I choose to loop
		// endlessly. If you remove your dead threads promptly, then this is the
		// best choice.
		while (true)
		{
			MicroThreadSleep();
		}
		DEBUG_BREAK();	// We should never get here.
	}

	static void PreserveStack(vector<StackType>& StackStorage)
	{
		#ifdef COPY_MICRO_THREAD_STACKS
			size_t StackSize = s_scriptStackTop - s_globalStackPointer;
			// For some reason a resize and then memmove is much faster than a
			// clear and then insert.
			StackStorage.resize(StackSize);
			memmove(&StackStorage[0], s_globalStackPointer, StackSize);
			//StackStorage.clear();
			//StackStorage.insert(StackStorage.begin(), s_globalStackPointer, s_scriptStackTop);
		#else
			s_activeScript->s_scriptStack = s_globalStackPointer;
			uint32_t* pStackBottom = reinterpret_cast<uint32_t*>(&s_activeScript->m_stack[0]);
			if (*pStackBottom != k_magicNumber)
				DEBUG_BREAK();	// Stack overwrite! Memory corruption! Very serious problem!
		#endif
	}

	static void RestoreStack(vector<StackType>& StackStorage)
	{
		#ifdef COPY_MICRO_THREAD_STACKS
			#if defined(PUT_MICRO_THREADS_ON_MAIN_THREAD_STACK) && defined(_DEBUG)
				int localVariable;
				// Check for stack conflicts.
				// The current stack pointer is detected by the address of a
				// local variable.
				StackType* currentStackPointer = (StackType*)&localVariable;
				const int k_safetyMargin = 1000;
				// If our current stack is close to or below our micro thread working
				// area then we will overwrite it if we proceed. We MUST stop immediately.
				if (currentStackPointer - k_safetyMargin < s_scriptStackTop)
					DEBUG_BREAK();
			#endif
			s_globalStackPointer = s_scriptStackTop - StackStorage.size();
			memmove(s_globalStackPointer, &(StackStorage[0]), StackStorage.size() * sizeof(StackType));
		#else
			s_globalStackPointer = s_activeScript->s_scriptStack;
		#endif
	}

	// Returns true if the script is still running. If it returns false then you
	// should whoever called SwitchToMicroThread() is responsible for deleting the
	// script.
	bool SwitchToMicroThread(MicroThreadInfo* const pScriptBase)
	{
		assert(pScriptBase);
		// Dead threads will be scheduled, but they won't do anything.
		//if (pScriptBase->IsDead())	// Don't schedule dead threads.
		//	return false;
		// This is very bad. Don't schedule one thread from another.
		// The RestoreStack function assumes that it can use the
		// s_scriptStack area of memory, but the currently executing
		// thread is already using it. It is possible to orchestrate a switch,
		// but I have chosen not too.
		#ifdef	_DEBUG
		if (s_activeScript)
			DEBUG_BREAK();
		#endif
		MicroThreadDetails* pScript = static_cast<MicroThreadDetails*>(pScriptBase);
		s_activeScript = pScript;
		RestoreStack(pScript->m_stack);
		// This function is pure magic. It switches to the thread's stack, and implicitly
		// to the thread, and when it calls MicroThreadSleep() it switches right back to
		// here.
		MicroThreadSleep();
		PreserveStack(pScript->m_stack);
		#ifdef	_DEBUG
		s_activeScript = 0;
		#endif
		return !pScript->IsDead();
	}

	#ifdef	PUT_MICRO_THREADS_ON_MAIN_THREAD_STACK
		const int k_stackUsagePerFunction = 1000;
		// Temporarily turn off optimizations, because otherwise this function
		// will be optimized away completely.
		#pragma optimize( "", off )
		static char* uselessBuffer;
		void PrimeStack(int x)
		{
			// Use up big chunks of stack.
			char	unusedArray[k_stackUsagePerFunction];
			uselessBuffer = unusedArray;	// Avoid unreferenced local variable warning.
			if (x > 0)
				PrimeStack(x - 1);
		}
		#pragma optimize( "", on ) 
	#endif

	MicroThreadInfo* CreateMicroThread(const tMicroThreadFunction scriptFunction, void* const pData /*= 0*/)
	{
		#ifdef	PUT_MICRO_THREADS_ON_MAIN_THREAD_STACK
			static bool s_stackPrimed = false;
			if (!s_stackPrimed)
			{
				// Make the OS commit memory far enough down the stack.
				PrimeStack(k_mainThreadStackSize / k_stackUsagePerFunction);
				int localVariable;
				// Initialize the script stack top to an appropriate location.
				// The current stack pointer is detected by the address of a
				// local variable.
				s_scriptStackTop = ((StackType*)&localVariable) - k_mainThreadStackSize;
				s_stackPrimed = true;
			}
		#endif
		MicroThreadDetails* pScript = new MicroThreadDetails;
		// All we have to do to initialize a new micro thread is create a plausible initial
		// stack, containing the needed registers. That's it.

		// At the bottom of the stack we have the four registers that we have to preserve and
		// restore. Above that we have the address of our startup function that we fall back
		// into. Once we've returned to that function we want to look like we just called
		// that function, so the next value down is the return address. We put MicroThreadReturn
		// there so that if the users thread routine returns they will automatically call
		// it. Lastly we push on the user specified parameter. Done.
		// There is something vaguely distressing about returning to the beginning of a
		// function, but there's nothing wrong with it.
		void* initialStack[] = {0, 0, 0, 0, scriptFunction, MicroThreadReturn, pData};
		#ifdef COPY_MICRO_THREAD_STACKS
			pScript->m_stack.insert(pScript->m_stack.begin(), (StackType*)initialStack, ((StackType*)initialStack) + sizeof(initialStack));
		#else
			assert(pScript->m_stack.size() == k_stackSize);
			pScript->s_scriptStack = &pScript->m_stack[k_stackSize - sizeof(initialStack)];
			memcpy(pScript->s_scriptStack, initialStack, sizeof(initialStack));
		#endif
		return pScript;
	}

};	// End MicroThread namespace
