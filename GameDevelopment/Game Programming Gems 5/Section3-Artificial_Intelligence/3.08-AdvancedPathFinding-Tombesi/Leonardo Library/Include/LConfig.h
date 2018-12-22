/* ============================================================================
 *  LConfig.h
 * ============================================================================

 *  Author:         (c) 2001 Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        November 28, 2001
 *  Module:         LL

 *  Last changed:   $Date: 2003/02/05 18:51:00 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.19 $
*/

#ifndef __LConfig__
#define __LConfig__

/* COMPONENT ID */
#define LConfig_ID  0x800C

/* Supported platforms */
#define __LSL_Win32_x86__       0
#define __LSL_MacOSX_PowerPC__  1
#define __LSL_Linux_x86__       2
#define __LSL_Solaris_SPARC__   3

/* Current platform */
#ifndef __LSL_PLATFORM__
#define __LSL_PLATFORM__       __LSL_MacOSX_PowerPC__ 
#endif

/* Enable Debug Mode */
#define __LSL_DEBUG__


/* ----------------------------------------------------------------------------
 *  AUTO-SETUP
 * ----------------------------------------------------------------------------
*/

/* Supported OS CORE API */
#define __LSL_Win32__           0
#define __LSL_POSIX__           1

/* Supported OS GUI API */
#define __LSL_Win32__           0
#define __LSL_QT__              1
#define __LSL_NONE__            2

/* Supported CPU */
#define __LSL_x86__             0
#define __LSL_SPARC__           1
#define __LSL_PowerPC__         2

/* Newline format */
#define __LSL_CR__              0
#define __LSL_LF__              1
#define __LSL_CRLF__            2

/* Directory separator */
#define __LSL_COLON__           ':'
#define __LSL_SLASH__           '/'
#define __LSL_BACKSLASH__       '\\'

/* Config current OS CORE, OS GUI, newline format and CPU */
#if __LSL_PLATFORM__ == __LSL_Win32_x86__
#define __LSL_CPU__     __LSL_x86__
#define __LSL_OS_CORE__ __LSL_Win32__
#define __LSL_OS_GUI__  __LSL_Win32__
#define __LSL_NEWLINE__ __LSL_CRLF__
#define __LSL_DIR_SEP__ __LSL_BACKSLASH__
#endif

#if __LSL_PLATFORM__ == __LSL_MacOSX_PowerPC__
#define __LSL_CPU__     __LSL_PowerPC__
#define __LSL_OS_CORE__ __LSL_POSIX__
#define __LSL_OS_GUI__  __LSL_NONE__
#define __LSL_NEWLINE__ __LSL_LF__
#define __LSL_DIR_SEP__ __LSL_SLASH__
#endif

#if __LSL_PLATFORM__ == __LSL_Linux_x86__
#define __LSL_CPU__     __LSL_x86__
#define __LSL_OS_CORE__ __LSL_POSIX__
#define __LSL_OS_GUI__  __LSL_QT__
#define __LSL_NEWLINE__ __LSL_LF__
#define __LSL_DIR_SEP__ __LSL_SLASH__
#endif

#if __LSL_PLATFORM__ == __LSL_Solaris_SPARC__
#define __LSL_CPU__     __LSL_SPARC__
#define __LSL_OS_CORE__ __LSL_POSIX__
#define __LSL_OS_GUI__  __LSL_NONE__
#define __LSL_NEWLINE__ __LSL_LF__
#define __LSL_DIR_SEP__ __LSL_SLASH__
#endif

/* Little endian CPU */
#if __LSL_CPU__ == __LSL_x86__
#define __LSL_LITTLE_ENDIAN__
#endif

#endif


/* Copyright (C) 2001 Camil Demetrescu

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
