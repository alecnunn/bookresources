/*----------------------------------------------------------------------------
 *
 *					LOCKS/UNLOCKS CD-ROM TRAY
 *					=========================
 *
 * build 0x001 @ 04.06.2003
----------------------------------------------------------------------------*/
#include <windows.h>
#include <winioctl.h>
#include <stdio.h>

#define IOCTL_CDROM_MEDIA_REMOVAL 0x24804

main(int argc, char **argv)
{
	int						a;
	BOOL					act;
	DWORD					xxxx;
	HANDLE					hCD;
	PREVENT_MEDIA_REMOVAL	pmrLockCDROM;
	printf("CD-ROM lock'er for 9x/NT IOCTL drive by Kris Kaspersky\n");
	// ARGUMENTS CHECK
	if (argc<3){printf("USAGE:\n\tCD.lock.exe \\\\.\\X: {+,-}\n"); return -1;}

	
	if (argv[2][0]=='+') act=TRUE;				// INCREASE LOCK COUNTER
		else if (argv[2][0]=='-') act=FALSE;	// DECREASE LOCK COUNTER
			else {printf(stderr,"-ERR: in arg %c\n", argv[2][0]); return -1;}

	// GET DEVICE DESCRIPTOR
	hCD=CreateFile(argv[1],GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
	if (hCD == INVALID_HANDLE_VALUE) {printf("-ERR: get CD-ROM\n");return -1;}

	// LOCK/UNLOCK CD-ROM TRAY
	pmrLockCDROM.PreventMediaRemoval = act;
	printf("try to %s drive %s...",act?"LOCK":"UNLOCK", argv[1]);
	a = DeviceIoControl (hCD, IOCTL_CDROM_MEDIA_REMOVAL,
					&pmrLockCDROM, sizeof(pmrLockCDROM), NULL, 0, &xxxx, NULL);
	if (a) printf("+OK\n"); else printf("-ERR:\x7\n");
}
