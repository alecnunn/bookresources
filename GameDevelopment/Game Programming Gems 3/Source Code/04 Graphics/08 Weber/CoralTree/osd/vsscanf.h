#ifndef OSD_VSSCANF_H
#define OSD_VSSCANF_H

/******************************************************************************

Coral Tree osd/vsscanf.h
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

#if OSD_COMPILER==OSD_KR
long osdIsSpace();
long osdIsDigit();
long osdIsXDigit();
long osdVSScanf();
long osdSScanf();
#else
long osdIsSpace(int c);
long osdIsDigit(int c);
long osdIsXDigit(int c);
long osdVSScanf(char *input, char *fmt, va_list argptr);
long osdSScanf(char *input, char *fmt,...);
#endif

#ifdef __cplusplus
    }
#endif

#endif /* OSD_VSSCANF_H */
