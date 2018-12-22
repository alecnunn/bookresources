#pragma once

#ifndef SYNCDEBUG_H
#define SYNCDEBUG_H

// Extra work done by the CPU to aid out-of-sync error debugging.
// Litter random points in your code with calls to SYNC_MSG, SYNC_INT and so on.

// To do the debugging, #define _SYNCDEBUG in your project settings.
// Elsewhere in your game you should calculate a simple checksum (such as the
// sum of all object positions) and when this checksum is no longer the same on
// all computers, simply call SyncDebug::OutputToFile() on every machine.
// This will write a large report to disk, either to the root of T: (which
// has always been a shared network temporary drive where I have worked),
// or C: if that does not exist.

// This shared file will have the messages that you littered your code with, as
// if they had been printf's and the file was the stdout.

// If you have a T:\, you will notice how the report is called syncreport_machinename.txt.
// This is useful when you're playing with others round the office when testing the game;
// one person can collect all the syncreports for the game. Typically only one machine
// will differ from the others.

#ifdef _SYNCDEBUG

//////////////////////////////////////////////////////////////////////////////
//
// Useful bit of the file.

// You can pass strings to SYNC_MSG, but they must be statically-declared strings,
// ie they stay in memory for the whole running of the game. This is because this
// module only saves the pointer to the strings, for speed's sake. You can also
// pass floats or integers.

#define SYNC_MSG(n)		 SyncDebug::Msg(n)

// SYNC_INT is good for most integer types; if you simply use SYNC_MSG the compiler
// will often complain about implicit type conversions. SYNC_INT just makes the
// type conversion explicit.

#define SYNC_INT(n)		 SyncDebug::Msg(int(n))

// This is a shorthand for SYNC_MSG("\n")

#define SYNC_CR()		 SyncDebug::NewLine()

// The new-gameturn message cannot be simply a SYNC_MSG because SyncDebug wants to
// know when gameturns happen, if it is asked to print out the report.

#define SYNC_GAMETURN(n) SyncDebug::NewGameTurn(n)

//////////////////////////////////////////////////////////////////////////////
//
// significantly less interesting. This is the implementation of the above #defines.
// I have put the three functions of interest at the top of the class.

class SyncDebug
{
public:

	// When the game has gone out of sync (as determined by a checksum you should
	// implement elsewhere), call this function, with the oldest gameturn for
	// which you want output produced, the current gameturn, and the gameturn
	// that you estimate the game went out of sync.
	static BOOL OutputToFile(DWORD oldest_gameturn, DWORD current_gameturn, DWORD gameturn_estimate); // TRUE if success

	// This clears out the out-of-sync data buffer.
	static void Reset();

	// There is a whole lot of Msg() functions later, but this one is more interesting
	// because it is a game-specific type that I have defined as an example
	// of how you can expand the system.
	static void Msg(const struct M31 &v);


	////////////////////////////////////////////////
	//
	// Beyond here in the file there is little of interest, unless you want to work
	// out how it works of course.
			
	enum TYPE
	{
		EMPTY = 0,
		STATIC_TEXT,
		TYPE_FLOAT,
		TYPE_INT,
		VX,
		VY,
		VZ,
		GX,
		GY,
		GZ,
		GAMETURN_START
	};

	TYPE Type;

	union
	{
		const char *Text;
		float Float;
		int Int;
		DWORD GameTurn;
	};

	static void Msg(const char* t);
	static void Msg(float f);
	static void Msg(int i);
	static void NewLine();
	static void NewGameTurn(DWORD gt);

};

#else // ifdef _SYNCDEBUG

// When you wish to remove the out-of-sync debugging, undefine _SYNCDEBUG in the project,
// recompile, and all the SYNC_* calls will be removed as follows:

#define SYNC_MSG(n)		 (1)
#define SYNC_INT(n)      (1)
#define SYNC_CR()		 (1)
#define SYNC_GAMETURN(n) (1)
#define SYNC_THING()	 (1)

#endif // ifdef _SYNCDEBUG

#endif
