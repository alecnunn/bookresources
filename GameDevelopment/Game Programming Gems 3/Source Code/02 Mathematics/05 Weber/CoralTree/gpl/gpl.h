
/******************************************************************************

Coral Tree gpl/gpl.h
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

#ifndef __GPL_H__
#define __GPL_H__


/* GPL_VERBOSITY */
#define GPL_DEBUG			1
#define GPL_NORMAL			2
#define GPL_SILENT			3

#ifndef GPL_VERBOSITY
#define GPL_VERBOSITY		GPL_SILENT
#endif

//#define GPL_USE_DESC_IMPLEMENTATION

#include <meta/platform.h>

#include <osd/osd.h>

// osd may already include this, but array needs this for preallocated new calls
#if OSD_OS!=OSD_LINUX
#if OSD_OS!=OSD_HPUX && OSD_COMPILER!=OSD_GNU
	#include <new.h>
#endif
#endif

#include <gpl/tools.h>
#include <gpl/error.h>
#include <gpl/typedefs.h>
#include <gpl/block.h>
#include <gpl/ref.h>
#include <gpl/cqueue.h>
#include <gpl/array.h>
#include <gpl/stack.h>
#include <gpl/darray.h>
#include <gpl/dlist.h>
#include <gpl/hash.h>
#include <gpl/list.h>
#include <gpl/hier.h>
#include <gpl/pair.h>
#include <gpl/map.h>
#include <gpl/set.h>
#include <gpl/str.h>
#include <gpl/bitgroup.h>
#include <gpl/regexp.h>
#include <gpl/file.h>
#include <gpl/ptr.h>
#include <gpl/ftable.h>
#include <gpl/depict.h>
#include <gpl/factory.h>
#include <gpl/object.h>
#include <gpl/persist.h>
#include <gpl/memory.h>
#include <gpl/heapmgr.h>
#include <gpl/llock.h>
#include <gpl/dbug.h>
#include <gpl/message.h>
#include <gpl/track.h>
#include <gpl/dir.h>
#include <gpl/time.h>
#include <gpl/value.h>
#include <gpl/desc.h>
#include <gpl/tuple.h>
#include <gpl/bag.h>
#include <gpl/log.h>
#include <gpl/version.h>
#include <gpl/filesel.h>

#endif /* __GPL_H__ */

