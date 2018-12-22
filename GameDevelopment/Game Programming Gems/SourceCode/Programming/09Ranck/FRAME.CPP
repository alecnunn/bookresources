/* Copyright (C) Steven Ranck, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steven Ranck, 2000"
 */
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include "frame.h"


static int _nByteAlignment;	// Memory alignment in bytes
static u8 *_pMemoryBlock;	// Value returned by malloc()
static u8 *_apBaseAndCap[2];// [0]=Base pointer, [1]=Cap pointer
static u8 *_apFrame[2];		// [0]=Lower frame pointer, [1]=Upper frame pointer


// Must be called exactly once at game initialization time.
// nByteAlignment must be a power-of-2.
// Returns 0 if successful, or 1 if an error occurred.
int InitFrameMemorySystem( int nSizeInBytes, int nByteAlignment ) {
	// Make sure nSizeInBytes is a multiple of nByteAlignment:
	nSizeInBytes = ALIGNUP( nSizeInBytes, nByteAlignment );

	// First allocate our Memory Block:
	_pMemoryBlock = (u8 *)malloc( nSizeInBytes + nByteAlignment );
	if( _pMemoryBlock == 0 ) {
		// Not enough memory. Return error flag:
		return 1;
	}

	_nByteAlignment = nByteAlignment;

	// Set up Base pointer:
	_apBaseAndCap[0] = (u8 *)ALIGNUP( _pMemoryBlock, nByteAlignment );

	// Set up Cap pointer:
	_apBaseAndCap[1] = (u8 *)ALIGNUP( _pMemoryBlock + nSizeInBytes, nByteAlignment );

	// Finally, initialize the Lower and Upper frame pointers:
	_apFrame[0] = _apBaseAndCap[0];
	_apFrame[1] = _apBaseAndCap[1];

	// Successful!
	return 0;
}


void ShutdownFrameMemorySystem( void ) {
	free( _pMemoryBlock );
}


// Returns a pointer to the base of the memory block,
// or returns 0 if there was insufficient memory.
// nHeapNum is the heap number: 0=lower, 1=upper.
void *AllocFrameMemory( int nBytes, int nHeapNum ) {
	u8 *pMem;

	// First, align the requested size:
	nBytes = ALIGNUP( nBytes, _nByteAlignment );

	// Check for available memory:
	if( _apFrame[0]+nBytes > _apFrame[1] ) {
		// Insufficient memory:
		return 0;
	}

	// Now perform the memory allocation:

	if( nHeapNum ) {
		// Allocating from upper heap, down:

		_apFrame[1] -= nBytes;
		pMem = _apFrame[1];

	} else {
		// Allocating from lower heap, up:

		pMem = _apFrame[0];
		_apFrame[0] += nBytes;
	}

	return (void *)pMem;
}


// Returns a Frame handle which can be used to
// later release memory allocated henceforth.
// nHeapNum is the heap number: 0=lower, 1=upper.
Frame_t GetFrame( int nHeapNum ) {
	Frame_t Frame;

	Frame.pFrame = _apFrame[nHeapNum];
	Frame.nHeapNum = nHeapNum;

	return Frame;
}


void ReleaseFrame( Frame_t Frame ) {
	// Check validity if releasing in lower heap (0):
	assert( Frame.nHeapNum==1 || (uint)Frame.pFrame<=(uint)_apFrame[0] );

	// Check validity if releasing in upper heap (1):
	assert( Frame.nHeapNum==0 || (uint)Frame.pFrame>=(uint)_apFrame[1] );

	// Release frame:
	_apFrame[Frame.nHeapNum] = Frame.pFrame;
}
