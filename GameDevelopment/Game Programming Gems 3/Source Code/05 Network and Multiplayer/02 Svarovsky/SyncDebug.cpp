#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include "mytypes.h" // some game-specific types.

#include "syncdebug.h"

#ifdef _SYNCDEBUG

// A queue of the latest messages. You can make this queue much shorter but
// we found it more useful to have a huge queue, and therefore have the luxury
// of producing absolutely huge amounts of sync data.
static const DWORD MAX_MESSAGES = 1 << 20;

// The queue is declared local to this module.
static SyncDebug ms[MAX_MESSAGES];
static DWORD ms_upto = 0;

//////////////////////////////////////////////////////////////////////////////

// A shorthand that will be used several times in the later code to step along
// the queue.
static void	inc()
{
	ms_upto++;
	ms_upto %= MAX_MESSAGES;
}

// part of the internals of the function used to dump out all the sync data.
static BOOL output(SyncDebug &m, FILE *f)
{
	switch(m.Type)
	{
	case SyncDebug::EMPTY         : break;
	case SyncDebug::STATIC_TEXT   : fprintf(f, "%s "   , m.Text ); break;
	case SyncDebug::TYPE_FLOAT    : fprintf(f, "%f "   , m.Float); break;
	case SyncDebug::TYPE_INT	  : fprintf(f, "%d "   , m.Int  ); break;
	case SyncDebug::GAMETURN_START: fprintf(f, "\nGameTurn: %d\n\n", m.GameTurn); break;

		// expansion of the system for our little game-specific type.
	case SyncDebug::VX            : fprintf(f, "{%f, " , m.Float); break;
	case SyncDebug::VY            : fprintf(f, "%f, "  , m.Float); break;
	case SyncDebug::VZ            : fprintf(f, "%f} "  , m.Float); break;

	default:
		assert(0);
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

// implementation of the different functions adding messages to the queue, for
// different data types.

void SyncDebug::Msg(const char* t)
{
	ms[ms_upto].Type = STATIC_TEXT;
	ms[ms_upto].Text = t;

	inc();
}

void SyncDebug::Msg(float f)
{
	ms[ms_upto].Type  = TYPE_FLOAT;
	ms[ms_upto].Float = f;

	inc();
}

void SyncDebug::Msg(int i)
{
	ms[ms_upto].Type = TYPE_INT;
	ms[ms_upto].Int  = i;

	inc();
}

// Here's an example of the output of a user-defined type.
void SyncDebug::Msg(const M31 &v)
{
	ms[ms_upto].Type  = VX ;
	ms[ms_upto].Float = v.X;
	inc();

	ms[ms_upto].Type  = VY ;
	ms[ms_upto].Float = v.Y;
	inc();

	ms[ms_upto].Type  = VZ ;
	ms[ms_upto].Float = v.Z;
	inc();
}

/////////////////////////////////////////////////////////////////

// two special messages; the new-line shorthand, and the gameturn
// increment message.

void SyncDebug::NewLine()
{
	ms[ms_upto].Type = STATIC_TEXT;
	ms[ms_upto].Text = ("\n");

	inc();
}

void SyncDebug::NewGameTurn(DWORD gt)
{
	ms[ms_upto].Type = GAMETURN_START;
	ms[ms_upto].GameTurn = gt;

	inc();
}

//////////////////////////////////////////////////////////////////////

// The implementation of the function that dumps the report out to
// disk on request.

// This internal function is the meat of it.
static BOOL output_to_file_internal(DWORD oldest_gameturn, DWORD current_gameturn, DWORD gt_estimate, const char *fname)
{
	// open the destination file.
	FILE *f = fopen(fname, ("wa") );

	// fail.
	if (!f) return FALSE;

	// A nice header explaining what's going on.
	fprintf(f, ("Game out of sync probably in gameturn %d. Debug output from %d to %d:\n\n"),
			gt_estimate, oldest_gameturn, current_gameturn);

	// find start of the oldest_gameturn (if we don't find it, we'll start from the tail of the queue)
	DWORD start = ms_upto;
	DWORD end   = ms_upto;
	for (DWORD i = 0; i < MAX_MESSAGES; i++)
	{
		if (ms[i].Type == SyncDebug::GAMETURN_START)
		{
			if (ms[i].GameTurn == oldest_gameturn)
			{
				// found it! Start printing from here.
				start = i;
			}
			if (ms[i].GameTurn == current_gameturn)
			{
				// and finish here.
				end = i;
			}
		}
	}

	if (start == ms_upto)
	{
		fprintf(f, ("WARNING: Start of debug output too old to have been stored...\n\n") );
	}

	// now print.
	if (start < end)
	{
		// simple run through queue
		for (i = start; i < end; i++)
		{
			if (!output(ms[i], f)) return FALSE;
		}
	}
	else
	{
		// the run through is broken by the end of the queue array.
		for (i = start; i < MAX_MESSAGES; i++)
		{
			if (!output(ms[i], f)) return FALSE;
		}
		for (i = 0    ; i < end     ; i++)
		{
			if (!output(ms[i], f)) return FALSE;
		}
	}

	if (end == ms_upto)
	{
		fprintf(f, "WARNING: End of debug output too fresh for this machine.\n" );
	}

	// finish.
	fclose(f);
	return TRUE;
}

// This is the function called by the game. The only reason the above code is separated out
// is because it is called twice, once to the network drive, if it exists, and once to
// the local c:
BOOL SyncDebug::OutputToFile(DWORD oldest_gameturn, DWORD current_gameturn, DWORD gt_estimate)
{
	// (just in case)
#ifndef FINAL_BUILD

	// make up a filename
	char fname[80];
	char username[100] = "unknown user";
	DWORD size = sizeof(username);
	GetComputerName(username, &size);

	sprintf(fname, "t:\\syncreport_%s.txt", username);

	// try outputting to these two places.
	BOOL retval = TRUE;

	if (!output_to_file_internal(oldest_gameturn, current_gameturn, gt_estimate, fname)) retval = FALSE;

	// And also to c:\ for people who don't have the shared network drive
	if (!output_to_file_internal(oldest_gameturn, current_gameturn, gt_estimate, "c:\\syncreport.txt")) retval = FALSE;

	return retval;
#endif // ifdef FINAL_BUILD
	return TRUE;
}

////////////////////////////////////////////////////////////////

// Resetting the queue.

void SyncDebug::Reset()
{
	for (int i = 0; i < MAX_MESSAGES; i++) ms[i].Type = EMPTY;
}

#endif

