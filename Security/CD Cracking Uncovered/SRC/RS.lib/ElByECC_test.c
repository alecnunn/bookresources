#include "ElByECC.h"



#define FILE_SECTOR_MODE1		"MODE1.good.dat"
#define FILE_SECTOR_MODE2FORM1  "MODE2Form1.good.dat"

#define PUT_ERR_MIN	6
#define PUT_ERR_MAX 666
main()
{
	
    int		a;                   	
	RLBA	lba;
	ULBA	ulba;
    FILE	*sector_mode1;
    FILE	*sector_mode2form1;

	struct	BCD 	bcd;
	struct	MSF 	msf;

	struct	RAW_SECTOR_MODE1	raw_sector_mode1;
	struct	RAW_SECTOR_MODE1	raw_sector_mode1_damaged;
	struct	RAW_SECTOR_MODE1	raw_sector_mode1_new;

	struct	RAW_SECTOR_MODE2FORM1	raw_sector_mode2form1;
	struct	RAW_SECTOR_MODE2FORM1	raw_sector_mode2form1_damaged;
	struct	RAW_SECTOR_MODE2FORM1	raw_sector_mode2form1_new;
		
	printf("ElbyCloneCD KERNEL enginere gate by Kris Kaspersky\n");
	printf("ElbyECC library testing...\n\n");

	// иницилизация
	sector_mode1 = fopen(FILE_SECTOR_MODE1, "rb");
	if ( sector_mode1== 0)
		{ fprintf(stderr,"-ERR: open file %s\x7\n",FILE_SECTOR_MODE1); return -1;}
	else
		{ fprintf(stderr,"+OK: open file %s\n",FILE_SECTOR_MODE1);}
		
	a = fread(&raw_sector_mode1, 1, sizeof(raw_sector_mode1), sector_mode1);
	if (a != sizeof(raw_sector_mode1))
		{ fprintf(stderr, "-ERR: read file %s\x7\n",FILE_SECTOR_MODE1); return -1;}
	else
	    { fprintf(stderr,"+OK: read file %s\n",FILE_SECTOR_MODE1);}

    //
	sector_mode2form1 = fopen(FILE_SECTOR_MODE2FORM1, "rb");
	if ( sector_mode2form1== 0)
		{ fprintf(stderr,"-ERR: open file %s\x7\n",FILE_SECTOR_MODE2FORM1); return -1;}
	else
		{ fprintf(stderr,"+OK: open file %s\n",FILE_SECTOR_MODE2FORM1);}
		
	a = fread(&raw_sector_mode2form1, 1, sizeof(raw_sector_mode2form1), sector_mode2form1);
	if (a != sizeof(raw_sector_mode2form1))
		{ fprintf(stderr, "-ERR: read file %s\x7\n",FILE_SECTOR_MODE2FORM1); return -1;}
	else
	    { fprintf(stderr,"+OK: read file %s\n",FILE_SECTOR_MODE2FORM1);}


	print_sector(&raw_sector_mode1);
	print_sector(&raw_sector_mode2form1);

	printf("\n");

	// +
	printf("= test BCDtoMSF   @ %02Xh:%02Xh:%02Xh -> ", bcd.M = 0x16, bcd.S = 0x11, bcd.F = 0x10);
	BCDtoMSF(&bcd, &msf);
	printf("%d:%d:%d\n",msf.M, msf.S, msf.F, bcd.F = bcd.S =bcd.M = 0);

	// +
	printf("= test MSFtoBCD   @ %d:%d:%d    -> ", msf.M, msf.S, msf.F); 
    MSFtoBCD(&msf, &bcd);
    printf("%02Xh:%02Xh:%02Xh\n",bcd.M, bcd.S, bcd.F);

    // +
    printf("= test LBAtoMSF   @ %d         -> ", lba = -75);
    LBAtoMSF(lba, &msf);
    printf("%02d:%02d:%02d\n",msf.M, msf.S, msf.F);

    // +
    printf("= test MSFtoLBA   @ %02d:%02d:%02d    -> ", msf.M, msf.S, msf.F, lba = 0);
    lba = MSFtoLBA(&msf);
    printf("%d\n",lba);

	// +
	printf("= test ULBAtoMSF  @ %d         -> ", ulba = 666);
	ULBAtoMSF(ulba, &msf);
	printf("%02d:%02d:%02d\n",msf.M, msf.S, msf.F);

    // +
    printf("= test MSFtoULBA  @ %02d:%02d:%02d    -> ", msf.M, msf.S, msf.F, lba = 0);
    ulba = MSFtoULBA(&msf);
    printf("%d\n",ulba);

    // +
    printf("= test LBAtoBCD   @ %d         -> ", lba = -10);
    LBAtoBCD(lba, &bcd);
    printf("%02Xh:%02Xh:%02Xh\n",bcd.M, bcd.S, bcd.F);

    // +
    printf("= test BCDtoLBA   @ %02Xh:%02Xh:%02Xh -> ", bcd.M, bcd.S, bcd.F, lba = 0);
    lba = BCDtoLBA(&bcd);
    printf("%d\n",lba);

    // +
    printf("= test ULBAtoBCD  @ %d         -> ", ulba = 666);
    ULBAtoBCD(ulba, &bcd);
    printf("%02Xh:%02Xh:%02Xh\n",bcd.M, bcd.S, bcd.F);

    // +
    printf("= test BCDtoULBA  @ %02Xh:%02Xh:%02Xh -> ", bcd.M, bcd.S, bcd.F, ulba = 0);
    ulba = BCDtoULBA(&bcd);
    printf("%d\n",ulba);

    printf("\n");

	// ++++
    printf("= test CheckECCAndEDC_Mode1 @ good (TEST_ONLY) -> ");
    a = CheckECCAndEDC_Mode1(&raw_sector_mode1, ElBy_TEST_ONLY);
    printf("%s\n",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");

    // +
    printf("= test CheckECCAndEDC_Mode1 @ damaged (TEST_ONLY) -> ");
    memcpy(&raw_sector_mode1_damaged, &raw_sector_mode1, sizeof(raw_sector_mode1));
    for (a = 0; a < PUT_ERR_MIN; a++) raw_sector_mode1_damaged.USER_DATA[a] ^= 0xFF;
    a = CheckECCAndEDC_Mode1(&raw_sector_mode1_damaged, ElBy_TEST_ONLY);
    printf("%s\n",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");

    // +
	printf("= test CheckECCAndEDC_Mode1 @ damaged (REPAIR) -> ");    
    a = CheckECCAndEDC_Mode1(&raw_sector_mode1_damaged, ElBy_REPAIR);
    printf("%s",(a == ElBy_SECTOR_OK)?"SECTOR REPAIR OK":"SECTOR REPAIR ERROR");
    printf(" (%s)\n",(memcmp(&raw_sector_mode1_damaged, &raw_sector_mode1,
    			sizeof(raw_sector_mode1)) == 0)?"REALLY REPAIRED":"NOT REPAIRED YET");

    	
	// +++
    printf("= test CheckECCAndEDC_Mode2Form1 @ good (TEST_ONLY) -> ");
    a = CheckECCAndEDC_Mode2Form1(&raw_sector_mode2form1, ElBy_TEST_ONLY);
    printf("%s\n",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");

    // +
    printf("= test CheckECCAndEDC_Mode2Form1 @ damaged (TEST_ONLY) -> ");
    memcpy(&raw_sector_mode2form1_damaged, &raw_sector_mode2form1, sizeof(raw_sector_mode2form1));
    for (a = 0; a < PUT_ERR_MIN; a++) raw_sector_mode2form1_damaged.USER_DATA[a] ^= 0xFF;
    a = CheckECCAndEDC_Mode2Form1(&raw_sector_mode2form1_damaged, ElBy_TEST_ONLY);
    printf("%s\n",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");

    // +
	printf("= test CheckECCAndEDC_Mode2Form1 @ damaged (REPAIR) -> ");    
    a = CheckECCAndEDC_Mode2Form1(&raw_sector_mode2form1_damaged, ElBy_REPAIR);
    printf("%s",(a == ElBy_SECTOR_OK)?"SECTOR REPAIR OK":"SECTOR REPAIR ERROR");
    printf(" (%s)\n",(memcmp(&raw_sector_mode2form1_damaged, &raw_sector_mode2form1,
    			sizeof(raw_sector_mode2form1)) == 0)?"REALLY REPAIRED":"NOT REPAIRED YET");


	// +++
    printf("= test CheckSector    @ good MODE1 (TEST_ONLY) -> ");
    a = CheckSector((struct RAW_SECTOR*) &raw_sector_mode1, ElBy_TEST_ONLY);
    printf("%s\n",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");

    // +
    printf("= test CheckSector    @ good MODE2FORM1 (TEST_ONLY) -> ");
    a = CheckSector((struct RAW_SECTOR*) &raw_sector_mode2form1, ElBy_TEST_ONLY);
    printf("%s\n",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");

    // +
    printf("= test CheckSector   @ damaged MODE1 (TEST_ONLY) -> ");
    memcpy(&raw_sector_mode1_damaged, &raw_sector_mode1, sizeof(raw_sector_mode1));
    for (a = 0; a < PUT_ERR_MIN; a++) raw_sector_mode1_damaged.USER_DATA[a] ^= 0xFF;
    a = CheckSector((struct RAW_SECTOR*)&raw_sector_mode1_damaged, ElBy_TEST_ONLY);
    printf("%s\n",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");

    // +
    printf("= test CheckSector   @ damaged MODE2FORM1 (TEST_ONLY) -> ");
    memcpy(&raw_sector_mode2form1_damaged, &raw_sector_mode2form1, sizeof(raw_sector_mode2form1));
    for (a = 0; a < PUT_ERR_MIN; a++) raw_sector_mode2form1_damaged.USER_DATA[a] ^= 0xFF;
    a = CheckSector((struct RAW_SECTOR*)&raw_sector_mode2form1_damaged, ElBy_TEST_ONLY);
    printf("%s\n",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");

    // +
	printf("= test CheckSector @ damaged MODE1 (REPAIR) -> ");    
    a = CheckSector((struct RAW_SECTOR*)&raw_sector_mode1_damaged, ElBy_REPAIR);
    printf("%s",(a == ElBy_SECTOR_OK)?"SECTOR REPAIR OK":"SECTOR REPAIR ERROR");
    printf(" (%s)\n",(memcmp(&raw_sector_mode1_damaged, &raw_sector_mode1,
    			sizeof(raw_sector_mode1)) == 0)?"REALLY REPAIRED":"NOT REPAIRED YET");

	// +
	printf("= test CheckSector @ damaged MODE2FORM1 (REPAIR) -> ");    
    a = CheckSector((struct RAW_SECTOR*)&raw_sector_mode2form1_damaged, ElBy_REPAIR);
    printf("%s",(a == ElBy_SECTOR_OK)?"SECTOR REPAIR OK":"SECTOR REPAIR ERROR");
    printf(" (%s)\n",(memcmp(&raw_sector_mode2form1_damaged, &raw_sector_mode2form1,
    			sizeof(raw_sector_mode2form1)) == 0)?"REALLY REPAIRED":"NOT REPAIRED YET");

	// +++
    printf("= test CheckSectors    @ good MODE1 (TEST_ONLY) -> ");
    a = CheckSectors((struct RAW_SECTOR*) &raw_sector_mode1, RAW_SECTOR_SIZE, 1, ElBy_TEST_ONLY);
    printf("%s\n",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");

    // +
    printf("= test CheckSectors   @ damaged MODE1 (TEST_ONLY) -> ");
    memcpy(&raw_sector_mode1_damaged, &raw_sector_mode1, sizeof(raw_sector_mode1));
    for (a = 0; a < PUT_ERR_MIN; a++) raw_sector_mode1_damaged.USER_DATA[a] ^= 0xFF;
    a = CheckSectors((struct RAW_SECTOR*)&raw_sector_mode1_damaged, RAW_SECTOR_SIZE, 1, ElBy_TEST_ONLY);
    printf("%s\n",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");


	// +++
	printf("= test IdentifyTrack @ MODE1 -> ");
	a = IdentifyTrack((struct RAW_SECTOR*)&raw_sector_mode1);
	printf("%s\n",ElBy_TRACK_TEXT(a));

	// +
	printf("= test IdentifyTrack @ MODE2FORM1 -> ");
	a = IdentifyTrack((struct RAW_SECTOR*)&raw_sector_mode2form1);
	printf("%s\n",ElBy_TRACK_TEXT(a));


    // BUILD SECTOR (GET USER_DATA, HEADER AND GENERATE RS-CODE)
    // =======================================================================

	// +++
  	printf("= test GenECCAndEDC_Mode1 @ -> ");
	a = GenECCAndEDC_Mode1(&raw_sector_mode1.USER_DATA[0], &raw_sector_mode1.ADDR,&raw_sector_mode1_new);
	printf("%s",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");
    printf(" (%s)\n",(memcmp(&raw_sector_mode1, &raw_sector_mode1_new,
    			sizeof(raw_sector_mode1)) == 0)?"REALLY OK":"NOT GOOD");

    // +
  	printf("= test GenECCAndEDC_Mode2Form1 @  -> ");
	a = GenECCAndEDC_Mode2Form1(&raw_sector_mode2form1.USER_DATA[0], &raw_sector_mode2form1.ADDR,
	&raw_sector_mode2form1.subheader, &raw_sector_mode2form1_new);
	printf("%s",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");
    printf(" (%s)\n",(memcmp(&raw_sector_mode2form1, &raw_sector_mode2form1_new,
    			sizeof(raw_sector_mode2form1)) == 0)?"REALLY OK":"NOT GOOD");

    // ++++
    printf("= test  GenHeadAndSyc_4 @ BUILD %02d ->",ulba  = 10);
    memset(&raw_sector_mode1, 0, sizeof(raw_sector_mode1));
    GenHeadAndSyc_4_RawDatenLeadin_0Sect(&raw_sector_mode1,1, ulba, 1);
    printf(" ... ");for (a = 8; a < 16; a++) printf("%02X ",((unsigned char *)&raw_sector_mode1)[a]); printf("\n");
	

	// +++++
	printf(" = test GenerateSector @ ZERO ->");
	GenerateSector((struct RAW_SECTOR*)&raw_sector_mode1, 0, 0x10, &raw_sector_mode1.USER_DATA);

//extern GenerateSector(struct RAW_SECTOR *sector, int DO, RLBA lba,  char *user_data);

}

