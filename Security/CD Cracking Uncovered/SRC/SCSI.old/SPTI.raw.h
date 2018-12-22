#define SCSI_IOCTL_DATA_IN		1 			// from ntddscsi.h
#define IOCTL_SCSI_PASS_THROUGH 0x4D004		// from ntddscsi.h
#define IOCTL_SCSI_PASS_THROUGH_DIRECT 0x4D014		// from ntddscsi.h4d014
#define IOCTL_SCSI_GET_ADDRESS 0x41018

typedef struct _SCSI_PASS_THROUGH {			// from ntddscsi.h
    USHORT Length;
    UCHAR ScsiStatus;
    UCHAR PathId;
    UCHAR TargetId;
    UCHAR Lun;
    UCHAR CdbLength;
    UCHAR SenseInfoLength;
    UCHAR DataIn;
    ULONG DataTransferLength;
    ULONG TimeOutValue;
    UCHAR* DataBufferOffset;
    ULONG SenseInfoOffset;
    UCHAR Cdb[16];
} SCSI_PASS_THROUGH, *PSCSI_PASS_THROUGH;

typedef struct _SCSI_ADDRESS {
    ULONG Length;
    UCHAR PortNumber;
    UCHAR PathId;
    UCHAR TargetId;
    UCHAR Lun;
}SCSI_ADDRESS, *PSCSI_ADDRESS;

