//--------------------------------------------------------------------------------------------------
// Filename:    ReadMe.txt
// Author:      Bruno Sousa (BS)
// Created in:  21-03-2001 12:36:09
// Version:     1.00.00
// 
// Remarks:     None.
//--------------------------------------------------------------------------------------------------
// You can use this file free of charge. You are free to modify it in anyway you want and 
// redistribute it. Author takes no responsability on any damage resulting directly or indirectly by
// the use of this or any other related files.
//
// Bruno Sousa (BS)	(bsousa@fireworks-interactive.com)
// http://www.fireworks-interactive.com
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// 21-03-2001 12:36:09 Bruno Sousa (BS)
// File created.
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Contents
//--------------------------------------------------------------------------------------------------
	1 - Licence
	2 - Installation notes
	3 - Known issues/errors
	4 - History


//--------------------------------------------------------------------------------------------------
// 1 - Licence
//--------------------------------------------------------------------------------------------------
	You are allowed to use this file as you desire, this includes recompiling, modifying, including
it in software, commercial or not.

	Author takes no responsability on any damage resulting directly or indirectly by the use of this 
or any other related files.

	This files belong to the public domain.

	Author can be reached at: 
	
	bsousa@fireworks-interactive.com
	http://www.fireworks-interactive.com


//--------------------------------------------------------------------------------------------------
// 2 - Installation notes
//--------------------------------------------------------------------------------------------------
	Unzip SealRFS to a directory of choice.
	Open SealRFS Workspace in Microsoft Visual C++


//--------------------------------------------------------------------------------------------------
// 3 - Known issues/errors
//--------------------------------------------------------------------------------------------------
	RLE compression has a problem when position % 127 = 126 because it will treat the next
segment as a compressed segment even if it isnt. RLE compression was removed to prevent corrupt 
files.

//--------------------------------------------------------------------------------------------------
// 4 - History
//--------------------------------------------------------------------------------------------------
	Version 1.00.00 - 21-03-2001 12:36:09
	
		- Basic functionability
		- bit-wise encryptation
		- raw files
		- support for directory structures


	//------------------
	// End of file

