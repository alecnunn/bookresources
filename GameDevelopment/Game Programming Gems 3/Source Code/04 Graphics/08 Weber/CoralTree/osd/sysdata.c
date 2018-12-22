
/******************************************************************************

Coral Tree osd/sysdata.c
Copyright (C) 1998 Infinite Monkeys Incorporated

This program is free software; you can redistribute it and/or modify
it under the terms of the Artistic License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Artistic License for more details.

You should have received a copy of the Artistic License along with
this program (see meta/ART_LICE); if not, write to coral@imonk.com.

******************************************************************************/

#include <osd/osd.h>

#define	OSD_DEFAULT_LOAD	1.0
#define OSD_MAXIMUM_PROCESSES	200
#define OSD_PROCESSES_ALLOC 100

#if OSD_OS==OSD_SUNOS && OSD_OSVER==4
#define	KERNEL_FILE	"/vmunix"
#define	KMEM_FILE	"/dev/kmem"

#elif OSD_OS==OSD_SUNOS && OSD_OSVER==5
#define	KERNEL_FILE	NULL
#define	PROC_FS		"/proc"
static DIR *proc_dir;

#elif OSD_OS==OSD_IRIX
#define	KERNEL_FILE "/unix"
#define	KMEM_FILE	"/dev/kmem"
#define	PROC_FS "/proc"
#define	FSCALE 1024.0
static DIR *proc_dir;

#elif OSD_OS==OSD_FBSD
#define	KERNEL_FILE "/kernel"
#define	KMEM_FILE	"/dev/kmem"
#define	PROC_FS "/proc"
#define	FSCALE 1024.0
static DIR *proc_dir;

#elif OSD_OS==OSD_NECEWS
#define	KMEM_FILE	"/dev/kmem"
#define	PROC_FS "/proc"
#define	FSCALE 256.0
static DIR *proc_dir;

#elif OSD_OS==OSD_HPUX
#define	KERNEL_FILE "/hp-ux"
#define	KMEM_FILE	"/dev/kmem"
#define	PS_CMD	"ps -e 2>/dev/null | cut -c 1-6 2>/dev/null | grep -v PID 2>/dev/null"

#elif OSD_OS==OSD_LINUX
#define	LOADAVG_FILE	"/proc/loadavg"
#define	PROC_FS	"/proc"
static DIR *proc_dir;
static OSD_HANDLE lavg = OSD_INVALID_HANDLE;

#elif OSD_OS==OSD_WIN95 || OSD_OS==OSD_WINNT
#define MUTEX_NAME "_JFHGFHGF" 
#define RESERVED 0L
#define INITIAL_SIZE 40960L
#define EXTEND_SIZE 4096L
#define US_SLEEP 2000
#define	OSD_FIRST_OBJECT(x)	((PPERF_OBJECT_TYPE) ((PBYTE) (x) + (x)->HeaderLength))
#define OSD_NEXT_OBJECT(x)	((PPERF_OBJECT_TYPE) ((PBYTE) (x) +  (x)->TotalByteLength))
#define OSD_FIRST_COUNTER(x)	((PERF_COUNTER_DEFINITION *) ((PCHAR) (x) + (x)->HeaderLength))
#define	OSD_NEXT_COUNTER(x)	((PERF_COUNTER_DEFINITION *) ((PCHAR) (x) + (x)->ByteLength))
#define	OSD_FIRST_INSTANCE(x)	((PERF_INSTANCE_DEFINITION *) ((PBYTE) (x) + (x)->DefinitionLength))
#define	OSD_NEXT_INSTANCE(x)	((PERF_INSTANCE_DEFINITION *) ((PBYTE) (x) + (x)->ByteLength + ((PERF_COUNTER_BLOCK *) ((PBYTE) (x) + (x)->ByteLength))->ByteLength))
typedef struct
{
	char *name;
	DWORD obj_entry;
	DWORD index;
	PPERF_COUNTER_DEFINITION counter;
	PPERF_OBJECT_TYPE object;
} OSD_NameEntry;
OSD_NameEntry objectlist[] =
{ 
#define SYSTEM 0
	{"System", 0, 0, 0, 0},
#define THREAD 1 
	{"Thread", 0, 0, 0, 0},
#define PROCESS 2
	{"Process", 0, 0, 0, 0},
	{NULL}	
};
OSD_NameEntry counterlist[] =
{
#define PROCESSOR_QUEUE_LENGTH 0
	{"Processor Queue Length", SYSTEM, 0, 0, 0},
#define	ID_THREAD 1
	{"ID Thread", THREAD, 0, 0, 0},
#define ID_PROCESS_THREAD 2
	{"ID Process", THREAD, 0, 0, 0},
#define ID_PROCESS_PROCESS 3
	{"ID Process", PROCESS, 0, 0, 0},
	{NULL}
};
static float loadAverage = OSD_DEFAULT_LOAD;
static char requestString[1000];
static PPERF_DATA_BLOCK dataBlock;
#endif /* OSD_OS==... */

#if OSD_OS==OSD_SUNOS
static kvm_t *kmem = 0;
#elif OSD_OS==OSD_IRIX || OSD_OS==OSD_FBSD || OSD_OS==OSD_NECEWS || OSD_OS==OSD_HPUX
static OSD_HANDLE kmem = OSD_INVALID_HANDLE;
#endif /* OSD_OS==... */

#if OSD_OS==OSD_NECEWS
#define	OSD_KERNEL_SYMBOL	struct mioc_rksym
#else /* OSD_OS==OSD_NECEWS */
#define	OSD_KERNEL_SYMBOL	struct nlist
#endif /* OSD_OS==OSD_NECEWS */

#if OSD_OSTYPE==OSD_UNIX && OSD_OS!=OSD_LINUX
#define	LOADAV	0
#define	NPRO	1
OSD_KERNEL_SYMBOL symbols[] =
{
#if OSD_OS==OSD_SUNOS
	{"_avenrun"},
#elif OSD_OS==OSD_IRIX || OSD_OS==OSD_FBSD || OSD_OS==OSD_NECEWS || OSD_OS==OSD_HPUX
	{"avenrun"},
#endif /* OSD_OS==... */

#if OSD_OS==OSD_SUNOS && OSD_OSVER==4
	{"_nproc"},
#elif OSD_OS==OSD_HPUX
	{"nproc"},
#endif /* OSD_OS==... */

	{0}
};
#endif

#if OSD_OS==OSD_WIN95 || OSD_OS==OSD_WINNT
int osdBuildNameList(OSD_NameEntry *names);
int osdGetPerformanceData(IN PPERF_DATA_BLOCK *pPerfData);
int osdExtractCounterData(PPERF_DATA_BLOCK pPerf);
DWORD osdDataCollector(LPDWORD lpdwParam);
#elif OSD_OS==OSD_LINUX
#else /* OSD_OS!=OSD_WIN95 && OSD_OS!=OSD_WINNT */
static int osdSystemDataRead(OSD_KERNEL_SYMBOL *symbol,
								char *buf, uint32 nbytes);
#endif /* OSD_OS!=OSD_WIN95 && OSD_OS!=OSD_WINNT */

/***********************************************************************
int osdSystemDataInitialize(void);
***********************************************************************/
int osdSystemDataInitialize(void)
{
#if OSD_OS==OSD_SUNOS && OSD_OSVER==5
	DIR *proc_dir;
#elif OSD_OS==OSD_WIN95 || OSD_OS==OSD_WINNT
	DWORD dwThreadId;
	DWORD dwThreadParam = 1;
	HANDLE hThread;
	HANDLE hMutex;
	char string[10];
#endif /* OSD_OS==OSD_SUNOS && OSD_OSVER==5 */
#if OSD_OS!=OSD_LINUX
	int i;
#endif

#if OSD_OS==OSD_WIN95 || OSD_OS==OSD_WINNT
	osdBuildNameList(objectlist);
	osdBuildNameList(counterlist);
	requestString[0] = '\0';
	for (i = 0; NULL != objectlist[i].name; ++i)
	{
		sprintf(string, "%d ", objectlist[i].index);
		strcat(requestString, string);
	}
	hMutex = CreateMutex(NULL, FALSE, MUTEX_NAME);
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) osdDataCollector,
							&dwThreadParam, 0, &dwThreadId);
	if (NULL == hThread)
	{
		fprintf(stderr, "thread couldn't start\n");
		return -1;
	}
#else /* OSD_OS!=OSD_WIN95 && OSD_OS!=OSD_WINNT */
	/* Open kernel memory. */
#if OSD_OS==OSD_SUNOS
	kmem = kvm_open(KERNEL_FILE, NULL, NULL, O_RDONLY, "kvm_open");
	if ((kvm_t *) 0 == kmem)
	{
		fprintf(stderr, "could not open %s\n", KERNEL_FILE);
		return -1;
	}
#elif OSD_OS==OSD_IRIX || OSD_OS==OSD_FBSD || OSD_OS==OSD_NECEWS || OSD_OS==OSD_HPUX
	kmem = open(KMEM_FILE, O_RDONLY);
	if (OSD_INVALID_HANDLE == kmem)
	{
		fprintf(stderr, "could not open %s\n", KMEM_FILE);
		return -1;
	}
#endif /* OSD_OS==... */

	/* Read the kernel symbols. */
#if OSD_OS!=OSD_NECEWS && OSD_OS!=OSD_LINUX
#if OSD_OS==OSD_SUNOS
	if (-1 == kvm_nlist(kmem, symbols))
	{
		fprintf(stderr, "kvm_nlist() error\n");
		return -1;
	}
#else /* OSD_OS!=OSD_SUNOS */
	if (-1 == nlist(KERNEL_FILE, symbols))
	{
		fprintf(stderr, "nlist() error\n");
		return -1;
	}
#endif /* OSD_OS!=OSD_SUNOS*/
	for (i = 0; NULL != symbols[i].n_name; ++i)
	{
		if (0 == symbols[i].n_type || 0 == symbols[i].n_value)
		{
			fprintf(stderr, "could not get %s\n", symbols[i].n_name);
			return -1;
		}
	}
#endif /* OSD_OS!=OSD_NECEWS && OSD_OS!=OSD_LINUX */

	/* Open the load average file. */
#if OSD_OS==OSD_LINUX
	if (OSD_INVALID_HANDLE == (lavg = open(LOADAVG_FILE, O_RDONLY)))
	{
		fprintf(stderr, "could not open %s\n", LOADAVG_FILE);
		return -1;
	}
#endif /* OSD_OS==OSD_LINUX */

	/* Open the process directory. */
#ifdef PROC_FS
	if (NULL == (proc_dir = opendir(PROC_FS)))
	{
		fprintf(stderr, "could not open the directory %s\n", PROC_FS);
		return -1;
	}
#endif /* PROC_FS */
#endif /* OSD_OS!=OSD_WIN95 && OSD_OS!=OSD_WINNT */ 
	return 0;
}

/***********************************************************************
int osdSystemDataFinalize(void);
***********************************************************************/
int osdSystemDataFinalize(void)
{
#if OSD_OS==OSD_WINNT || OSD_OS==OSD_WIN95
	RegCloseKey(HKEY_PERFORMANCE_DATA);
#endif /* OSD_OS==OSD_WINNT || OSD_OS==OSD_WIN95 */
	return 0;
}

/***********************************************************************
int osdSystemDataGetLoad(float *load);
***********************************************************************/
int osdSystemDataGetLoad(float *load)
{
#if OSD_OS==OSD_LINUX
	char buffer[256];
#elif OSD_OS==OSD_WIN95 || OSD_OS==OSD_WINNT
	HANDLE hMutex;
#else /* OSD_OS!=OSD_LINUX */
	long value[3];
#endif /* OSD_OS!=OSD_LINUX */

	*load = OSD_DEFAULT_LOAD;
#if OSD_OS==OSD_LINUX
	if (-1 == lseek(lavg, 0, 0))
	{
		fprintf(stderr, "lseek() error\n");
		return -1;
	}
	if (-1 == read(lavg, buffer, 255))
	{
		fprintf(stderr, "read() error\n");
		return -1;
	}
	buffer[255] = 0;
	sscanf(buffer, "%f", load);
#elif OSD_OS==OSD_WIN95 || OSD_OS==OSD_WINNT
	hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, MUTEX_NAME);
	*load = loadAverage;
	CloseHandle(hMutex);
#else /* OSD_OS!=OSD_LINUX && OSD_OS!=OSD_WIN95 && OSD_OS!=OSD_WINNT */
	if (0 == osdSystemDataRead(&(symbols[LOADAV]), (char *) value,
								sizeof(value)))
	{
#if OSD_OS==OSD_HPUX
		*load = (float) value[1];
#else /* OSD_OS!=OSD_HPUX */
		*load = (float) value[0] / ((float) FSCALE);
#endif /* OSD_OS!=OSD_HPUX */
	}
#endif /* OSD_OS==... */
	return 0;
}

/***********************************************************************
int osdSystemDataThreadAlive(OSD_ExecutionId eid);
***********************************************************************/
int osdSystemDataThreadAlive(OSD_ExecutionId eid)
{
	OSD_PID pid;
	int alive = 0;
#if OSD_OS==OSD_WIN95 || OSD_OS==OSD_WINNT
	PERF_COUNTER_BLOCK *counter_block;
	PPERF_OBJECT_TYPE pThisObject;
	PPERF_INSTANCE_DEFINITION pThisInstance;
	HANDLE hMutex;
	char *pointer;
	long lThisInstance;
#else
	struct dirent *dir_struct;
	DIR *proc_dir;
#endif /* OSD_OS==... */

#if OSD_OS==OSD_SUNOS && OSD_OSVER==4
	alive = (NULL != kvm_getproc(kmem, eid.pid));
#elif OSD_OS==OSD_HPUX
	fprintf(stderr, "WARNING: osdSystemDataThreadAlive() not implemented\n");
#elif OSD_OS==OSD_WIN95 || OSD_OS==OSD_WINNT
	OSD_TID tid;
	hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, MUTEX_NAME);
	osdGetPerformanceData(&dataBlock);
	osdExtractCounterData(dataBlock);
	pThisObject = (PPERF_OBJECT_TYPE) counterlist[ID_THREAD].object;
	for (lThisInstance = 0, pThisInstance = OSD_FIRST_INSTANCE(pThisObject);
		lThisInstance < pThisObject->NumInstances;
		++lThisInstance, pThisInstance = OSD_NEXT_INSTANCE(pThisInstance))
	{
		counter_block = (PERF_COUNTER_BLOCK *)
						((PBYTE) pThisInstance + pThisInstance->ByteLength);

		pointer = (char *) counter_block +
					counterlist[ID_THREAD].counter->CounterOffset;
		tid = (OSD_TID) (*((DWORD *) pointer));
		pointer = (char *) counter_block +
					counterlist[ID_PROCESS_THREAD].counter->CounterOffset;
		pid = (OSD_PID) (*((DWORD *) pointer));
		if (eid.pid == pid)
		{
			alive = 1;
			break;
		}
	}
	CloseHandle(hMutex);
#else /* OSD_OS!=OSD_WIN95 && OSD_OS!=OSD_WINNT */
	rewinddir(proc_dir);
	while (NULL != (dir_struct = readdir(proc_dir)))
	{
		if ('0' <= dir_struct->d_name[0] && '9' >= dir_struct->d_name[0])
		{
			pid = atoi(dir_struct->d_name);
			if (pid == eid.pid)
			{
				alive = 1;
				break;
			}
		}
	}
#endif /* OSD_OS!=OSD_WIN95 && OSD_OS!=OSD_WINNT */
	return alive;
}

/***********************************************************************
int osdSystemDataThreadList(OSD_ExecutionId **eids, uint32 *n);
***********************************************************************/
#define OSD_PROC_REALLOC(i,a) while(i >= a){								\
	*eids = (OSD_ExecutionId *) osdRealloc((void *)(*eids),sizeof(OSD_ExecutionId) * (a +OSD_PROCESSES_ALLOC));												\
	if(*eids == NULL){fprintf(stderr,"memory allocation error\n");exit(-1);}\
	else {a+=OSD_PROCESSES_ALLOC;}}

int osdSystemDataThreadList(OSD_ExecutionId **eids, uint32 *n)
{
	int allocated;
#if OSD_OS==OSD_SUNOS && OSD_OSVER==4
	struct proc *proc_struct;
#elif OSD_OS==OSD_HPUX
	FILE *f;
	int r;
#elif OSD_OS==OSD_WIN95 || OSD_OS==OSD_WINNT
	PERF_COUNTER_BLOCK *counter_block;
	PPERF_OBJECT_TYPE pThisObject;
	PPERF_INSTANCE_DEFINITION pThisInstance;
	HANDLE hMutex;
	long lThisInstance;
	char *pointer;
#else
	struct dirent *dir_struct;
	DIR *proc_dir;
#endif /* OSD_OS==... */

#if OSD_OS==OSD_WIN95 || OSD_OS==OSD_WINNT
	hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, MUTEX_NAME);
	osdGetPerformanceData(&dataBlock);
	osdExtractCounterData(dataBlock);
	pThisObject = (PPERF_OBJECT_TYPE) counterlist[ID_THREAD].object;
	*eids = (OSD_ExecutionId *) osdMalloc(sizeof(OSD_ExecutionId) * pThisObject->NumInstances);
#else /* OSD_OS!=OSD_WIN95 && OSD_OS!=OSD_WINNT */
	/* TODO: make this allocation adaptive to actual # of processes */
	*eids = (OSD_ExecutionId *) osdMalloc(sizeof(OSD_ExecutionId) * OSD_PROCESSES_ALLOC);
	allocated = OSD_PROCESSES_ALLOC;
#endif /* OSD_OS!=OSD_WIN95 && OSD_OS!=OSD_WINNT */
	if (NULL == *eids)
	{
		fprintf(stderr, "memory allocation error\n");
		return -1;
	}

#if OSD_OS==OSD_SUNOS && OSD_OSVER==4
	if (-1 == kvm_setproc(kmem))
	{
		fprintf(stderr, "error rewinding system process table\n");
		return -1;
	}

	for (*n = 0; NULL != (proc_struct = kvm_nextproc(kmem)); ++(*n))
	{
		OSD_PROC_REALLOC(*n, allocated);
		(*eids)[*n].pid = proc_struct->p_pid;
		(*eids)[*n].tid = 0;
	}
#elif OSD_OS==OSD_HPUX
	/* TODO: this routine is simply calling ps.  It would probably be */
	/* better (faster, use less processes, etc) to probe the kernel   */
	if (NULL == (f = popen(PS_CMD, "r")))
	{
		fprintf(stderr, "popen() error\n");
		return -1;
	}
	*n = 0;
	while (EOF != (r = fscanf(f, "%d", &((*eids)[*n].pid))))
	{
		OSD_PROC_REALLOC(*n, allocated);
		if (0 < r)
		{
			(*eids)[*n].tid = 0;
			++(*n);
		}
	}
	pclose(f);
#elif OSD_OS==OSD_WIN95 || OSD_OS==OSD_WINNT
	for (*n = 0, lThisInstance = 0, pThisInstance = OSD_FIRST_INSTANCE(pThisObject);
		lThisInstance < pThisObject->NumInstances;
		++(*n), ++lThisInstance, pThisInstance = OSD_NEXT_INSTANCE(pThisInstance))
	{
		counter_block = (PERF_COUNTER_BLOCK *)
						((PBYTE) pThisInstance + pThisInstance->ByteLength);
		pointer = (char *) counter_block +
					counterlist[ID_THREAD].counter->CounterOffset;
		(*eids)[*n].tid = (OSD_TID) (*((DWORD *) pointer));
		pointer = (char *) counter_block +
					counterlist[ID_PROCESS_THREAD].counter->CounterOffset;
		(*eids)[*n].pid = (OSD_PID) (*((DWORD *) pointer));
	}
	CloseHandle(hMutex);
#else
	rewinddir(proc_dir);
	for (*n = 0; NULL != (dir_struct = readdir(proc_dir)); ++(*n))
	{
		OSD_PROC_REALLOC(*n, allocated);
		if ('0' <= dir_struct->d_name[0] && '9' >= dir_struct->d_name[0])
		{
			(*eids)[*n].pid = atoi(dir_struct->d_name);
			(*eids)[*n].tid = 0;
		}
	}
#endif /* OSD_OS==... */
	return 0;
}

/***********************************************************************
int osdSystemDataProcessList(OSD_PID **pids, uint32 *n);
***********************************************************************/
int osdSystemDataProcessList(OSD_PID **pids, uint32 *n)
{
	uint32 i;
	OSD_ExecutionId *eids;

	*n = 0;
	if (-1 == osdSystemDataThreadList(&eids, n))
	{
		return -1;
	}
	*pids = (OSD_PID *) osdMalloc(sizeof(OSD_PID) * (*n));
	for (i = 0; *n > i; ++i)
	{
		(*pids)[i] = eids[i].pid;
	}
	osdFree(eids);
	return *n;
}
/******************************************************************************
static int osdSystemDataRead(OSD_KERNEL_SYMBOL *symbol,
								char *buf, uint32 nbytes);
******************************************************************************/
#if OSD_OS!=OSD_LINUX
static int osdSystemDataRead(OSD_KERNEL_SYMBOL *symbol,
								char *buf, uint32 nbytes)
{
#if OSD_OS==OSD_SUNOS
	if (-1 == kvm_read(kmem, symbol->n_value, buf, nbytes))
	{
		fprintf(stderr, "error reading\n");
		return -1;
	}
#elif OSD_OS==OSD_IRIX || OSD_OS==OSD_FBSD || OSD_OS==OSD_HPUX
	if (-1 == lseek(kmem, symbol->n_value, 0))
	{
		fprintf(stderr, "lseek error\n");
		return -1;
	}
	if (-1 == read(kmem, buf, nbytes))
	{
		fprintf(stderr, "/dev/kmem read error\n");
		return -1;
	}
#elif OSD_OS==OSD_NECEWS
	symbol->mirk_buf = buf;
	symbol->mirk_buflen = nbytes;
	if (-1 == ioctl(kmem, MIOC_READKSYM, symbol))
	{
		fprintf(stderr, "ioctl kmem error\n");
		return -1;
	}
#endif /* OSD_OS==... */
	return 0;
}
#endif

#if OSD_OS==OSD_WIN95 || OSD_OS==OSD_WINNT
/***********************************************************************
int osdBuildNameList(OSD_NameEntry *names);
***********************************************************************/
int osdBuildNameList(OSD_NameEntry *names)
{
	LPTSTR lptstr;
	DWORD dwType;
	DWORD dwSize;
	int iIndex;
	int i;

	dwSize = 0;
	if (ERROR_SUCCESS != RegQueryValueEx(HKEY_PERFORMANCE_DATA,
										TEXT("Counter 009"), 0, &dwType,
										NULL, &dwSize))
/*	assert(lErr == ERROR_SUCCESS);*/
	{
		fprintf(stderr, "RegQueryValue() failed\n");
		return -1;
	}
	lptstr = osdMalloc(dwSize * sizeof(TCHAR));
	if (ERROR_SUCCESS != RegQueryValueEx(HKEY_PERFORMANCE_DATA,
										TEXT("Counter 009"), 0, &dwType,
										(LPBYTE) lptstr, &dwSize))
/*	assert(lErr == ERROR_SUCCESS);*/
	{
		fprintf(stderr, "RegQueryValue() failed\n");
		return -1;
	}

	while (*lptstr)
	{
		/* Get the index. */
		_stscanf(lptstr, TEXT(" %d"), &iIndex);
		lptstr += lstrlen(lptstr) + 1;

		/* Check the name. */
		for (i = 0; NULL != names[i].name; ++i)
		{
			if (0 == strcmp(names[i].name, lptstr))
			{
				names[i].index = (DWORD) iIndex;
			}
		}
		lptstr += lstrlen(lptstr) + 1;
	}

	return 0;
}

/***********************************************************************
int osdGetPerformanceData(IN PPERF_DATA_BLOCK *pPerfData);
***********************************************************************/
int osdGetPerformanceData(IN PPERF_DATA_BLOCK *pPerfData)
{
	DWORD size;
	DWORD type;

	if (NULL == *pPerfData &&
		NULL == (*pPerfData = osdMalloc(INITIAL_SIZE)))
	{
		fprintf(stderr, "memory allocation error\n");
		return -1;
	}

	while (TRUE)
	{
		size = _msize(*pPerfData);
		switch (RegQueryValueEx(HKEY_PERFORMANCE_DATA, requestString,
								RESERVED, &type, (LPBYTE) *pPerfData, &size))
		{
		case 0:
			if ((0 < size) &&
				(*pPerfData)->Signature[0] == (WCHAR)'P' &&
				(*pPerfData)->Signature[1] == (WCHAR)'E' &&
				(*pPerfData)->Signature[2] == (WCHAR)'R' &&
				(*pPerfData)->Signature[3] == (WCHAR)'F')
			{
				return 0;
			}
			break;
		case ERROR_MORE_DATA:
			if (NULL == (*pPerfData = osdRealloc(*pPerfData,
											_msize(*pPerfData) + EXTEND_SIZE)))
			{
				fprintf(stderr, "memory allocation error\n");
				return -1;
			}
			break;
		default:
			return -1;
		}
	}
}

/***********************************************************************
int osdExtractCounterData(PPERF_DATA_BLOCK pPerf);
***********************************************************************/
int osdExtractCounterData(PPERF_DATA_BLOCK pPerf)
{
	DWORD dwThisObject;
	PPERF_OBJECT_TYPE pThisObject;
	DWORD dwThisCounter;
	PPERF_COUNTER_DEFINITION pThisCounter;
	int i;

	for (dwThisObject = 0, pThisObject = OSD_FIRST_OBJECT(pPerf);
		dwThisObject < pPerf->NumObjectTypes;
		++dwThisObject, pThisObject = OSD_NEXT_OBJECT(pThisObject))
	{
		for (dwThisCounter = 0, pThisCounter = OSD_FIRST_COUNTER(pThisObject);
			dwThisCounter < pThisObject->NumCounters;
			++dwThisCounter, pThisCounter = OSD_NEXT_COUNTER(pThisCounter))
		{
			for (i = 0; NULL != counterlist[i].name; ++i)
			{
				if (counterlist[i].index == pThisCounter->CounterNameTitleIndex)
				{
					if (pThisObject->ObjectNameTitleIndex ==
							objectlist[counterlist[i].obj_entry].index)
					{
						counterlist[i].counter = pThisCounter;
						counterlist[i].object = pThisObject;
					}
				}
			}
		}
	}
	return 1;
}

/***********************************************************************
DWORD osdDataCollector(LPDWORD lpdwParam);
***********************************************************************/
DWORD osdDataCollector(LPDWORD lpdwParam)
{
	DWORD count;
	PERF_COUNTER_BLOCK *counter_block;
	HANDLE hMutex;
	char *pointer;

	loadAverage = OSD_DEFAULT_LOAD;
	while (TRUE)
	{
		hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, MUTEX_NAME);
		osdGetPerformanceData(&dataBlock);
		osdExtractCounterData(dataBlock);
		pointer = (char *) counterlist[PROCESSOR_QUEUE_LENGTH].counter;
		counter_block = (PERF_COUNTER_BLOCK *) pointer +
					counterlist[PROCESSOR_QUEUE_LENGTH].counter->ByteLength;
		pointer = (char *) counter_block +
					counterlist[PROCESSOR_QUEUE_LENGTH].counter->CounterOffset;
		count = *((DWORD *) (pointer));
		loadAverage = (float) 0.8 * loadAverage;
		loadAverage += (float) 0.2 * (float)count;
		CloseHandle(hMutex);
		SleepEx(US_SLEEP, FALSE);
	}
	return 0;
}
#endif /* OSD_OS==OSD_WIN95 || OSD_OS==OSD_WINNT */

