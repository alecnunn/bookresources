#include <windows.h>
#include <winioctl.h>  // From the Win32 SDK \Mstools\Include
#include <stdio.h>
#include "ntddcdrm.h"  // From the Windows NT DDK \Ddk\Src\Storage\Inc

#define X_SEC	2
AngleSector(char *drive, int from_sec, int to_sec, int N_SEC)
{ 
	HANDLE			hCD;
	LPBYTE			lpSector;
	DISK_GEOMETRY	dgCDROM;
	DWORD			a, w1,w2, dwNotUsed, dwSize;

   hCD = CreateFile (drive, GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,
                     NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);

   if (hCD == INVALID_HANDLE_VALUE) return -1;

   // Get CD sector size
   if (DeviceIoControl (hCD, IOCTL_CDROM_GET_DRIVE_GEOMETRY, NULL, 0,
   	 &dgCDROM, sizeof(dgCDROM),&dwNotUsed, NULL)==0) return -1;

    dwSize = X_SEC * dgCDROM.BytesPerSector;  // 2 sectors

    lpSector = VirtualAlloc (NULL, dwSize,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);

	printf("N SEC");
	for (a = 0; a < N_SEC; a++) printf("\t%d",a + from_sec); printf("\ntime");

	for (a = 0; a< N_SEC; a++)
	{
		w1=GetTickCount();
		SetFilePointer (hCD, dgCDROM.BytesPerSector * from_sec, NULL, FILE_BEGIN);         
		ReadFile (hCD, lpSector, dwSize, &dwNotUsed, NULL);

		SetFilePointer (hCD, dgCDROM.BytesPerSector * (a + to_sec), NULL, FILE_BEGIN);
		ReadFile (hCD, lpSector, dwSize, &dwNotUsed, NULL);
		w2=GetTickCount(); printf("\t%d",w2-w1);
	} printf("\n");

	VirtualFree (lpSector, 0, MEM_RELEASE);
	CloseHandle (hCD);

	return 0;
} 

main(int argc, char** argv)
{
	if (argc<5)
	{
		fprintf(stderr,"USAGE: CD.angle.sector.exe \\\\.\\X: from_sec, to_sec, N_SEC\n");
		return 0;
	}

	AngleSector(argv[1], atol(argv[2]), atol(argv[3]), atol(argv[4]));
}