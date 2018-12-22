// Main.cpp: Main program entrypoint.
// Date: 7/02/04
// Created by: Aurelio Reis

#include <windows.h>
#include <iostream>
#include <conio.h>
using namespace std;

#include "Character.h"
using namespace AURELIO_REIS;

#pragma comment(lib, "WinMM.lib")


// The character object.
CCharacter g_Character;


void main()
{
	// TODO: FindFiles in the 'characters' directory to load
	// all character files in it.
	const char *strFileName = "Data/Character.txt";

	// Begin the rudimentary timer.
	DWORD dwStartTime = timeGetTime();

	// Load and show the players states.
	if ( !g_Character.LoadStats( strFileName ) )
	{
		return;
	}

	cout << "Parsing completed in " << ( timeGetTime() - dwStartTime ) * 0.001f
		 << " seconds." << endl;

	// Show the players stats.
	g_Character.ShowStats();

	cout << endl << endl << endl;
	getch();
}