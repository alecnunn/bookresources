/* Copyright (C) Steven Ranck, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steven Ranck, 2000"
 */
#ifndef _FRAME_H
#define _FRAME_H

#define ALIGNUP( nAddress, nBytes ) ( (((uint)nAddress) + (nBytes)-1) & (~((nBytes)-1)) )

typedef unsigned char u8;
typedef unsigned int uint;

typedef struct {
	u8 *pFrame;
	int nHeapNum;
} Frame_t;

typedef struct {
	Frame_t SysmemFrame;	// System memory Frame
	Frame_t TexmemFrame;	// Texture memory Frame
	Frame_t SoundmemFrame;	// Sound memory Frame
} MasterFrame_t;

extern int GetObjectSize( const char *pszObjectName );
extern int LoadFromDisk( const char *pszObjectName, void *pLoadAddress );

// frame.cpp
extern int InitFrameMemorySystem( int nSizeInBytes, int nByteAlignment );
extern void ShutdownFrameMemorySystem( void );
extern void *AllocFrameMemory( int nBytes, int nHeapNum );
extern Frame_t GetFrame( int nHeapNum );
extern void ReleaseFrame( Frame_t Frame );

#endif
