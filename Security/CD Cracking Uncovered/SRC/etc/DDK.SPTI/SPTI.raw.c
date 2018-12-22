/*----------------------------------------------------------------------------
 *
 *				ЧТЕНИЕ СЕКТОРА С CD-ROM В СЫРОМ РЕЖИМЕ ЧЕРЕЗ SPTI
 *				=================================================
 *
 * build 0x001 @ 27.05.2003
----------------------------------------------------------------------------*/
#include <windows.h>
#include <stdio.h>
#include "SPTI.raw.h"

#define RAW_READ_CMD			0xBE	// ATAPI RAW READ

#define WHATS_READ				0xF8	// Sync & All Headers & User Data + EDC/ECC
#define PACKET_LEN				2352	// длина одного сектора

//#define WHATS_READ			0x10	// User Data
//#define PACKET_LEN			2048	// длина одного сектора

//-[DWORD READ_RAW_SECTOR_FROM_CD]---------------------------------------------
//		функция читает один или несколько секторов с CDROM в сыром (RAW) виде,
// согласно переданным флагам
//
//	ARG:
//		driver		-	что открывать (типа "\\.\X:")
//		adapter_id	-	номер шины (0 - primary, 1 - secondary)
//		read_id		-	номер устройства на шине (0 - master, 1 - slaeyer)
//		buf			-	буфер куда читать
//		buf_len		-	размер буфера в байтах
//		StartSector	-	с какого сектора читать, считая от нуля
//		N_SECTOR	-	сколько секторов читать \
//		flags		-	что читать (см. спецификацию на ATAPI)
//
//	RET:
//		!=0			-	функция завершилась успешно
//		==0			-	функция завершилась с ошибкой
//
//	NOTE:
//		работает только под NT/W2K/XP и требует прав администратора
//-----------------------------------------------------------------------------
DWORD READ_RAW_SECTOR_FROM_CD(char *driver, int adapter_id, int read_id,
char *buf, int buf_len, DWORD StartSector, DWORD N_SECTOR, BYTE flags)
{
	HANDLE				hCD;
	SCSI_PASS_THROUGH	srb;
	DWORD				returned, length, status;

	// ОТКРЫВАЕМ УСТРОЙСТВО
	//------------------------------------------------------------------------
	// внимание! не надо делать так "\\\\.\\SCSI0" или так "\\\\.\\CdRom0"
	// все равно не сработает! (это, кстати, частая ошибка начинающих)
	hCD = CreateFile (	driver, GENERIC_WRITE|GENERIC_READ,
						FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,0,0);
	if (hCD == INVALID_HANDLE_VALUE) return 0;

	// ФОРМИРУЕМ SRB
	//------------------------------------------------------------------------
	memset(&srb, 0, sizeof(SCSI_PASS_THROUGH));	// иницилизация

	// ОПЦИИ
	srb.Length				= sizeof(SCSI_PASS_THROUGH);
	srb.PathId				= 0;					// SCSI controller ID
	srb.TargetId			= adapter_id;			// target device ID
	srb.Lun 				= read_id;				// logical unit device ID
	srb.CdbLength			= 12;					// длина CDB пакета
	srb.SenseInfoLength		= 0;					// нам не нужна SenseInfo
	srb.DataIn 				= SCSI_IOCTL_DATA_IN;	// мы будем читать
	srb.DataTransferLength	= PACKET_LEN*N_SECTOR;	// сколько мы будем читать
	srb.TimeOutValue 		= 200;					// время выхода по TimeOut
	srb.DataBufferOffset	= buf;					// указатель на буфер
	srb.SenseInfoOffset 	= 0;					// SenseInfo на не нужна

	// CDB-пакет, содержащий команды ATAPI
	srb.Cdb[0]				= RAW_READ_CMD;			// читать сырой сектор
	srb.Cdb[1]				= 0x0;					// формат диска - любой
	
	// номер первого сектора для чтения, причем сначала передается старший
	// байт старшего слова, а потом младший байт младшего слова
	srb.Cdb[2]				= HIBYTE(HIWORD(StartSector));
	srb.Cdb[3]				= LOBYTE(HIWORD(StartSector));
	srb.Cdb[4]				= HIBYTE(LOWORD(StartSector));
	srb.Cdb[5]				= LOBYTE(LOWORD(StartSector));

	// количество секторов для чтения
	srb.Cdb[6]				= LOBYTE(HIWORD(N_SECTOR));
	srb.Cdb[7]				= LOBYTE(LOWORD(N_SECTOR));
	srb.Cdb[8]				= HIBYTE(LOWORD(N_SECTOR));

	srb.Cdb[9]				= flags;				// что читать
	srb.Cdb[10]				= 0;					// Sub-Channel Data Bits
	srb.Cdb[11]				= 0;					// reserverd

	// ОТПРАВЛЯЕМ SRB-блок ATAPI-устройству
	status = DeviceIoControl(hCD, IOCTL_SCSI_PASS_THROUGH,
			&srb, sizeof(SCSI_PASS_THROUGH), &srb, 0, &returned, FALSE);

	return 1;
}

#define FNAME	"raw.sector.dat"

main(int argc, char **argv)
{
	FILE *f;
	char *buf;

	if (argc<6)
	{
		fprintf(stderr,	"USAGE: SPTI.raw.exe \\\\.\\X: adapter_id device_id"\
						"start_sector N_sector\n"); return -1;
	}

	buf = malloc(PACKET_LEN*atol(argv[5]));
	if (READ_RAW_SECTOR_FROM_CD(argv[1], atol(argv[2]), atol(argv[3]),buf,
		PACKET_LEN*atol(argv[5]),atol(argv[4]),atol(argv[5]),WHATS_READ) != 0)
	{
		f=fopen(FNAME,"w");fwrite(buf,1,PACKET_LEN*atol(argv[5]),f);fclose(f);
	}

	return 0;
}