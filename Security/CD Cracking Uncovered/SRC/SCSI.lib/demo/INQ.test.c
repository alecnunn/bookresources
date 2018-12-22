#include <stdio.h>
#include "..\CD.h"
#include "..\SCSI.h"

main()
{
	char LIST[100];
	ASPI_SEND_SCSI_INQUITY_EX(LIST, 0, 0, 5, 0, 0, 0);
	printf(LIST);



}