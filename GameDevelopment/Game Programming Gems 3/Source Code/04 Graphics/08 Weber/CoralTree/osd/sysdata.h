#ifndef OSD_SYSDATA_H
#define OSD_SYSDATA_H

/******************************************************************************

Coral Tree osd/sysdata.h
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


#ifdef __cplusplus
    extern "C"
    {
#endif


extern int osdSystemDataInitialize(void);
extern int osdSystemDataFinalize(void);
extern int osdSystemDataGetLoad(float *load);
extern int osdSystemDataThreadAlive(OSD_ExecutionId eid);
extern int osdSystemDataThreadList(OSD_ExecutionId **eids, uint32 *n);
extern int osdSystemDataProcessList(OSD_PID **pids, uint32 *n);


#ifdef __cplusplus
    }
#endif


#endif /* OSD_SYSDATA_H */
