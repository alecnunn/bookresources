/*----------------------------------------------------------------------------
 *
 *					DEMONSTRATION OF THE IN/OUT CALL FROM APPLICATION LEVEL
 *			(Attention! the GIVEIO.SYS driver must be previously loaded!)
 *			====================================================================
 *
 *	hard disks enabling/disabling (I usually disable the second HDD, when it
 *  isn't needed, for the sake of silence)
----------------------------------------------------------------------------*/
#include <stdio.h>
#include <windows.h>

#define IDE_DRIVEHEAD_REG	6
#define IDE_COMMAND_REG		7
#define IDE_DRIVE_BIT		4
#define IDE_PRIMARY_BASE	0x1F0
#define IDE_SECONDRY_BASE	0x170
#define IDE_MASTER			0x0
#define IDE_SLAYER			0x1

#define IDE_SLEEP_CMD		0xE6

int main(int argc, char** argv)
{

	HANDLE	h;
	BYTE	device;	
	BYTE	command;
	DWORD	controller, ide_reg;

	command	= 	IDE_SLEEP_CMD;

	if (argc<2)
	{
		fprintf(stderr,"USAGE: HDDOFF.EXE {P,S}{M,S}\n");
		fprintf(stderr,"P - Primary controller\n");
		fprintf(stderr,"S - Secondary controller\n");
		fprintf(stderr,"M - Master device\n");
		fprintf(stderr,"S - Slayer device\n\n");
		fprintf(stderr,"for example: HDDOFF.EXE SM  - turn off secondary master\n");
		return -1;
	}

	if (strlen(argv[1])<2)
	{
		fprintf(stderr,"-ERR: incorrect arg\n");
		return -1;
	}

	switch(argv[1][0])
	{
		case 'P':	controller = IDE_PRIMARY_BASE;
					break;

	    case 'S':	controller = IDE_SECONDRY_BASE;
	    			break;

	    default:    fprintf(stderr, "-ERR: %c\n",argv[1][0]);
	    			return -1;
	}

	switch(argv[1][1])
	{
		case 'M':	device = IDE_MASTER;
					break;

	    case 'S':	device = IDE_SLAYER;
	    			break;

	    default:	fprintf(stderr, "-ERR: %c\n",argv[1][1]);
	    			return -1;
	}

	// allowing I/O
	h = CreateFile("\\\\.\\giveio", GENERIC_READ, 0, NULL, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (h == INVALID_HANDLE_VALUE)
	{
		printf("Couldn't access giveio device\n"); return -1;
	} 	CloseHandle(h);

	// selecting the device
	device = device << IDE_DRIVE_BIT; ide_reg = controller + IDE_DRIVEHEAD_REG;
	__asm
	{
		
		mov edx,	ide_reg
		mov al,		device
		_emit		0EEh	; OUT DX, AL
	}

	// giving the device time for preparation
	Sleep(10);

	// giving the device the command to go to sleep
    ide_reg = controller + IDE_COMMAND_REG;
	__asm{

		mov edx,	ide_reg
		mov al,		command
		_emit		0EEh	; OUT DX, AL
	}
	return 0;
}
