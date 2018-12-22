
/******************************************************************************

Coral Tree osd/signal.h
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

#if OSD_OS==OSD_SUNOS
#if OSD_OSVER==4
#define OSD_SignalHandler(x)	void x (...)
#else
#if OSD_OSMINORVER==4
#define OSD_SignalHandler(x)	void x ()
#else
#define OSD_SignalHandler(x)	void x (int)
#endif
#endif


#elif OSD_OS==OSD_IRIX
#if OSD_OSVER==5
#define OSD_SignalHandler(x)	void x (...)
#else
#define OSD_SignalHandler(x)	void x (int)
#endif


#elif OSD_OS==OSD_HPUX
#define OSD_SignalHandler(x)	void x (int)


#elif OSD_OS==OSD_LINUX
#define OSD_SignalHandler(x)	void x (...)

#elif OSD_OS==OSD_FBSD
#define OSD_SignalHandler(x)	void x (int)

#elif OSD_OS==OSD_NECEWS
#define OSD_SignalHandler(x)	void x ()


#elif OSD_OS==OSD_WINNT
#define OSD_SignalHandler(x)	void x (...)

#elif OSD_OS==OSD_SCO
#define OSD_SignalHandler(x)	void x (int)


#endif


#if OSD_OSTYPE==OSD_UNIX
#define OSD_SetSignalHandler(f,a) {											\
								  struct sigaction t;						\
								  t.sa_handler = f;							\
								  sigfillset(&(t.sa_mask));					\
								  t.sa_flags = 0;							\
								  if(sigaction(a,&t,NULL)!=0)				\
								      osdPrint(OSD_SYSTEM,OSD_FATAL,		\
								      "could not set signal handler");		\
								  }

#else
#define OSD_SetSignalHandler(f,a) /*NOOP*/
#endif
