/******************************************************************************

Coral Tree osd/system.h
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

#ifndef OSD_SYSTEM_H
#define OSD_SYSTEM_H

/* remove me */
//#define osdStrdup strdup

/* OSD_OSTYPE */
#define OSD_UNIX			1
#define OSD_WIN32			2

/* OSD_OS */
#define OSD_WINNT			1
#define OSD_WIN95			2
#define OSD_WIN31			3
#define OSD_IRIX			4
#define OSD_LINUX			5
#define OSD_SUNOS			6
#define OSD_HPUX			7
#define OSD_NECEWS			8
#define OSD_FBSD			9
#define OSD_LYNX			10
#define OSD_SCO				11

/* OSD_OSVER */
/* use the primary version number */

/* OSD_OSMINORVER */
/* use the minor version number */

/* OSD_COMPILER */
#define OSD_ANSI			1
#define OSD_KR				2
#define OSD_GNU				3
#define OSD_VISUAL			4

/* OSD_CHIP */
#define OSD_SPARC			1
#define OSD_X86				2
#define OSD_AXP				3
#define OSD_HPPA			4
#define OSD_MIPS			5
#define OSD_68K				6

/* OSD_ENDIAN */
#define OSD_LITTLE			1
#define OSD_BIG				2

/* OSD_THREADS */
#define OSD_CTHREAD_PKG		1
#define OSD_SGI_SPROC		2
#define OSD_WIN32_THREADS	3
#define OSD_PTHREADS		4
#define OSD_NO_THREADS		5

/* OSD_VERBOSITY */
#define	OSD_DEBUG			1
#define OSD_NORMAL			2
#define OSD_SILENT			3

#ifndef OSD_VERBOSITY
#define OSD_VERBOSITY 		OSD_NORMAL
#endif

#include <meta/platform.h>

/* THIS HAS TO BE AFTER meta/platform.h */
#ifdef __cplusplus
#if OSD_COMPILER==OSD_KR
#undef OSD_COMPILER
#define OSD_COMPILER OSD_ANSI
#endif /* OSD_COMPILER==OSD_KR */
#endif /* __cplusplus */
#if OSD_HARDWARE==OSD_X86 || OSD_HARDWARE==OSD_AXP
#define OSD_ENDIAN			OSD_LITTLE
#else /* OSD_HARDWARE!=OSD_X86 && OSD_HARDWARE!=OSD_AXP */
#define OSD_ENDIAN			OSD_BIG
#endif /* OSD_HARDWARE!=OSD_X86 && OSD_HARDWARE!=OSD_AXP */

/******************************************************************************
STUPID DLL CRAP
******************************************************************************/
#ifdef CORAL_DLL_CLASS
#if OSD_OSTYPE==OSD_UNIX 
#elif OSD_OSTYPE==OSD_WIN32
#define class class __declspec(dllexport)
#endif
#endif


/******************************************************************************
INCLUDES
******************************************************************************/

/**************************** UNIX *******************************************/
#if OSD_OSTYPE==OSD_UNIX  /* begin Unix-specific section */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/utsname.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <dirent.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <math.h>
#include <syslog.h>
#include <sys/wait.h>

/*********************** HPUX ******************************************/
#if OSD_OS==OSD_HPUX
#define	MAP_FAILED	((void *)-1L)
#if OSD_OSVER==10
#include <errno.h>
#endif
#endif

/*********************** LYNX OS  **************************************/
#if OSD_OS==OSD_LYNX
#include <errno.h>
#include <crypt.h>
#include <malloc.h>
#endif

/*********************** SCO *******************************************/
#if OSD_OS==OSD_SCO
#include <sys/dir.h>
#include <prot.h>
#define OSD_REPLACE_SCANDIR
#include <errno.h>
#include <sys/param.h>
#include <sys/sysmacros.h>
#endif

/*********************** NEC *******************************************/
#if OSD_OS==OSD_NECEWS
#include <sys/ksym.h>
#include <sys/dir.h>
int alphasort(struct direct **d1, struct direct **d2);
#include <errno.h>
#include <sys/select.h>
#include <sys/filio.h>
#include <crypt.h>
#define OSD_REPLACE_SCANDIR
int alphasort(struct dirent **d1,struct dirent **d2);
#ifdef __cplusplus
#include <new.h>
extern "C"
{
	int select(int , fd_set *, fd_set *, fd_set *execptfds, struct timeval *);
}
#endif /* __cplusplus */
#endif /* OSD_NECEWS */

/*********************** SUNOS   ***************************************/
#if OSD_OS==OSD_SUNOS
#include <kvm.h>
#include <sys/filio.h>
	int alphasort();
#include <sys/wait.h>
#include <sys/systeminfo.h>
#endif /* OSD_SUNOS */

/*********************** SUNOS 4 ***************************************/
#if OSD_OS==OSD_SUNOS && OSD_OSVER==4
#include <sys/select.h>
#include <errno.h>
#ifdef __cplusplus
#include <new.h>
#endif /* __cplusplus */

#if OSD_COMPILER==OSD_GNU
#ifdef __cplusplus
	extern "C"
	{
		int msgget(key_t key,int msgflg);
		int msgsnd(int msqid,struct msgbuf *msgp,int msgsz,int msgflg);
		int msgrcv(int msqid,struct msgbuf *msgp,int msgsz,long msgtyp,int msgflg);
		int socket(int domain, int type, int protocol);
		int bind(int s,struct sockaddr *name,int namelen);
		int listen(int s,int backlog);
		int accept(int s, struct sockaddr *addr, int *addrlen);
		int connect(int s, struct sockaddr *name, int namelen);
		int getsockname(int s,struct sockaddr *name,int *namelen);
		void bzero(char *b,int length);
		int select(int width,fd_set *,fd_set *,fd_set *,struct timeval *timeout);
		int semget(key_t key,int nsems,int semflg);
		int semop(int semid, struct sembuf *sops, int nsops);
		int ioctl(int fd, int request, caddr_t arg);
		int shmget(key_t key, int size, int shmflg);
		char *shmat(int shmid,char *shmaddr,int shmflg);
		int shmdt(char *shmaddr);
		int send(int s, char *msg, int len, int flags);
		int sendto(int s,char *msg,int len,int flags,struct sockaddr *to,int tolen);
		int recv(int s, char *buf, int len, int flags);
		int recvfrom(int s, char *buf,int len,int flags, struct sockaddr *from, int *fromlen);
	}
#endif /* __cplusplus */
#endif /* OSD_GNU */
#endif /* OSD_SUNOS */

/*********************** SUNOS 5 ***************************************/
#if OSD_OS==OSD_SUNOS && OSD_OSVER==5
#include <errno.h>
#include <libintl.h>
#include <crypt.h>
#include <dlfcn.h>
#include <sys/param.h>
#define OSD_REPLACE_SCANDIR

/*
#if OSD_COMPILER!=OSD_GNU && defined __cplusplus
#include <new.h>
#endif
*/

#endif /* OSD_SUNOS 5 */

/*********************** IRIX *****************************************/
#if OSD_OS==OSD_IRIX

/* permit multi-threaded errno values */
#define _SGI_MP_SOURCE

#include <errno.h>
#include <bstring.h>
#include <sys/prctl.h>
#include <malloc.h>
#include <crypt.h>
#endif /* OSD_IRIX */

/*********************** LINUX *****************************************/
/*#if OSD_OS==OSD_LINUX && OSD_THREADS==OSD_PTHREADS*/

#if OSD_OS==OSD_LINUX
/*	#define	MAP_FAILED	((void *)-1L)  <--- this was for pre-egcs/glibc2 */
/*	#define OSD_REPLACE_SCANDIR*/
/*	int alphasort(struct dirent **d1,struct dirent **d2);*/
#include <errno.h>
#include <crypt.h>
#else
#include <nlist.h>
#endif

/*********************** FreeBSD ***************************************/
#if OSD_OS==OSD_FBSD

#include <errno.h>

#endif

/*********************** CTHREAD_PKG **********************************/
#if OSD_THREADS==OSD_CTHREAD_PKG
#include <cthread.h>
#endif

/*********************** C++ ******************************************/
#ifdef __cplusplus
extern "C"
{
	char *mktemp(char *);
	int close(int);
}
#endif /* __cplusplus */

#elif OSD_OSTYPE==OSD_WIN32 /* NT Specific */

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys\types.h>
#include <malloc.h>
#include <process.h>
#include <time.h>
#include <new.h>
#include "getopt.h"
#include <direct.h> /* for getcwd() */
#include <assert.h>
#include <memory.h>
#include <tchar.h>

#ifdef __INSIDE_OSD_H_EXTERN_C__
}
#include <math.h>
extern "C"
{
#else
#include <math.h>
#endif 

#endif

/******************************************************************************
TYPES
******************************************************************************/
typedef	char				char8;
typedef	char				int8;
typedef	unsigned char		uint8;
typedef	short				int16;
typedef	unsigned short		uint16;
#if OSD_HARDWARE==OSD_AXP
typedef	int					int32;
typedef	unsigned int		uint32;
typedef	long				int64;
typedef	unsigned long		uint64;
#else
typedef	long				int32;
typedef	unsigned long		uint32;
#if OSD_COMPILER==OSD_VISUAL
/*
typedef	hyper				int64;
typedef	unsigned hyper		uint64;
*/
typedef	__int64				int64;
typedef	unsigned __int64	uint64;
#else
typedef	long long			int64;
typedef	unsigned long long	uint64;
#endif
#endif
typedef	int					word;
typedef	unsigned int		uword;
typedef	float				real32;
typedef	double				real64;
typedef	long				OSD_Boolean;
typedef uint8				OSD_Byte;
typedef	char8				OSD_Character;
typedef	int32				OSD_Integer;
typedef	real32				OSD_Real;
typedef	uint32				OSD_Opaque;
typedef	void *				OSD_Address;

/**************************** UNIX *******************************************/
#if OSD_OSTYPE==OSD_UNIX
#define	OSD_HANDLE			int
#define OSD_FILE_HANDLE		int
#define OSD_ERROR_NUMBER	int
#define OSD_PID				pid_t
#define OSD_TID				pid_t
#define	OSD_KEY				key_t
#define	OSD_TIME			time_t

/**************************** WIN32 ******************************************/
#elif OSD_OSTYPE==OSD_WIN32
typedef long ssize_t;
#define OSD_HANDLE			HANDLE
#define OSD_FILE_HANDLE		HANDLE
#define OSD_ERROR_NUMBER	int
#define OSD_PID				long
#define OSD_TID				long
#define OSD_KEY				int
#define	OSD_TIME			SYSTEMTIME
#endif /* OSD_OSTYPE==OSD_WIN32 */

typedef struct {
	OSD_PID pid;
	OSD_TID tid;
} OSD_ExecutionId;

/******************************************************************************
DEFINES
******************************************************************************/
#ifndef	BITSPERBYTE
#define	OSD_BITS_PER_BYTE	8
#else
#define	OSD_BITS_PER_BYTE	BITSPERBYTE
#endif /* BITSPERBYTE */
#ifndef	BITSPERLONG
#define	OSD_BITS_PER_WORD	(OSD_BITS_PER_BYTE * sizeof(unsigned long))
#else
#define	OSD_BITS_PER_WORD	BITSPERLONG
#endif /* BITSPERLONG */
#define	OSD_BAD_ADDRESS	((void *) 0)
#define OSD_UNDEFINED_ERROR	0xFFF0

/******************************************************************************
Maximum Platform string len for osdPlatform()
******************************************************************************/
#define OSD_MAX_PLATFORM_LEN 64

/**************************** UNIX *******************************************/
#if OSD_OSTYPE==OSD_UNIX
#define FALSE						0
#define TRUE						1
#define	OSD_INVALID_HANDLE			-1
#define OSD_INVALID_FILE_HANDLE		-1
#define OSD_ERRNO				errno
#define OSD_MAX_COMMAND_LINE_LEN	259
#define OSD_DIRECTORY_SEPARATOR		'/'
#define OSD_INVALID_TID				-1

#if OSD_OS==OSD_FBSD
#define EPROTO					OSD_UNDEFINED_ERROR+1
#define	ENOSR					OSD_UNDEFINED_ERROR+2
#endif
#define	OSD_EWOULDBLOCK			EWOULDBLOCK
#define	OSD_EBADF				EBADF
#define	OSD_ENOTSOCK            ENOTSOCK
#define	OSD_EINTR               EINTR
#define	OSD_ENODEV              ENODEV
#define	OSD_EOPNOTSUPP          EOPNOTSUPP
#define	OSD_EPROTO              EPROTO
#define	OSD_ENOMEM              ENOMEM
#define	OSD_ENOSR               ENOSR
#define	OSD_EACCES				EACCES
#define	OSD_EADDRINUSE			EADDRINUSE
#define	OSD_EADDRNOTAVAIL		EADDRNOTAVAIL
#define	OSD_EAFNOSUPPORT		EAFNOSUPPORT
#define	OSD_ENOENT				ENOENT
#define	OSD_ECONNREFUSED		ECONNREFUSED
#define	OSD_EINPROGRESS			EINPROGRESS
#define	OSD_EINVAL				EINVAL
#define	OSD_EIO					EIO
#define	OSD_ENXIO				ENXIO
#define	OSD_ECONNRESET			ECONNRESET
#define	OSD_EISCONN				EISCONN
#define	OSD_ENETUNREACH			ENETUNREACH
#define	OSD_ETIMEDOUT			ETIMEDOUT
#define	OSD_EMFILE				EMFILE
#define	OSD_EPROTONOSUPPORT		EPROTONOSUPPORT
#define	OSD_ESTALE				ESTALE
#define	OSD_EMSGSIZE			EMSGSIZE

/**************************** WIN32 ******************************************/
#elif OSD_OSTYPE==OSD_WIN32
#define OSD_INVALID_HANDLE				0
#define OSD_INVALID_FILE_HANDLE			0
#define OSD_ERRNO					GetLastError()
#define OSD_MAX_WINDOW_STR_LEN			64
#define OSD_MAX_COMMAND_LINE_LEN		259
#define OSD_DIRECTORY_SEPARATOR			'\\'
#define OSD_INVALID_TID					-1

#define	OSD_EWOULDBLOCK			WSAEWOULDBLOCK
#define	OSD_EBADF				WSAEBADF
#define	OSD_ENOTSOCK            WSAENOTSOCK
#define	OSD_EINTR               WSAEINTR
#define	OSD_ENODEV              OSD_UNDEFINED_ERROR+0
#define	OSD_EOPNOTSUPP          WSAEOPNOTSUPP
#define	OSD_EPROTO              OSD_UNDEFINED_ERROR+1
#define	OSD_ENOMEM              OSD_UNDEFINED_ERROR+2
#define	OSD_ENOSR               OSD_UNDEFINED_ERROR+3
#define	OSD_EACCES				WSAEACCES
#define	OSD_EADDRINUSE			WSAEADDRINUSE
#define	OSD_EADDRNOTAVAIL		WSAEADDRNOTAVAIL
#define	OSD_EAFNOSUPPORT		WSAEAFNOSUPPORT
#define	OSD_ENOENT				OSD_UNDEFINED_ERROR+4
#define	OSD_ECONNREFUSED		WSAECONNREFUSED
#define	OSD_EINPROGRESS			WSAEINPROGRESS
#define	OSD_EINVAL				WSAEINVAL
#define	OSD_EIO					OSD_UNDEFINED_ERROR+5
#define	OSD_ENXIO				OSD_UNDEFINED_ERROR+6
#define	OSD_ECONNRESET			WSAECONNRESET
#define	OSD_EISCONN				WSAEISCONN
#define	OSD_ENETUNREACH			WSAENETUNREACH
#define	OSD_ETIMEDOUT			WSAETIMEDOUT
#define	OSD_EMFILE				WSAEMFILE
#define	OSD_EPROTONOSUPPORT		WSAEPROTONOSUPPORT
#define	OSD_ESTALE				WSAESTALE
#define	OSD_EMSGSIZE			WSAEMSGSIZE
#endif /* OSD_OSTYPE==OSD_WIN32 */

/******************************************************************************
MACROS
******************************************************************************/
/******************************************************************************
THERE IS A MUTUALLY EXCLUSIVE PROBLEM BETWEEN HP AND SGI WITH THE CASTING
OF SOME THINGS.  HP AND NT NEED IT WHILE SGI WARNS ITS NOT NECESSARY.  
******************************************************************************/
#if OSD_OS==OSD_HPUX || OSD_OSTYPE==OSD_WIN32
#define OSD_HELP_CAST(x) (x)
#else
#define OSD_HELP_CAST(x)
#endif

/**************************** UNIX *******************************************/
#if OSD_OSTYPE==OSD_UNIX
#define osdBeginSignalRedoLoop();			do{
#define osdEndSignalRedoLoop(if_error)		} while ((if_error) && (OSD_ERRNO == EINTR))
#define osdCWDGet(a,b)				getcwd(a,b)
#define osdSleep					sleep
#elif OSD_OSTYPE==OSD_WIN32
#define osdBeginSignalRedoLoop();			do{
#define osdEndSignalRedoLoop(if_error)		} while ((if_error) && (OSD_ERRNO == WSAEINTR))
#define osdCWDGet(a,b)				getcwd(a,b)
#define osdSleep(x)					Sleep(x*1000) 
#endif

#if OSD_OS==OSD_IRIX
#define	osdFastSquareRoot(x)		fsqrt(x)
#else
#define	osdFastSquareRoot(x)		sqrt(x)
#endif

#if (OSD_OS==OSD_SUNOS && OSD_OSVER==4 && OSD_COMPILER!=OSD_GNU) && defined __cplusplus
#define	osdMallocImpl(x)			malloc((unsigned int) x )
#define osdCallocImpl(x,y)			calloc((unsigned int) x, (unsigned int) y)
#define	osdFreeImpl(x)				free((char *) x)
#define osdMemsetImpl(x,y,z)		memset((char *) x, (int) y, (int) z)
#define osdMemcpyImpl(x,y,z)		memcpy((char *) x, (char *) y, (int) z)
#define osdMemcmpImpl(x,y,z)		memcmp((char *) x, (char *) y, (int) z)
#define osdMemmoveImpl(x,y,z)		bcopy((char *) y, (char *) x, (int) z)
#elif OSD_OS==OSD_HPUX
#define	osdMallocImpl(x)			malloc((size_t) x )
#define osdCallocImpl(x,y)			calloc((size_t) x, (size_t) y)
#define	osdFreeImpl(x)				free((void *) x )
#define osdMemsetImpl(x,y,z)		memset((void *) x, (int) y, (size_t) z)
#define osdMemcpyImpl(x,y,z)		memcpy((void *) x, (void *) y, (size_t) z)
#define osdMemcmpImpl(x,y,z)		memcmp((void *) x, (void *) y, (size_t) z)
#define osdMemmoveImpl(x,y,z)		memmove((void *) x, (void *) y, (size_t) z)
#else
#define	osdMallocImpl				malloc
#define osdCallocImpl				calloc
#define	osdFreeImpl					free
#define osdMemsetImpl				memset
#define osdMemcpyImpl				memcpy
#define osdMemcmpImpl				memcmp
#if (OSD_OS==OSD_SUNOS && OSD_OSVER==4)
#define osdMemmoveImpl(x,y,z)		bcopy(y, x, z)
#else
#define osdMemmoveImpl				memmove
#endif
#endif

#if 0
#define osdMalloc	osdMallocImpl
#define osdCalloc	osdCallocImpl
#define osdFree		osdFreeImpl
#define osdMemset	osdMemsetImpl
#define osdMemcpy	osdMemcpyImpl
#define osdMemcmp	osdMemcmpImpl
#define osdMemmove	osdMemmoveImpl
#define osdRealloc	osdReallocImpl
#endif

#ifdef __cplusplus
extern "C"
{
#endif

extern void * (*osdMallocImplPtr)(size_t);
extern void * (*osdCallocImplPtr)(size_t, size_t);
extern void * (*osdReallocImplPtr)(void *, size_t);
extern void   (*osdFreeImplPtr)(void *);
extern void * (*osdMemsetImplPtr)(void *, int, size_t);
extern void * (*osdMemcpyImplPtr)(void *, const void *, size_t);
extern int    (*osdMemcmpImplPtr)(const void *, const void *, size_t);
extern void * (*osdMemmoveImplPtr)(void *, const void *, size_t);

void *osdMallocDefault(size_t x);
void *osdCallocDefault(size_t x, size_t y);
void *osdReallocDefault(void *x, size_t y);
void  osdFreeDefault(void *x);
void *osdMemsetDefault(void *x, int y, size_t z);
void *osdMemcpyDefault(void *x, const void *y, size_t z);
int   osdMemcmpDefault(const void *x, const void *y, size_t z);
void *osdMemmoveDefault(void *x, const void *y, size_t z);

void *osdMalloc(size_t x);
void *osdCalloc(size_t x, size_t y);
void *osdRealloc(void *x, size_t y);
void  osdFree(void *x);
void *osdMemset(void *x, int y, size_t z);
void *osdMemcpy(void *x, const void *y, size_t z);
int   osdMemcmp(const void *x, const void *y, size_t z);
void *osdMemmove(void *x, const void *y, size_t z);

/******************************************************************************
GLOBAL BUFFER
******************************************************************************/
#define OSD_GLOBAL_BUFFER_SIZE	256
#define OSD_GLOBAL_BUFFER_COUNT	10
extern char OSD_GLOBAL_BUFFER[OSD_GLOBAL_BUFFER_COUNT][OSD_GLOBAL_BUFFER_SIZE];
extern char OSD_BUILDID[];

/******************************************************************************
PROTOTYPES
******************************************************************************/
long osdKill(OSD_PID pid);
OSD_PID osdSpawnve(char *path, char *argv[], char **env);
OSD_PID osdSpawnveAsUser(char *username, char *password, char *path, char *argv[], char **env);
OSD_Boolean osdExecute(char *command, int *status, char **output, int printStdout);
OSD_PID osdGetPID(void);
char *osdBasename(char *path);
void osdPathnameConvert(char *out_name, char *in_name);
OSD_Boolean osdUserSet(char *username, char *password);
OSD_Boolean osdAuthenticateUser(char *username, char *password);
OSD_Boolean osdUserGet(char *username, long n);
void osdSetLastError(OSD_ERROR_NUMBER err);
OSD_ERROR_NUMBER osdGetLastError(void);
OSD_Boolean osdCWDSet(char *path);
long osdTimer(void);
void osdMilliSleep(long milliseconds);
void *osdReallocImpl(void *ptr, unsigned long size);
OSD_Boolean osdGetEnvironmentVariable(char *name, char *value, long size);
double osdRoundInteger(double x);
char *osdPlatform(const char *fields);
char *osdStrdup(const char *str);
OSD_Boolean osdIsWin95(void);
#ifdef __cplusplus
}
#endif

#endif /* OSD_SYSTEM_H */
