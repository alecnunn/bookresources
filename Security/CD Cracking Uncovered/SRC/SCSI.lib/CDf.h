cd_raw_sector_read(char *CD, char *buf, int buf_size, int start_sec, int n_sec, int flags)
{
	
	unsigned char CDB[ATAPI_CDB_SIZE];
	memset(CDB, 0, ATAPI_CDB_SIZE);

	CDB[0] = READ_CD;
	CDB[1] = 0x0; 		// any CD

						// first sector number
	CDB[2]	= HIBYTE(HIWORD(start_sec));
	CDB[3]	= LOBYTE(HIWORD(start_sec));
	CDB[4]	= HIBYTE(LOWORD(start_sec));
	CDB[5]	= LOBYTE(LOWORD(start_sec));

						// number of sectors to be read
	CDB[6]	= LOBYTE(HIWORD(n_sec));
	CDB[7]	= HIBYTE(LOWORD(n_sec)); 
	CDB[8]	= LOBYTE(LOWORD(n_sec)); 

	CDB[9]	= flags;	// what to read?

	return SEND_SCSI_CMD(CD, CDB, ATAPI_CDB_SIZE, NO_SENSE, buf, buf_size, SCSI_DATA_IN);
}


cd_raw_toc_read(char *CD, char *buf, int buf_size, int flags)
{
	long a;
	unsigned char CDB[ATAPI_CDB_SIZE];	// SCSI CDB block for SCSI/ATAPI devices

	// buffer initialization
	memset(CDB, 0, ATAPI_CDB_SIZE); memset(buf, 0, buf_size);

	// preparing CDB block
	CDB[0] = READ_TOC;    					// READ TOC
	CDB[2] = LOBYTE(flags);					// what to read
	CDB[6] = HIBYTE(flags);					// number of the first session
	CDB[7] = HIBYTE(buf_size);				// buffer...
	CDB[8] = LOBYTE(buf_size);				//			...size

	// sending CDB block to SCSI/ATAPI device
	return  SEND_SCSI_CMD(CD, CDB, ATAPI_CDB_SIZE, NO_SENSE,buf, buf_size, SCSI_DATA_IN);
}