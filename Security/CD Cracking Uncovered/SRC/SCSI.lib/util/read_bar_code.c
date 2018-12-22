/*----------------------------------------------------------------------------
 *
 *									BAR CODE READER
 *									===============
 *
 *		Reads bar code from a CD
 *
 * build 0x001 @ 11.11.2003
---------------------------------------------------------------------------*/
#include <stdio.h>
#include "..\CD.h"
#include "..\SCSI.h"

#define BAR_CODE_SIZE	8	// (normal)
#define UPC_CODE_SIZE 	13	// (BCD)


//-[isReadBarCodeCapable]-----------------------------------------------------
//
//		This function determines if the drive supports the possibility of reading
// physical bar code located near the inner edge of the CD
//
//	ARG:
//		CD		- drive ("TEAC", "2.1", "\\\\.\\X:", "\\\\.\\CdRom0" è ò.ä.
//
//	RET:
//		== -1	- command execution error
//		==  0	- reading of bar codes not supported
//		==  1	- reading of bar codes is supported
//
//
// 	NOTE:
//		the function outputs debug messages to the console
//	
isReadBarCodeCapable(char *CD)
{
	int a;
	int len;
	char *p;
	unsigned char BUF[MAX_TOC_SIZE];
	unsigned char CDB[ATAPI_CDB_SIZE];

	memset(BUF, 0, ATAPI_CDB_SIZE);

	// determining if the drive can read BAR-CODE
	CDB[0] = 0x5A; // MODE SENSE
	CDB[2] = 0x2A; // Table 234 - C/DVD Capabilities and Mechanical Status Page Format
	CDB[7] = HIBYTE(MAX_TOC_SIZE);
	CDB[8] = LOBYTE(MAX_TOC_SIZE);

	a = SCSI_SEND_CMD(CD, CDB, ATAPI_CDB_SIZE, NO_SENSE, BUF, MAX_TOC_SIZE, SCSI_DATA_IN);
	if (a != SCSI_OK) {	printf("-ERR: MODE SENSE\x7\n"); return -1; }

	len = BUF[0]*0x100L + BUF[1];
	if (len < (8 + 8)){	printf("-ERR: non-STANDART SENSE INFO\x7\n");return -1;}

	p = &BUF[8];

	if (p[0] != 0x2A)
	{
		printf("-ERR: CD-ROM DEVICE MODE SENSE ERROR\x7\n");
		return -1;
	}

	return (p[5] & (1 << 7));
}


//-[isReadUPCCodeCapable]-----------------------------------------------------
//
//		This function determines if the drive can read 
// UPC/MCN bar code located in the Lead-In area
//
//	ARG:
//		CD		- drive ("TEAC", "2.1", "\\\\.\\X:", "\\\\.\\CdRom0" è ò.ä.
//
//	RET:
//		== -1	- command execution error
//		==  0	- reading of bar codes not supported
//		==  1	- reading of bar codes is supported
//
//
// 	NOTE:
//		the function outputs debug messages to the console
//	
isReadUPCCodeCapable(char *CD)
{
	int a;
	int len;
	char *p;
	unsigned char BUF[MAX_TOC_SIZE];
	unsigned char CDB[ATAPI_CDB_SIZE];

	memset(BUF, 0, ATAPI_CDB_SIZE);

	// defining if the drive can read BAR-CODE
	CDB[0] = 0x5A; // MODE SENSE
	CDB[2] = 0x2A; // Table 234 - C/DVD Capabilities and Mechanical Status Page Format
	CDB[7] = HIBYTE(MAX_TOC_SIZE);
	CDB[8] = LOBYTE(MAX_TOC_SIZE);

	a = SCSI_SEND_CMD(CD, CDB, ATAPI_CDB_SIZE, NO_SENSE, BUF, MAX_TOC_SIZE, SCSI_DATA_IN);
	if (a != SCSI_OK) {	printf("-ERR: MODE SENSE\x7\n"); return -1; }

	len = BUF[0]*0x100L + BUF[1];
	if (len < (8 + 8)){	printf("-ERR: non-STANDART SENSE INFO\x7\n");return -1;}

	p = &BUF[8];

	if (p[0] != 0x2A)
	{
		printf("-ERR: CD-ROM DEVICE MODE SENSE ERROR\x7\n");
		return -1;
	}

	return (p[5] & (1 << 6));
}



//-[ReadNativeBarCode]--------------------------------------------------------
//
//		This function reads the native (physical) bar code. Before calling it,
// it is recommended to make sure that the drive supports bar codes reading
// (not every drive supports reading of the native bar codes)
// 
//
//	ARG:
//		CD		- drive ("TEAC", "2.1", "\\\\.\\X:", "\\\\.\\CdRom0" etc.
//		dst		- pointer to 8-byte buffer, can be set to zero,
//				  in which case the function will only read the bar code, but won't
//				  copy it anywhere
//
//	RET:
//		== -1	- execution error (error messages are displayed on the console)
//		== 0	- no bar code, dst buffer remains unchanged
//		== 1	- bar code successfully copied into dst
ReadNativeBarCode(char *CD, char *dst)
{
	int a;
	int len;
	char *p;
	unsigned char BUF[MAX_TOC_SIZE];
	unsigned char CDB[ATAPI_CDB_SIZE];
	memset(BUF, 0, ATAPI_CDB_SIZE);

	// determining if the drive can read BAR-CODE
	CDB[0] = 0x51; // DISK INFO
	CDB[7] = HIBYTE(MAX_TOC_SIZE);
	CDB[8] = LOBYTE(MAX_TOC_SIZE);

	a = SCSI_SEND_CMD(CD, CDB, ATAPI_CDB_SIZE, NO_SENSE, BUF, MAX_TOC_SIZE, SCSI_DATA_IN);
	if (a != SCSI_OK){printf("-ERR: DISK INFO\x7\n"); return -1;}

	len = BUF[0]*0x100L + BUF[1]; if (len < 32)	return 0; // no bar code :-(
	p = &BUF[24];if (dst) memcpy(dst, p, 8); return 1;
}

//-[ReadUPCBarCode]-----------------------------------------------------------
//
//		This function reads UCP/MCN code written into the Lead-In area and
// readable by practically all drives. However, your burner program must
// support "swapping" native bar code into UCP/MCN code
//
//	ARG:
//		CD		- drive ("TEAC", "2.1", "\\\\.\\X:", "\\\\.\\CdRom0" etc.
//		dst		- pointer to 13-byte buffer,can be equal to zero,
//				  then function only determines the presence of the bar code,
//				  but doesn't copy it anywhere
//
//	RET:
//		== -1	- execution error (error messages go to console)
//		== 0	- no bar code, dst buffer remains unchanged
//		== 1	- bar code successfully copied to dst
ReadUPCBarCode (char *CD, char *dst)
{

	int a;
	int len;
	int MCVAL;
	int SubChannel_Data_Format_Code;
	unsigned char *p;
	unsigned char BUF[MAX_TOC_SIZE];
	unsigned char CDB[ATAPI_CDB_SIZE];

	memset(CDB, 0, ATAPI_CDB_SIZE);

	CDB[0] = 0x42; // READ SUB
	CDB[2] = 1 << 6;
	CDB[3] = 0x2; // UPC
	CDB[7] = 0x1; // LEN

	a = SEND_SCSI_CMD(CD, CDB, ATAPI_CDB_SIZE, NO_SENSE, BUF, MAX_TOC_SIZE, SCSI_DATA_IN);
	if (a != SCSI_OK) { 	printf("-ERR: SENS SCSI CMD\n"); return -1; }

	len = BUF[2]*0x100L + BUF[3];

	if (len < 20) {	printf("-ERR: non-standart Bar-Code or ISRC\x7\n");	return -1;}
	len -= 2; // subtract itself from the length

	p = &BUF[4];	// go to beginning of the reply
	MCVAL = p[4];
	if (MCVAL & (1<<7))	if (dst) memcpy(dst, &p[5], 13); else return 0;
	return 1;
}


#define argCD	((argc>1)?argv[1]:"TEAC")
main(int argc, char **argv)
{

	int a;
	unsigned char buf[13];
	memset(buf, '0', 13);

	// TITLE
	fprintf(stderr,"BAR CODE READER by Kris Kaspersky (kk@sendmail.ru) for Y.Haron\n");
	// can the drive read bar codes?
	a = isReadBarCodeCapable(argCD);
	if ( a == -1) {printf("-ERR: isReadBarCodeCapable\x7\n");return -1;}
	printf("Read Native  Bar Code Capable: %s\n", (a)?"YES, you have a good cd-rom":"NO,  facked off your cd-rom\n");
	if (a)		
	{			// read the native bar code, if possible

		a = ReadNativeBarCode(argCD, buf);
		printf("Native  Bar Code: ");
		if (a) for (a = 0; a < 8; a++) printf("%02X ", buf[a]);
			else
		printf("- - - - - - - -"); printf("\n");

	}

	// can the drive read UPC/MCN codes?
	a = isReadUPCCodeCapable(argCD);
	if ( a == -1) {printf("-ERR: isReadUPCCodeCapable\x7\n");return -1;}
	printf("Read UPC/MCN Bar Code Capable: %s\n", (a)?"YES, you have a good cd-rom":"NO,  facked off your cd-rom\n");
	if (a)		
	{			// read UPC/MCN bar code, if possible

		// read UPC/MCN/bar-code 
		a = ReadUPCBarCode(argCD, buf); printf("UPC/MCN Bar Code: ");
		if (a) for (a = 0; a < 13; a++) printf("%02X ", buf[a]);
			else
		printf("- - - - - - - - - - - - -"); printf("\n");
	}
		
}
