/* Copyright (C) Bryon Hapgood, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bryon Hapgood, 2001"
 */
#define TempRet\
__asm{pop edx}\
__asm{call edx}

#define NAKED void __declspec(naked)
#define JUMP __asm jmp
#define RET __asm ret

static NAKED createWindow(){
	// open the window
	TempRet
	// close it
RET
}

static NAKED setCooperativeLevel(){
	// set to exclusive
	TempRet
	// restore
	RET
}

static NAKED changeDisplayMode(){
// set 640x480 16 bpp
TempRet
// restore
RET
}

static NAKED createSurfaces(){
	// create primary surface
// get attached back
TempRet
// restore
RET
}

NAKED SetupDX7(){
	createWindow();
	setCooperativeLevel();
	changeDisplayMode();
	createSurfaces();
	JUMP run
}

