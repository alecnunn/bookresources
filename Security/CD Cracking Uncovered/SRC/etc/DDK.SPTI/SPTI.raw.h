#define SCSI_IOCTL_DATA_IN		1 			// from ntddscsi.h
#define IOCTL_SCSI_PASS_THROUGH 0x4D004		// from ntddscsi.h

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

