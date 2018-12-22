/*-----------------------------------------------------------------------------
 *
 *					CYCLICALLY OPENS/CLOSES CD TRAY DOOR
 *					=======================================
 *
 * build 0x001 @ 08.06.2003
----------------------------------------------------------------------------*/
#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <math.h>

#define CRAZZY_DEFAULT 1
#define WIDE		70

main(int argc, char **argv)
{
	int a,b;
	int		crazzy_power = CRAZZY_DEFAULT;
	DWORD x;
	HANDLE h;

	if (argc<2)
	{
		printf("USAGE:CD.crazzy.exe \\\\.\\X: [N_TRY_TO_EJECT]\n");
   		printf("for example CD.crazzy.exe \\\\.\\G: 13\n");
   		return -1;		
	}

	h=CreateFile(argv[1],GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
	if (argc>2) crazzy_power = atol(argv[2]);

	for(a = 1; a<=crazzy_power; a++)
	{			 
        printf("\r%02d\\%02d [",a,crazzy_power);
        for (b=0;b<=(WIDE*a/crazzy_power);b++) printf(":");
        for (b=(WIDE*a/crazzy_power);b<WIDE;b++) printf(".");printf("]");

		DeviceIoControl(h,IOCTL_STORAGE_EJECT_MEDIA, 0,0,0,0,&x,0); Sleep(100);
		DeviceIoControl(h,IOCTL_STORAGE_LOAD_MEDIA, 0,0,0,0,&x,0);   Sleep(100);
	}
	printf("\n");
}