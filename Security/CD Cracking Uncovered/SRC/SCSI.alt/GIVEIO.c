/*----------------------------------------------------------------------------
 *
 *						SDRIVER ALLOWS EXECUTION
 *				OF IN/OUT MACHINE COMMANDS AT THE APPLICATION LEVEL
 *				====================================================
 *
 * 
 *
 * GIVEIO.SYS: by Dale Roberts
 * COMPILING: Use DDK BUILD
 * GOAL: To provide direct I/O access from the user mode
----------------------------------------------------------------------------*/

#include <ntddk.h>


/* Our device driver */
#define DEVICE_NAME_STRING L"giveio"

// IOPM structure is simply byte array of the size 0x2000, containing
// 8К * 8 bits == 64К bits IOPM, which cover the entire I/O address
// space of x86 processors. Every bit set to 0 provides access to appropriate
// port for user-mode process; every bit set to 1 denies access to I/O 
// through appropriate port.
#define IOPM_SIZE 0x2000
typedef UCHAR IOPM[IOPM_SIZE];


// array of zeroes that is copied into IOPM in TSS by means of calling
// dsKe386SetIoAccessMap()
// required memory is allocated in the course of driver loading
IOPM *IOPM_local = 0;

// two undocumented functions that we are going to use
// to provide access to I/O for the calling process
// * Ke386IoSetAccessMap()     - copies the passed access map into  TSS
// * Ke386IoSetAccessProcess() - changes the pointer of IOPM offset
//                               after which the copied map is used

void Ke386SetIoAccessMap(int, IOPM *);
void Ke386QueryIoAccessMap(int, IOPM *);
void Ke386IoSetAccessProcess(PEPROCESS, int);

// RELEASE ALL EARLIER ALLOCATED OBJECTS
VOID GiveioUnload(IN PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING uniDOSString;
	WCHAR DOSNameBuffer[] = L"\\DosDevices\\" DEVICE_NAME_STRING;

	if(IOPM_local) MmFreeNonCachedMemory(IOPM_local, sizeof(IOPM));
	RtlInitUnicodeString(&uniDOSString, DOSNameBuffer);
	IoDeleteSymbolicLink (&uniDOSString);
	IoDeleteDevice(DriverObject->DeviceObject);
}

//----------------------------------------------------------------------------
//	Setting IOPM  of the caller process so that to provide it full I/O access
//	Массив  IOPM_local[]  array is filled with zeroes
//	consequently IOPM will be reset to zero.
//	If OnFlag == 1, process is allowed access to I/O;
//	If it is equal to 0, access is denied.
//----------------------------------------------------------------------------
VOID SetIOPermissionMap(int OnFlag)
{
	Ke386IoSetAccessProcess(PsGetCurrentProcess(), OnFlag);
	Ke386SetIoAccessMap(1, IOPM_local);
}


void GiveIO(void)
{
	SetIOPermissionMap(1);
}

//----------------------------------------------------------------------------
//	Handler for user-mode call of CreateProcess().
//	This function is introduced into the I/O  calls table of the driver object
//	using the DriverEntry() function. When user-mode application calls CreateFile(), 
//	this function gets control in the context of the caller process, but 
//	with CPL set to 0.
//	This allows to carry out operations possible only in kernel mode.
//	GiveIO is called for providing the caller process access to I/O.
//	All that the user-mode application needs to do is to open this
//	device using  CreateFile()
//	No other actions are needed.
//----------------------------------------------------------------------------
NTSTATUS GiveioCreateDispatch(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp)
{
	GiveIO(); // give the calling process I/O access

	Irp->IoStatus.Information	= 0;
	Irp->IoStatus.Status		= STATUS_SUCCESS;

	IoCompleteRequest(Irp, IO_NO_INCREMENT); return STATUS_SUCCESS;
}


//----------------------------------------------------------------------------
//	Driver entry point. This procedure is called only once after loading
//  the driver. It allocates the required resources for driver operation
//	In our case it allocates the memory for IOPM array, and creates a device
//	that can be opened by the user-mode applciation.
//	It also creates a symbolic link to the device driver, which allows the
//	user-mode  application to get access to our driver using the
//	\\.\giveio notation.
//----------------------------------------------------------------------------
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS		status;
	PDEVICE_OBJECT	deviceObject;
	UNICODE_STRING	uniNameString, uniDOSString;

	WCHAR NameBuffer[]		= L"\\Device\\" DEVICE_NAME_STRING;
	WCHAR DOSNameBuffer[]	= L"\\DosDevices\\" DEVICE_NAME_STRING;

	// Allocate buffer for local IOPM and set it to zero.
	IOPM_local = MmAllocateNonCachedMemory(sizeof(IOPM));
	if(IOPM_local == 0) return STATUS_INSUFFICIENT_RESOURCES;
	RtlZeroMemory(IOPM_local, sizeof(IOPM));

	// Initialize the device driver and device object
	RtlInitUnicodeString(&uniNameString, NameBuffer);
	RtlInitUnicodeString(&uniDOSString, DOSNameBuffer);
	status = IoCreateDevice(DriverObject, 0, &uniNameString,FILE_DEVICE_UNKNOWN,
							0, FALSE, &deviceObject);
	if(!NT_SUCCESS(status)) return status;

	status = IoCreateSymbolicLink (&uniDOSString, &uniNameString);
	if (!NT_SUCCESS(status)) return status;

	// Initialize driver entry points in the driver object.
	// All we need is to use the Create and Unload operations
	DriverObject->MajorFunction[IRP_MJ_CREATE] = GiveioCreateDispatch;
	DriverObject->DriverUnload = GiveioUnload;

	return STATUS_SUCCESS;
}
