/***
 * File:   MemoryManager.cpp - Implements MemoryManager.h
 *         -----------------------------------------------------
 * Author: Peter Dalton
 * Date:   3/23/01 1:23:45 PM
 *
 * Description:
			This Memory Manager software provides the following functionality:
			  1. Seamless interface.
			  2. Tracking all memory allocations and deallocations.
				3. Reporting memory leaks, unallocated memory.
				4. Reporting memory bounds violations.
				5. Reporting the percentage of allocated memory that is actually being used.
				6. Customizable tracking.

	    The code is self documented, thus reading through this implementation file should 
			explain how everything is implemented and the reasoning behind it.
 *
 * Copyright (C) Peter Dalton, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied warranties. You may freely copy 
 * and compile this source into applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 *                  "Portions Copyright (C) Peter Dalton, 2001"
 */

#include <string.h>           // It is important to note here the order in which the files are
#include <stdarg.h>           // included to avoid syntax and linking errors.  First you should
#include <stdio.h>            // include all of the required standard header files followed by
#include <time.h>             // the MemoryManager.h header.  All other custom file should be
#include <assert.h>           // included after the MemoryManager.h header. 
 
#include "MemoryManager.h"    

/*******************************************************************************************/
/*******************************************************************************************/
// ***** Implementation:

#ifdef ACTIVATE_MEMORY_MANAGER  // Only execute if the memory manager has been enabled.

/*******************************************************************************************/
// Turn off the defined macros to avoid confusion.  We want to avoid circular definition, 
// it is also not desired to track memory allocations within the memory manager module.
#include "new_off.h"

// Define our own version of assert to simply set a break point.
#define m_assert(x) if ((x) == false) __asm { int 3 }  // Set a break point

/*******************************************************************************************/
// ***** Global Variables Definitions:

const long PADDING          = 0xDEADC0DE;
const long BODY             = 0xBAADC0DE;
const char BREAK_ON_DEALLOC = 0x1;
const char BREAK_ON_REALLOC = 0x2;
const int  HASH_SIZE        = 1024;
int   NumAllocations        = 0;
char  LOGFILE[40]           = "memoryLogFile.log"; 
const char* const s_allocationTypes[] = { "Unknown", "new", "new[]", "malloc", "calloc",
                                          "realloc", "delete", "delete[]", "free" };


/*******************************************************************************************/
// ***** Here are the containers that make up the memory manager.  

struct StackNode {                  // This struct is used to hold the file name and line
	const char *fileName;             // number of the file that is requesting a deallocation.
	unsigned short lineNumber;        // Only deallocations are recorded since the allocation
	StackNode *next;                  // routines accept these additional parameters.
};

struct MemoryNode                   // This struct defines the primary container for tracking
{                                   // all memory allocations.  It holds information that 
  size_t         actualSize;        // will be used to track memory violations and information
  size_t         reportedSize;      // to help the user track down specific problems reported
  void          *actualAddress;     // to the log file upon termination of the program.
  void          *reportedAddress;   
  char           sourceFile[30];    // I have tried to keep the physical size of this struct
  unsigned short sourceLine;        // to a minimum, to reduce the memory tracking overhead.  
	unsigned short paddingSize;       // At the same time I have tried to allow for as much
  char           options;           // flexibility and information holding as possible.
	long           predefinedBody;
  ALLOC_TYPE     allocationType;
  MemoryNode    *next, *prev;

	void InitializeMemory( long body = BODY ) ; // Initailize the nodes memory for interrogation.
};

// This class implements a basic stack for record keeping.  It is necessary to use this class
// instead of the STL class since we need to explicitly call the init() method to initialize
// the data members.  This is due to the restriction of only using the malloc() method within
// this file to avoid calling our own new() method and creating circular definitions.  It is 
// necessary to create this stack for logging file information when deallocating memory due to
// to order in which memory is deallocated and the effect of the delete macro.  To understand
// this better refer to the sample illustration below.
// 
//    Sample Code     file1 => delete classAObject;
//                    file2 => ~classA() { delete[] intArray; }
//    
//    Function Calls      1.  setOwner( file1, line );
//                        2.  setOwner( file2, line );
//                        3.  deAllocate( intArray, MM_DELETE_ARRAY );
//                        4.  deAllocate( classAObject, MM_DELETE );
//
// The order of operations requires a stack for proper file tracking.
class myStack
{
public:
	myStack() {}
	__inline void init() 
		{ m_head = NULL; m_count = 0; }
	__inline bool empty() 
		{ return (m_count == 0); }
	__inline StackNode* top() 
		{ return m_head; }
	__inline void push( StackNode *n ) 
		{ if (!n) return; n->next = m_head; m_head = n; m_count++; }
	__inline StackNode* pop()  
		{ StackNode *n = m_head; if (n) m_head = m_head->next; m_count--; return n; } 
private:
	StackNode *m_head;
	int m_count;
};

// This class provides the implementation for the Memory Manager.  I created this class to 
// act as a container to centeralize the control instead of allowing everyone to become 
// intertangled.  Be design I have also left a lot of data members public for ease of access
// since this file is the only one that can ever create a MemoryManager object.
class MemoryManager
{
public:
  MemoryManager( void )  {};    // Default Constructor.
  ~MemoryManager( void ) {};    // Destructor.

	void initialize( void );      // Initailize internal memory.
	void release( void );         // Release internal memory.

	// Hash Table Routines
	void insertMemoryNode( MemoryNode *node );      // Insert a new memory node.
	MemoryNode *getMemoryNode( void *address );     // Retrieve a memory node.
	MemoryNode *removeMemoryNode( void *address );  // Remove a memory node.
	bool validateMemoryUnit( MemoryNode *node );    // Validate a memory node's memory.

	// Memory Caching to minimize allocations within the memory manager
	void deallocateMemory( MemoryNode *node );
	MemoryNode* allocateMemory( void );

	// Error Reporting Routines
	void dumpLogReport( void );
	void dumpMemoryAllocations( void );
	void log( char *s, ... );
	
	// User programmable options
	int           m_breakOnAllocationCount;
	unsigned int  m_paddingSize;
	bool          m_logAlways;
	bool          m_cleanLogFileOnFirstRun;

	// Statistical Information
	int          m_totalMemoryAllocations;  // The number of allocations.
	unsigned int m_totalMemoryAllocated;    // Number of bytes allocated.
	unsigned int m_totalMemoryUsed;         // Number of bytes used.
	
	unsigned int m_peakMemoryAllocation;    // The largest memory allocation.
	unsigned int m_peakTotalNumAllocations; // The largest number of allocation existing simaltaniously.
	unsigned int m_overheadMemoryCost;      // The current amount of memory required for memory tracking.
	unsigned int m_peakOverHeadMemoryCost;  // The peak overhead memory cost.
	unsigned int m_totalOverHeadMemoryCost; // The total overhead memory cost. 
	unsigned int m_allocatedMemory;         // The current amount of allocated memory.
	unsigned int m_numBoundsViolations;     // The number of memory bounds violations.

	// Stack for tracking file information for deallocations.
	myStack m_topStack;

	unsigned int m_numAllocations;      // The number of entries within the hash table.
private:
	int getHashIndex( void *address );  // Given an address this returns the hash table index
	int calculateUnAllocatedMemory();   // Return the amount of unallocated memory.

	MemoryNode *m_hashTable[HASH_SIZE]; // Hash Table container for tracking memory allocations.

	MemoryNode *m_memoryCache;          // Used for caching unused memory nodes.
};
MemoryManager *s_manager = NULL;      // Declaration of the one and only Memory Manager Object 


/*******************************************************************************************/
// ***** Function Prototypes:  Refer to implemations below for additional details.

bool InitializeMemoryManager( void );
void releaseMemoryManager( void );
char *formatOwnerString( const char *file, int line );
char *sourceFileStripper( const char *sourceFile );
void log( char *s, ... );
char *insertCommas( unsigned long value );
char *memorySizeString( unsigned long size, bool lengthenString = true );


/*******************************************************************************************/

/**
 * AllocateMemory():
 *  This is the main memory allocation routine, this is called by all of the other 
 *  memory allocation routines to allocate and track memory.
 * 
 *  Return Type: void 
 *  Arguments: 
 *  	const char *file	   : The file requesting the deallocation.
 *  	int line	           : The line within the file requesting the deallocation.
 *    size_t size          : The size of the memory to be reallocated.
 *  	ALLOC_TYPE type	     : The type of reallocation being performed.
 */
void *AllocateMemory( const char *file, int line, size_t size, ALLOC_TYPE type, void *address ) 
{
	MemoryNode *memory;

	// If the memory manager has not yet been initialized due to the order in which static
	// variables are allocated, create the memory manager here.
	if (!s_manager && !InitializeMemoryManager()) {
		if (NumAllocations != 0) {
			log( "The Memory Manager has already been released from memory, however an allocation was requested" );
			log( "\t%-40s", formatOwnerString( file, line ) );
		}
		return malloc( size );  // Try to allocate the memory for the requesting process. 
	}

  if (size == 0) size = 1; // ANSI states that allocation requests of size 0 should return
	                         // a valid pointer.
		
	// Has the user requested to break on the N-th allocation.
  m_assert( s_manager->m_totalMemoryAllocations != s_manager->m_breakOnAllocationCount );

  // If the type is UNKNOWN then this allocation was made from a source not set up to 
  // use memory tracking, include the MemoryManager header within the source to elimate
  // this error.
  m_assert( type != MM_UNKNOWN );

	if (type == MM_REALLOC) {
		MemoryNode *memory = s_manager->removeMemoryNode( address );

		// Validate that the memory exists
		m_assert( memory );
		if (!memory) {
			s_manager->log( "Request to reallocate RAM that was never allocated." );
		}

	  // Validate that there is not a allocation/reallocation mismatch
		m_assert( memory->allocationType == MM_MALLOC ||
			        memory->allocationType == MM_CALLOC ||
				      memory->allocationType == MM_REALLOC );

		// Validate that a break point on reallocation has not been requested.
		m_assert( (memory->options & BREAK_ON_REALLOC) == 0x0 );

		memory->actualSize    = size + s_manager->m_paddingSize * sizeof(long)*2;
		memory->reportedSize  = size;
		memory->actualAddress = realloc( memory->actualAddress, memory->actualSize );
	}
	else {
		// Create a new memory block for tracking the memory
		memory = s_manager->allocateMemory();

		// Validate the memory node allocation
		m_assert( memory != NULL );
		if (memory == NULL) {
			s_manager->log( "Could not allocate memory for memory tracking.  Out of memory." );
		}

		memory->actualSize        = size + s_manager->m_paddingSize * sizeof(long)*2;
		memory->reportedSize      = size;
		memory->actualAddress     = malloc( memory->actualSize );
		memory->options           = 0;
	}

  memory->reportedAddress   = (char*)memory->actualAddress + s_manager->m_paddingSize * sizeof(long);
  memory->sourceLine        = line;
	memory->paddingSize       = s_manager->m_paddingSize;
  memory->allocationType    = type;
  strncpy( memory->sourceFile, sourceFileStripper( file ), 30 );
  memory->sourceFile[29]=0;

	if (s_manager->m_logAlways) {
    s_manager->log( "Memory Allocation        : %-40s %8s(0x%08p) : %s", formatOwnerString( file, line ),
                    s_allocationTypes[type], memory->reportedAddress, memorySizeString( size ) );
  }

  // Validate the memory allocated
  m_assert( memory->actualAddress );
  if (!memory->actualAddress) {
		s_manager->log( "Request for allocation failed.  Out of memory." );
  }

  // Initialize the memory allocated for tracking upon deallocation
  if (type == MM_CALLOC) memory->InitializeMemory( 0x00000000 );
  else                   memory->InitializeMemory( 0xBAADC0DE );
  

  // Insert the memory node into the hash table, this is a linked list hash table.
	s_manager->insertMemoryNode( memory );
  return memory->reportedAddress;
}

/*******************************************************************************************/

/**
 * deAllocateMemory():
 *  This is the main memory de-allocation routine.  This method is used by all of the 
 *  other de-allocation routines for de-allocating and tracking memory.
 * 
 *  Return Type: void 
 *  Arguments: 
 *  	void *address	       : The address of memory to be deallocated.
 *  	ALLOC_TYPE type	     : The type of deallocation being performed.
 */
void deAllocateMemory( void *address, ALLOC_TYPE type ) 
{
	// If the memory manager has not yet been initialized due to the order in which static
	// variables are allocated, create the memory manager here.
	if (!s_manager && !InitializeMemoryManager()) {
		free( address );   // Release the memory
		if (NumAllocations != 0) {
			log( "The Memory Manager has already been released from memory, however a deallocation was requested" );
		}
		return;
	}

	// The topStack contains the logged information, such as file name and line number.
	StackNode *info = s_manager->m_topStack.empty() ? NULL : s_manager->m_topStack.top();

	// Does the memory node exist within the hash table of the memory manager.
	MemoryNode *memory = s_manager->removeMemoryNode( address );

  
  if (!memory) {      // Validate that the memory was previously allocated.  If the memory was not logged 
		free( address );  // by the memory manager simple free the memory and return.  We do not log or 
    return;           // create any errors since we want the memory manager to be as seemless as possible.
	}

	// Log the memory deallocation if desired.
  if (s_manager->m_logAlways) {
		s_manager->log( "Memory Deallocation      : %-40s %8s(0x%08p) : %s", 
			              formatOwnerString( info->fileName, info->lineNumber ),
			              s_allocationTypes[type], address, memorySizeString( memory->reportedSize ) );
  }

  // If the type is UNKNOWN then this allocation was made from a source not set up to 
  // use memory tracking, include the MemoryManager header within the source to elimate
  // this error.
  m_assert( type != MM_UNKNOWN );

	// Validate that no memory errors occured.  If any errors have occured they will be written to the log 
	// file by the validateMemoryUnit() method.
  s_manager->validateMemoryUnit( memory );

  // Validate that there is not a allocation/deallocation mismatch
  m_assert( type == MM_DELETE       && memory->allocationType == MM_NEW       ||
            type == MM_DELETE_ARRAY && memory->allocationType == MM_NEW_ARRAY ||
            type == MM_FREE         && memory->allocationType == MM_MALLOC    ||
            type == MM_FREE         && memory->allocationType == MM_CALLOC    ||
            type == MM_FREE         && memory->allocationType == MM_REALLOC );

  // Validate that a break on deallocate was not set
  m_assert( (memory->options & BREAK_ON_DEALLOC) == 0x0 );

  // Free the memory
  free( memory->actualAddress );

  // Free the memory used to create the Memory Node
  s_manager->deallocateMemory( memory );

	// Free the info node used to hold the file and line number information for this deallocation.
	if (info) {
		s_manager->m_topStack.pop();
		free( info );
	}
}

/*******************************************************************************************/
/*******************************************************************************************/
// ***** Helper Functions

/*******************************************************************************************/
/*******************************************************************************************/
// ****** Implementation of the MemoryManager Class:

/**
 * MemoryManager::initialize():
 *  This method is responsible for initializing the Memory Manager.  
 * 
 *  Return Type : void 
 *  Arguments   : NONE
 */
void MemoryManager::initialize( void ) 
{
	m_breakOnAllocationCount = -1;
	m_paddingSize            = 4; 
	m_logAlways              = true;
	m_cleanLogFileOnFirstRun = true;  

	m_totalMemoryAllocated = m_totalMemoryUsed         = m_totalMemoryAllocations  = 0;
	m_peakMemoryAllocation = m_numAllocations          = m_peakTotalNumAllocations = 0;   
	m_overheadMemoryCost   = m_totalOverHeadMemoryCost = m_peakOverHeadMemoryCost  = 0;
	m_allocatedMemory      = m_numBoundsViolations     = 0;

	for (int ii = 0; ii < HASH_SIZE; ++ii) {
		m_hashTable[ii] = NULL;
	}

	m_topStack.init();

	m_memoryCache = NULL;
}

/*******************************************************************************************/

/**
 * MemoryManager::release():
 *  This method is responsible for releasing the Memory Manager.  It dumps the log file and 
 *  cleans up any memory that has been left behind.
 * 
 *  Return Type : void 
 *  Arguments   : NONE
 */
void MemoryManager::release( void )
{
	dumpLogReport();     // Dump the statistical information to the log file.

	// If there are memory leaks, be sure to clean up memory that the memory manager allocated.
	// It would really look bad if the memory manager created memory leaks!!!
	if (m_numAllocations != 0) {
		for (int ii = 0; ii < HASH_SIZE; ++ii) {
			while (m_hashTable[ii]) {
				MemoryNode *ptr = m_hashTable[ii];
				m_hashTable[ii] = m_hashTable[ii]->next;
				free( ptr->actualAddress );      // Free the memory left behind by the memory leak.
				free( ptr );                     // Free the memory used to create the Memory Node.
			}
		}
	}

	// Clean up the stack if it contains entries.
	while (!m_topStack.empty()) {
		free( m_topStack.top() );
		m_topStack.pop();
	}

	// Clean the memory cache
	MemoryNode *ptr;
	while (m_memoryCache) {
		ptr = m_memoryCache;
		m_memoryCache = ptr->next;
		free( ptr );
	}
}

/*******************************************************************************************/

/**
 * MemoryManager::insertMemoryNode():
 *  Inserts a memory node into the hash table and collects statistical information.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *  	MemoryNode *node : The memory node to be inserted into the hash table.
 */
void MemoryManager::insertMemoryNode( MemoryNode *node )
{
	int hashIndex = getHashIndex( node->reportedAddress );
	node->next = m_hashTable[hashIndex];
	node->prev = NULL;

	if (m_hashTable[hashIndex]) m_hashTable[hashIndex]->prev = node;

	m_hashTable[hashIndex] = node;
	
	// Collect Statistic Information.
	m_numAllocations++;

	m_allocatedMemory += node->reportedSize;

	if (m_allocatedMemory > m_peakMemoryAllocation)   m_peakMemoryAllocation    = m_allocatedMemory;
	if (m_numAllocations > m_peakTotalNumAllocations) m_peakTotalNumAllocations = m_numAllocations;

	m_totalMemoryAllocated += node->reportedSize;
	m_totalMemoryAllocations++;
}

/*******************************************************************************************/

/**
 * MemoryManager::getMemoryNode():
 *  Returns the memory node for the given memory address, if the node does not exist a 
 *  NULL pointer is returned.
 * 
 *  Return Type : MemoryNode* -> A pointer to the requested memory node.
 *  Arguments   : 
 *  	void *address	: The address of the memory to be retrieved.
 */
MemoryNode* MemoryManager::getMemoryNode( void *address )
{
	MemoryNode *ptr = m_hashTable[getHashIndex( address )];
	while (ptr && ptr->reportedAddress != address) {
		ptr = ptr->next;
	}
	return ptr;
}

/*******************************************************************************************/

/**
 * MemoryManager::removeMemoryNode():
 *  Returns the memory node for the given memory address, if the node does not exist, a NULL
 *  pointer is returned.  This method also removes the memory node from the hash table.
 * 
 *  Return Type : MemoryNode* -> A pointer to the requested memory node.
 *  Arguments   : 
 *  	void *address	: The address of the memory to be retrieved.
 */
MemoryNode* MemoryManager::removeMemoryNode( void *address )
{
	int hashIndex = getHashIndex( address );
	
	if (hashIndex == 17) 
		int ttt = 0;
	
	MemoryNode *ptr = m_hashTable[hashIndex];
	while (ptr && ptr->reportedAddress != address) {
		ptr = ptr->next;
	}

	if (ptr) {
		if (ptr->next) ptr->next->prev = ptr->prev;
		if (ptr->prev) ptr->prev->next = ptr->next;
		else           m_hashTable[hashIndex] = ptr->next;

		// Update Statistical Information.
		m_numAllocations--;
		m_allocatedMemory -= ptr->reportedSize;
	}
	return ptr;
}

/*******************************************************************************************/

/**
 * MemoryManager::validateMemoryUnit():
 *  Given a Memory Node, this method will interrogate its memory looking for bounds violations
 *  and the number of bytes that were actually used.  This method should only be called before
 *  deleting a Memory Node to generate statistical information.  This method will report all 
 *  errors to the log file.  Returns TRUE if no bounds violations where found, otherwise FALSE.
 * 
 *  Return Type : bool -> True if no bounds violations, otherwise False.
 *  Arguments   : 
 *  	MemoryNode *node : The memory node to be interrogated.
 */
bool MemoryManager::validateMemoryUnit( MemoryNode *node )
{
	bool success = true;
	unsigned int ii;
	unsigned int totalBytesUsed = 0, boundViolations = 0;

	// Detect bounds violations
	long *beginning = (long*)node->actualAddress;
	long *ending    = (long*)((char*)node->actualAddress + node->actualSize - node->paddingSize*sizeof(long));
	for (ii = 0; ii < node->paddingSize; ++ii) {
		if (beginning[ii] != PADDING || ending[ii]!= PADDING) {
			success = false;  // Report the bounds violation.
			boundViolations++;
		}
	}

	if (!success) m_numBoundsViolations++;

	// Attempt to determine how much of the allocated memory was actually used.
	// Initialize the memory padding for detecting bounds violations.
	long *lptr       = (long*)node->reportedAddress;
	unsigned int len = node->reportedSize / sizeof(long);
	unsigned int cnt;
	for (cnt = 0; cnt < len; ++cnt) {                        
		if (lptr[cnt] != node->predefinedBody)       totalBytesUsed += sizeof(long);
	}
	char *cptr = (char*)(&lptr[cnt]);
	len = node->reportedSize - len*sizeof(long);
	for (cnt = 0; cnt < len; ++cnt) {    
		if (cptr[cnt] != (char)node->predefinedBody) totalBytesUsed++;
	}

	m_totalMemoryUsed += totalBytesUsed;

	if (m_logAlways && totalBytesUsed != node->reportedSize) {          // Report the percentage 
		this->log( "Unused Memory Detected   : %-40s %8s(0x%08p) : %s",   //  of waisted memory space.
			         formatOwnerString( node->sourceFile, node->sourceLine ),
			         s_allocationTypes[node->allocationType], node->reportedAddress, 
							 memorySizeString( node->reportedSize - totalBytesUsed ) );
	}

	if (m_logAlways && !success) {                                      // Report the memory 
		this->log( "Bounds Violation Detected: %-40s %8s(0x%08p)",        //  bounds violation.
			         formatOwnerString( node->sourceFile, node->sourceLine ),
			         s_allocationTypes[node->allocationType], node->reportedAddress );
	}

	return success;
}

/*******************************************************************************************/

/**
 * MemoryManager::deallocateMemory():
 *  This method adds the MemoryNode to the memory cache for latter use.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *  	MemoryNode *node : The MemoryNode to be released.
 */
void MemoryManager::deallocateMemory( MemoryNode *node )
{
	m_overheadMemoryCost -= (node->paddingSize * 2 * sizeof(long));
	node->next = m_memoryCache;
	m_memoryCache = node;
}

/*******************************************************************************************/

/**
 * MemoryManager::allocateMemory():
 *  This method checks the memory cache for unused MemoryNodes, if one exists it is removed
 *  from the cache and returned.  Otherwise, new memory is allocated for the MemoryNode and
 *  returned.
 * 
 *  Return Type : MemoryNode* -> The allocated MemoryNode.
 *  Arguments   : NONE
 */
MemoryNode* MemoryManager::allocateMemory( void )
{
	if (!m_memoryCache) {
		int overhead = m_paddingSize * 2 * sizeof(long) + sizeof( MemoryNode );
		m_overheadMemoryCost += overhead;
		m_totalOverHeadMemoryCost += overhead;

		if (m_overheadMemoryCost > m_peakOverHeadMemoryCost) {
			m_peakOverHeadMemoryCost =  m_overheadMemoryCost;
		}

		return (MemoryNode*)malloc( sizeof(MemoryNode) );
	}
	else {
		int overhead = m_paddingSize * 2 * sizeof(long);
		m_overheadMemoryCost += overhead;
		m_totalOverHeadMemoryCost += overhead;

		if (m_overheadMemoryCost > m_peakOverHeadMemoryCost) {
			m_peakOverHeadMemoryCost =  m_overheadMemoryCost;
		}

		MemoryNode *ptr = m_memoryCache;
		m_memoryCache = m_memoryCache->next;
		return ptr;
	}
}

/*******************************************************************************************/

/**
 * MemoryManager::dumpLogReport():
 *  This method implements the main reporting system.  It reports all of the statistical
 *  information to the desired log file.
 * 
 *  Return Type : void 
 *  Arguments   : NONE
 */
void MemoryManager::dumpLogReport( void )
{
	if (m_cleanLogFileOnFirstRun) {      // Cleanup the log?
		unlink( LOGFILE );                 // Delete the existing log file.
		m_cleanLogFileOnFirstRun = false;  // Toggle the flag.
	}

	FILE	*fp = fopen( LOGFILE, "ab" ); // Open the log file
	if (!fp) return;

  time_t t = time( NULL );
  tm *time = localtime( &t );
	
	int memoryLeak = calculateUnAllocatedMemory();
	int totalMemoryDivider = m_totalMemoryAllocated != 0 ? m_totalMemoryAllocated : 1;

	// Header Information
	fprintf( fp, "\r\n" );
	fprintf( fp, "******************************************************************************* \r\n");
  fprintf( fp, "*********           Memory report for: %02d/%02d/%04d %02d:%02d:%02d            ********* \r\n", time->tm_mon + 1, time->tm_mday, time->tm_year + 1900, time->tm_hour, time->tm_min, time->tm_sec );
	fprintf( fp, "******************************************************************************* \r\n");
	fprintf( fp, "\r\n" );

	// Report summary
	fprintf( fp, "                     T O T A L  M E M O R Y  U S A G E                          \r\n" );
	fprintf( fp, "------------------------------------------------------------------------------- \r\n" );
	fprintf( fp, "           Total Number of Dynamic Allocations: %10s\r\n", insertCommas( m_totalMemoryAllocations ) );
	fprintf( fp, "      Reported Memory usage to the Application: %s\r\n", memorySizeString( m_totalMemoryAllocated ) );
	fprintf( fp, "          Actual Memory use by the Application: %s\r\n", memorySizeString( m_totalOverHeadMemoryCost + m_totalMemoryAllocated ) );
	fprintf( fp, "                      Memory Tracking Overhead: %s\r\n", memorySizeString( m_totalOverHeadMemoryCost ) );
	fprintf( fp, "\r\n");

	fprintf( fp, "                      P E A K  M E M O R Y  U S A G E                           \r\n");
	fprintf( fp, "------------------------------------------------------------------------------- \r\n");
	fprintf( fp, "            Peak Number of Dynamic Allocations: %10s\r\n", insertCommas( m_peakTotalNumAllocations ) );
	fprintf( fp, " Peak Reported Memory usage to the application: %s\r\n", memorySizeString( m_peakMemoryAllocation ) );
	fprintf( fp, "     Peak Actual Memory use by the Application: %s\r\n", memorySizeString( m_peakOverHeadMemoryCost + m_peakMemoryAllocation ) );
	fprintf( fp, "                 Peak Memory Tracking Overhead: %s\r\n", memorySizeString( m_peakOverHeadMemoryCost ) );
	fprintf( fp, "\r\n");

	fprintf( fp, "                          U N U S E D  M E M O R Y                              \r\n");
	fprintf( fp, "------------------------------------------------------------------------------- \r\n");
	fprintf( fp, "  Percentage of Allocated Memory Actually Used: %10.2f %%\r\n", (float)(1 - (m_totalMemoryAllocated - m_totalMemoryUsed)/(float)totalMemoryDivider) * 100 );
	fprintf( fp, "       Percentage of Allocated Memory Not Used: %10.2f %%\r\n", (float)(m_totalMemoryAllocated - m_totalMemoryUsed)/(float)totalMemoryDivider * 100 );
	fprintf( fp, "        Memory Allocated but not Actually Used: %s\r\n", memorySizeString( m_totalMemoryAllocated - m_totalMemoryUsed ) );
	fprintf( fp, "\r\n");

	fprintf( fp, "                      B O U N D S  V I O L A T I O N S                          \r\n");
	fprintf( fp, "------------------------------------------------------------------------------- \r\n");
	fprintf( fp, "            Number of Memory Bounds Violations: %10s\r\n", insertCommas( m_numBoundsViolations ) );
	fprintf( fp, "\r\n");

	fprintf( fp, "                           M E M O R Y  L E A K S                               \r\n");
	fprintf( fp, "------------------------------------------------------------------------------- \r\n");
	fprintf( fp, "                        Number of Memory Leaks: %10s\r\n", insertCommas( m_numAllocations ) );
	fprintf( fp, "                 Amount of Memory Un-Allocated: %s\r\n", memorySizeString( memoryLeak ) );
	fprintf( fp, "   Percentage of Allocated Memory Un-Allocated: %10.2f %%\r\n", (float)(1 - (m_totalMemoryAllocated - memoryLeak)/(float)totalMemoryDivider) * 100 );
	fprintf( fp, "\r\n");

	if (m_numAllocations != 0) {  // Are there memory leaks?
		fclose( fp );               // Close the log file.
		dumpMemoryAllocations();    // Display any memory leaks.
	}
	else {
		fclose( fp );
	}
}

/*******************************************************************************************/

/**
 * MemoryManager::dumpMemoryAllocations():
 *  This method is responsible for reporting all memory that is currently allocated.  This is
 *  achieved by reporting all memory that is still within the hash table.  
 * 
 *  Return Type : void 
 *  Arguments   : NONE
 */
void MemoryManager::dumpMemoryAllocations( void )
{
	if (m_cleanLogFileOnFirstRun) {      // Cleanup the log?
		unlink( LOGFILE );                 // Delete the existing log file.
		m_cleanLogFileOnFirstRun = false;  // Toggle the flag.
	}

	FILE	*fp = fopen( LOGFILE, "ab" ); // Open the log file
	if (!fp) return;

	fprintf( fp, "              C U R R E N T L Y  A L L O C A T E D  M E M O R Y                 \r\n" );
	fprintf( fp, "------------------------------------------------------------------------------- \r\n" );

	for (int ii = 0, cnt = 1; ii < HASH_SIZE; ++ii) {
		for (MemoryNode *ptr = m_hashTable[ii]; ptr; ptr = ptr->next) {
			fprintf( fp, "** Allocation # %2d\r\n", cnt++ );
			fprintf( fp, "Total Memory Size : %s\r\n", memorySizeString( ptr->reportedSize, false ) );
			fprintf( fp, "Source File       : %s\r\n", ptr->sourceFile );
			fprintf( fp, "Source Line       : %d\r\n", ptr->sourceLine );
			fprintf( fp, "Allocation Type   : %s\r\n", s_allocationTypes[ptr->allocationType] );
			fprintf( fp, "\r\n");
		}
	}

	fprintf( fp, "------------------------------------------------------------------------------- \r\n" );
	fprintf( fp, "******************************************************************************* \r\n" );
	fprintf( fp, "\r\n" );

	fclose( fp );
}

/*******************************************************************************************/

/**
 * MemoryManager::log():
 *  Dumps a specific string to the log file.  Used for error reporting during runtime.  This
 *  method accepts a variable argument lenght such as printf() for ease of reporting.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *  	char *s	: The string to be written to the log file.
 *  	...	    : The parameters to be placed within the string, simular to say: printf( s, ... )
 */
void MemoryManager::log( char *s, ... )
{
	if (m_cleanLogFileOnFirstRun) {      // Cleanup the log?
		unlink( LOGFILE );                 // Delete the existing log file.
		m_cleanLogFileOnFirstRun = false;  // Toggle the flag.
	}

	static char buffer[2048];    // Create the buffer
	va_list	list;                // Replace the strings variable arguments with the provided
	va_start( list, s );         //  arguments.
	vsprintf( buffer, s, list );
	va_end( list );

	FILE	*fp = fopen( LOGFILE, "ab" );  // Open the log file
	if (!fp) return;

	fprintf( fp, "%s\r\n", buffer );     // Write the data to the log file
	fclose( fp );                        // Close the file
}

/*******************************************************************************************/

/**
 * MemoryManager::getHashIndex():
 *  Returns the hash index for the given memory address.
 * 
 *  Return Type : int -> The hash table index.
 *  Arguments   : 
 *  	void *address	: The address to determine the hash table index for.
 */
int MemoryManager::getHashIndex( void *address )
{
	return ((unsigned int)address >> 4) & (HASH_SIZE -1);
}

/*******************************************************************************************/

/**
 * MemoryManager::calculateUnAllocatedMemory():
 *  Returns the amount of unallocated memory in BYTES.
 * 
 *  Return Type : int -> The number of BYTES of unallocated memory.
 *  Arguments   : NONE
 */
int MemoryManager::calculateUnAllocatedMemory( void )
{
	int memory = 0;
	for (int ii = 0; ii < HASH_SIZE; ++ii) {
		for (MemoryNode *ptr = m_hashTable[ii]; ptr; ptr = ptr->next) {
			memory += ptr->reportedSize;
		}
	}
	return memory;
}


/*******************************************************************************************/
/*******************************************************************************************/
// ****** Implementation of the MemoryNode Struct

/**
 * MemoryNode::InitializeMemory():
 *  Initialize the padding and the body of the allocated memory so that it can be interrogated
 *  upon deallocation.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *  	long body	: The value to which the body of the allocated memory should be intialized too.
 */
void MemoryNode::InitializeMemory( long body ) 
{
	// Initialize the memory padding for detecting bounds violations.
	long *beginning = (long*)actualAddress;
	long *ending    = (long*)((char*)actualAddress + actualSize - paddingSize*sizeof(long));
	for (unsigned short ii = 0; ii < paddingSize; ++ii) {
		beginning[ii] = ending[ii] = PADDING;
	}

	// Initialize the memory body for detecting unused memory.
	beginning        = (long*)reportedAddress;
	unsigned int len = reportedSize / sizeof(long);
	unsigned int cnt;
	for (cnt = 0; cnt < len; ++cnt) {                         // Initialize the majority of the memory
		beginning[cnt] = body;
	}
	char *cptr = (char*)(&beginning[cnt]);
	len = reportedSize - len*sizeof(long);
	for (cnt = 0; cnt < len; ++cnt) {    // Initialize the remaining memory
		cptr[cnt] = (char)body;
	}

	predefinedBody = body;
}


/*******************************************************************************************/
/*******************************************************************************************/
// ****** Implementation of the Initialize Class

/**
 * Initialize::Initialize():
 *  Initialize the Memory Manager Object.  This class is required to ensure that the Memory
 *  Manager has been created before dynamic allocation occure within other statically 
 *  allocated objects.
 * 
 *  Return Type : 
 *  Arguments   : NONE
 */
Initialize::Initialize( void )
{
	InitializeMemoryManager();  // Create the Memory Manager Object.
}


/*******************************************************************************************/
/*******************************************************************************************/
// ****** Implementation of Helper Functions

/**
 * InitializeMemoryManager():
 *  This method is responsible for creating a Memory Manager Object.  If the object already
 *  exists or is successfully created TRUE is returned.  Otherwise if the object was 
 *  previously created and has been destroyed FALSE is returned.  The goal is to guarantee
 *  that the Memory Manager is the first object to be created and the last to be destroyed.
 * 
 *  Return Type : bool -> True if intialized, otherwise False.
 *  Arguments   : NONE
 */
bool InitializeMemoryManager( void )
{
	static bool hasBeenInitialized = false;

	if (s_manager) {                     // The memory manager object already exists.
		return true;
	}
	else if (hasBeenInitialized) {       // The memory manager object has already been created
		return false;                      //    once, however it was release before everyone 
	}                                    //    was done.
	else {                               // Create the memory manager object.
		s_manager = (MemoryManager*)malloc( sizeof(MemoryManager) );
		s_manager->initialize();
		atexit( releaseMemoryManager );    // Log this function to be called upon program shut down.
		hasBeenInitialized = true;
		return true;
	}
}

/*******************************************************************************************/

/**
 * releaseMemoryManager():
 *  This method is automatically called when the application is terminated.  It is important
 *  that this is the last function called to perform application cleanup since the memory 
 *  manager object should be the last object to be destoryed, thus this must be the first 
 *  method logged to perform application clean up.
 * 
 *  Return Type : void 
 *  Arguments   : NONE
 */
void releaseMemoryManager( void ) 
{
	NumAllocations = s_manager->m_numAllocations;
	s_manager->release();  // Dump the log report and free remaining memory.
	free( s_manager );
	s_manager = NULL;
}

/*******************************************************************************************/

/**
 * formatOwnerString():
 *  This method is responsible for formating the owner string.  This string states the file
 *  name and line number within the specified file.
 * 
 *  Return Type : char* -> A pointer to the string representing the owner string. 
 *  Arguments   : 
 *  	const char *file : The files name
 *  	int line	       : The line number within the specified file.
 */
char *formatOwnerString( const char *file, int line )
{
  static char	str[90];
	memset( str, 0, sizeof(str));
	sprintf( str, "%s(%05d)", sourceFileStripper(file), line );
	return str;
}

/*******************************************************************************************/

/**
 * sourceFileStripper():
 *  This method takes a file name and strips off all directory information.
 * 
 *  Return Type : char* -> A pointer to the actual file minus all directory information. 
 *  Arguments   : 
 *  	const char *sourceFile : The file to strip.
 */
char *sourceFileStripper( const char *sourceFile )
{
	if (!sourceFile) return NULL;
	char *ptr = strrchr( sourceFile, '\\' );
	if (ptr) return ptr + 1;
	ptr = strrchr(sourceFile, '/');
	if (ptr) return ptr + 1;
	return (char*)sourceFile;
}

/*******************************************************************************************/

/**
 * log():
 *  Dumps a specific string to the log file.  Used for error reporting during runtime.  This
 *  method accepts a variable argument lenght such as printf() for ease of reporting.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *  	char *s	: The string to be written to the log file.
 *  	...	    : The parameters to be placed within the string, simular to say: printf( s, ... )
 */
void log( char *s, ... )
{
	static char buffer[2048];            // Create the buffer
	va_list	list;
	va_start( list, s );
	vsprintf( buffer, s, list );
	va_end( list );

	FILE	*fp = fopen( LOGFILE, "ab" );  // Open the log file
	if (!fp) return;

	fprintf( fp, "%s\r\n", buffer );     // Write the data to the log file
	fclose( fp );                        // Close the file
}

/*******************************************************************************************/

/**
 * insertCommas():
 *  This method takes a value and inserts commas, creating a nicely formated string.  Thus
 *  the value => 23456 would be converted to the following string => 23,456.
 * 
 *  Return Type : char* -> A string representing the provided value with commas inserted. 
 *  Arguments   : 
 *  	unsigned long value	: The value to insert commas into.
 */
char *insertCommas( unsigned long value )
{
	static	char	str[30];
	for (int ii = 0; ii < 30; ++ii) str[ii] = NULL;
	sprintf(str, "%d", value);
	if (strlen(str) > 3) {
		memmove( &str[strlen(str)-3], &str[strlen(str)-4], 4 );
		str[strlen(str) - 4] = ',';
	}
	if (strlen(str) > 7) {
		memmove( &str[strlen(str)-7], &str[strlen(str)-8], 8 );
		str[strlen(str) - 8] = ',';
	}
	if (strlen(str) > 11) {
		memmove( &str[strlen(str)-11], &str[strlen(str)-12], 12 );
		str[strlen(str) - 12] = ',';
	}
	return str;
}

/*******************************************************************************************/

/**
 * memorySizeString():
 *  This method takes a memory size and creates a user friendly string that displays the 
 *  memory size in bytes, K or M. 
 * 
 *  Return Type : char* -> The final memory size string. 
 *  Arguments   : 
 *  	unsigned long size	: The size of the memory.
 *  	bool lengthenString : Whether or not to pad the string with white spaces.
 */
char *memorySizeString( unsigned long size, bool lengthenString /* = true */ )
{
	static	char	str[90];
	if (lengthenString) {
		if (size > (1024*1024))	sprintf(str, "%10s (%7.2fM)", insertCommas(size), size / (1024.0 * 1024.0));
		else if (size > 1024)		sprintf(str, "%10s (%7.2fK)", insertCommas(size), size / 1024.0);
		else				            sprintf(str, "%10s bytes     ", insertCommas(size), size);
	}
	else {
		if (size > (1024*1024))	sprintf(str, "%s (%7.2fM)", insertCommas(size), size / (1024.0 * 1024.0));
		else if (size > 1024)		sprintf(str, "%s (%7.2fK)", insertCommas(size), size / 1024.0);
		else				            sprintf(str, "%s bytes     ", insertCommas(size), size);
	}
	return str;
}

/*******************************************************************************************/
/*******************************************************************************************/
// ****** Implementation of Access Functions defined within MemoryManager.h

/**
 * dumpLogReport():
 *  Dump the log report to the file, this is the same method that is automatically called 
 *  upon the programs termination to report all statistical information.
 * 
 *  Return Type : void 
 *  Arguments   : NONE
 */
void dumpLogReport( void )
{
	if (s_manager) s_manager->dumpLogReport();
}

/*******************************************************************************************/

/**
 * dumpMemoryAllocations():
 *  Report all allocated memory to the log file.
 * 
 *  Return Type : void 
 *  Arguments   : NONE
 */
void dumpMemoryAllocations( void )
{
	if (s_manager) s_manager->dumpMemoryAllocations();
}

/*******************************************************************************************/

/**
 * setLogFile():
 *  Allows for the log file to be changed from the default.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *  	char *file : The name of the new log file.
 */
void setLogFile( char *file )
{
	if (file) strcpy( LOGFILE, file );
}

/*******************************************************************************************/

/**
 * setExhaustiveTesting():
 *  This method allows for exhaustive testing.  It has the same functionality as the following
 *  function calls => setLogAlways( true ); setPaddingSize( 1024 ); 
 * 
 *  Return Type : void 
 *  Arguments   : 
 *  	bool test : Whether or not to turn exhaustive testing on or off.
 */
void setExhaustiveTesting( bool test /* = true */ )
{
	if (!s_manager) return;

	if (test) {
		setPaddingSize( 1024 );
		setLogAlways();
	}
	else {
		setPaddingSize();
		setLogAlways( false );
	}
}

/*******************************************************************************************/

/**
 * setLogAlways():
 *  Sets the flag for exhaustive information logging.  All information is sent to the log file.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *  	bool log 	: Whether or not to log all information.
 */
void setLogAlways( bool log /* = true */ )
{
	if (s_manager) s_manager->m_logAlways = log;
} 

/*******************************************************************************************/

/**
 * setPaddingSize():
 *  Sets the padding size for memory bounds checks.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *  	int size 	: The new padding size.
 */
void setPaddingSize( int size /* = 4 */ )
{
	if (s_manager && size > 0) s_manager->m_paddingSize = size;
}

/*******************************************************************************************/

/**
 * cleanLogFile():
 *  Cleans out the log file by deleting it.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *  	bool clean : Whether or not to clean the log file.
 */
void cleanLogFile( bool clean /* = true */ )
{
	if (s_manager) s_manager->m_cleanLogFileOnFirstRun = true;
}

/*******************************************************************************************/

/**
 * breakOnAllocation():
 *  Allows you to set a break point on the n-th allocation.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *  	int allocationCount	: The allocation count to break on.
 */
void breakOnAllocation( int allocationCount )
{
	if (s_manager && allocationCount > 0) s_manager->m_breakOnAllocationCount = allocationCount;
}

/*******************************************************************************************/

/**
 * breakOnDeallocation():
 *  Sets a flag that will set a break point when the specified memory is deallocated.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *  	void *address	: The address to break on when it is deallocated.
 */
void breakOnDeallocation( void *address )
{
	if (!s_manager || !address) return;

	MemoryNode *node = s_manager->getMemoryNode( address );
	node->options |= BREAK_ON_DEALLOC;
}

/*******************************************************************************************/

/**
 * breakOnReallocation():
 *  Sets a flag that will set a break point when the specified memory is reallocated by 
 *  using the realloc() method.
 *
 *  Return Type : void 
 *  Arguments   : 
 *  	void *address	: The address to break on when it is reallocated.
 */
void breakOnReallocation( void *address )
{
	if (!s_manager || !address) return;

	MemoryNode *node = s_manager->getMemoryNode( address );
	node->options |= BREAK_ON_REALLOC;
}

/*******************************************************************************************/

/**
 * setOwner():
 *  This method is only called by the delete macro defined within the MemoryManager.h header.
 *  It is responsible for logging the file and line number for tracking information.  For
 *  an explanation for the stack implementation refer to the MemoryManager class definition.
 *
 *  Return Type : void 
 *  Arguments   : 
 *  	const char *file : The file requesting the deallocation.
 *  	int line	       : The line number within the file.
 */
void setOwner( const char *file, int line )
{
	if (s_manager) {
		StackNode *n = (StackNode*)malloc( sizeof(StackNode) );
		n->fileName = file;
		n->lineNumber = line;
		s_manager->m_topStack.push( n );
	}
}

#endif  /* ACTIVATE_MEMORY_MANAGER */

// ***** End of MemoryManager.cpp
/*******************************************************************************************/
/*******************************************************************************************/