/*----------------------------------------------------------------------------
 *
 *					DEMO OF IN/OUT CALL FROM THE APPLICATION LEVEL
 *			(Attention! The GIVEIO.SYS driver must be previously loaded!)
 *			====================================================================
 *
 * -------------------------------------------------------------------------
 *
 * GIVEIO.TST: by Dale Roberts
 * Õ¿«Õ¿◊≈Õ»≈: Testing the GIVEIO driver by carrying out some I/O operation.
 *			 : (we are accessing the PC speaker)
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <conio.h>

typedef struct {
	short int pitch;
	short int duration;
} NOTE;

// NOTES
NOTE notes[] = {{14, 500}, {16, 500}, {12, 500}, {0, 500}, {7, 1000}};

// SETTING THE SPEAKER FREQUENCY
// SPEAKER IS CONTROLLED BY INTEL 8253/8254 SYSTEM TIMER WITH i/o PORTS 0X40-0X43
void setfreq(int hz)
{
	hz = 1193180 / hz;		// Basic timer frequency 1.19MHz
	_outp(0x43, 0xb6);		// Choosing timer 2, write operation, mode 3.
	_outp(0x42, hz);		// Setting frequency divider
	_outp(0x42, hz >> 8);	// most significant divider byte
}

//-----------------------------------------------------------------------------
//	Note duration is specified in parts of 400 Hz frequency, number 12 specifies 
//  the scale. Speaker is controlled through port 0x61. Setting two least 
//  significant bits allows to use channel 2 of 8253/8254 and enables the speaker.
//-----------------------------------------------------------------------------
void playnote(NOTE note)
{
	_outp(0x61, _inp(0x61) | 0x03);		// enable the speaker
		setfreq((int)(400 * pow(2, note.pitch / 12.0))); Sleep(note.duration);
	_outp(0x61, _inp(0x61) & ~0x03);	// disable
}



//----------------------------------------------------------------------------
//	Opening and closing the GIVEIO device, which allows us direct access to
//  I/O;
//	then trying to play music
//----------------------------------------------------------------------------
int main()
{
	int		i;
	HANDLE	h;
	
	h = CreateFile("\\\\.\\giveio", GENERIC_READ, 0, NULL, OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL, NULL);
	if(h == INVALID_HANDLE_VALUE)
	{
		printf("Couldn't access giveio device\n"); return -1;
	}
	CloseHandle(h);

	for(i = 0; i < sizeof(notes)/sizeof(int); ++i) playnote(notes[i]);

	return 0;
}
