/******************************************************************************

Coral Tree osd/input.h
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

#ifdef __cplusplus
    extern "C"
    {
#endif

#define OSD_GETSTDIN_BLOCKSIZE	40

#if OSD_COMPILER==OSD_KR
char *osdGetStdin();
void osdStdinSetEchoMode();
#else
char *osdGetStdin(long *n);
void osdStdinSetEchoMode(long m);
#endif

#ifdef __cplusplus
    }
#endif
