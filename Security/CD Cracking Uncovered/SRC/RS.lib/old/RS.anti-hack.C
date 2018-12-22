#include <stdio.h>
#include <windows.h>

#define SECHEADER_SIZE	4
#define RAWSECTOR_SIZE  2352

#define SYNC_SIZE		12
#define pSYNC			0

#define HEADER_SIZE		4
#define pHEADER			12

#define USER_DATA_SIZE	2048
#define pUSER_DATA		16

#define EDC_SIZE		4
#define pEDC			2064

#define INTERMEDIATE_SZ 8
#define pINTERMEDIATE	2068

#define P_party_SIZE			172
#define pP_party				2076

#define Q_party_SIZE			104
#define pQ_party				2248

#define CHECK_ONLY				0	
#define CHECK_AND_RECORVER		1

#define SECTOR_OK				1
#define NEED_RECORVER	0
#define SECTOR_ERROR	0


#define MAX_PSW_LEN	6
check_passwd()
{

	char buff[MAX_PSW_LEN];

	printf("enter password:"); fgets(buff, MAX_PSW_LEN, stdin);	
	if (strlen(buff)+1 == MAX_PSW_LEN) buff[strlen(buff)-1] = 0;
	if (strcmp(buff,"KPNC"))
	{
		printf("wrong password\n");	return 0;
	}
	else
	{
		printf("password ok\n"); return 1;
	}
}

#define MAX_A	13
DO_WORK()
{
	int a;
	for (a=0; a <= MAX_A; a++) printf("%d^2 = %d\n",a, a*a);
}

char EDC_ECC[RAWSECTOR_SIZE - pEDC];

main(int argc, char **argv)
{
	FILE *f;
	HANDLE h;
	unsigned char rawsector[RAWSECTOR_SIZE];

	int (__stdcall *GenECCAndEDC_Mode1)  (char *userdata, char *header, char *sector);
	int (__stdcall *CheckECCAndEDC_Mode1)  (char *userdata,  char *sector);

	h=LoadLibrary("ElbyECC.dll"); if (h==0) { printf("-ERR:LoadLibrary(\"ElbyECC.dll\") are fault!\n");return -1;}

	GenECCAndEDC_Mode1   = GetProcAddress(h,  "GenECCAndEDC_Mode1");
	CheckECCAndEDC_Mode1 = GetProcAddress(h,  "CheckECCAndEDC_Mode1");
		
	if ((argc > 1) && (!strcmp(argv[1],"/GEN")))
	{
		f=fopen("EDC_ECC.dat","wb");
		if (f)
		{
  			GenECCAndEDC_Mode1((char *) &check_passwd, (char *) &h, rawsector);
  			fwrite(&rawsector[pEDC], 1, RAWSECTOR_SIZE - pEDC, f); fclose(f);
  			printf("ECD_ECC addess is:%x\n",&EDC_ECC);	return 0;
		}
	}

	memcpy(&rawsector[pUSER_DATA],   &check_passwd, USER_DATA_SIZE);
	memcpy(&rawsector[pEDC],EDC_ECC, RAWSECTOR_SIZE - pEDC);

	if (CheckECCAndEDC_Mode1(rawsector, 1))
	{
		memcpy(&check_passwd, &rawsector[pUSER_DATA], USER_DATA_SIZE);
	}
	else
	{
		printf("-ERR: programm is damaged! check youself for virus, trojan of hacker\n");
		return -1;
	}

	if (check_passwd()==0) return 0;

	DO_WORK();
	
}
