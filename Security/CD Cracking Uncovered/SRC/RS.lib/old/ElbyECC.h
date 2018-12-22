/*----------------------------------------------------------------------------
 *
 *				HEADER FILE OF THE ElbyECC LIBRARY
 *			created on the basis of its disassembling
 *			=========================================
 *
 * Build 0x001 @ 02.07.2003
----------------------------------------------------------------------------*/


// Compute EDC/ECC fields for user data and add SYNC+HEADER fields to it
// -----------------------------------------------------------------------
// userdata - inout data
// header - header (Sector address/mode)
// sector  - generated sector
// int __stdcall GenECCAndEDC_Mode1(char *userdata, int *header, char *sector);

// check EDC/ECC and recover userdata if necessary
// -----------------------------------------------------------------------
// rawsector - pointer to the sector which needs to be checked or recovered (src/dst)
// DO : 1 - recover, 0 - check only
// ret: 1 - sector OK/successfully recovered, 0 - sector is bad or irrecoverable
//CheckECCAndEDC_Mode1(char *rawsector , int DO);

// compute new EDC/ECC for userdata
//-------------------------------------
// rawsector - pointer to the sector
// ReGenECCAndEDC_Mode1(char *rawsector)


#define SECHEADER_SIZE			4
#define RAWSECTOR_SIZE			2352

#define SYNC_SIZE				12
#define pSYNC					0

#define HEADER_SIZE				4
#define pHEADER					12

#define USER_DATA_SIZE			2048
#define pUSER_DATA				16

#define EDC_SIZE				4
#define pEDC					2064

#define INTERMEDIATE_SZ			8
#define pINTERMEDIATE			2068

#define P_party_SIZE			172
#define pP_party				2076

#define Q_party_SIZE			104
#define pQ_party				2248

#define CHECK_ONLY				0	
#define CHECK_AND_RECORVER		1

#define SECTOR_OK				1
#define NEED_RECORVER			0
#define SECTOR_ERROR			0


#define LEN_SCREEN	80
print_hex(unsigned char *p, int len)
{
	int a;
	for (a = 0; a < len; a++)
	{
		printf("%02X ", p[a]);
		if ((a+1) % ((LEN_SCREEN - 1)/3) == 0) printf("\n");
	}printf("\n");
}

print_sector(char *rawsector)
{
	printf("\nSYNC:\n");      print_hex(&rawsector[pSYNC], SYNC_SIZE);
	printf("\nHEADER:\n");    print_hex(&rawsector[pHEADER], HEADER_SIZE);
	printf("\nUSER DATA:\n"); print_hex(&rawsector[pUSER_DATA], USER_DATA_SIZE);
	printf("\nEDC:\n");       print_hex(&rawsector[pEDC], EDC_SIZE);
	printf("\nINTERMEDIATE:\n");       print_hex(&rawsector[pINTERMEDIATE], INTERMEDIATE_SZ);
	printf("\nP Party:\n");       print_hex(&rawsector[pP_party], P_party_SIZE);
	printf("\nQ Party:\n");       print_hex(&rawsector[pQ_party], Q_party_SIZE);
}
