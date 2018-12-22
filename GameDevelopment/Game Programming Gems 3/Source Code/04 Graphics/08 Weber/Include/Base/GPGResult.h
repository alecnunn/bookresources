/******************************************************************************
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.

By downloading, copying, installing or using the software you agree to this
license. If you do not agree to this license, do not download, install,
copy or use the software.

Intel Open Source License for "Improved Bones Deformation" and
"Constrained Inverse Kinematics" sample code

Copyright (c) 2002, Intel Corporation.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
	be used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR
JURISDICTION. It is each licensee's responsibility to comply with any export
regulations applicable in each licensee's jurisdiction.
******************************************************************************/
//
//	GPGResult.H
//
//	DESCRIPTION
//		This module defines the GPGRESULT data type, GPGSUCCESS/GPGFAILURE
//	testing macros and the various return codes (both warnings and errors)
//	that are returned from different subsystems.
//
//	NOTES
//


#ifndef GPGResult_H
#define GPGResult_H


/***************************************************************************
//	Includes
***************************************************************************/


#include "GPGDataTypes.h"


/***************************************************************************
//	Classes, structures and types
***************************************************************************/


//---------------------------------------------------------------------------
//	GPGRESULT
//
//	This type is just a signed 32 bit value used to define function and
//	method return values.  Macros are available to discect the various bit
//	fields which are defined as follows:
//
//		  MSB                                       LSB
//		+----------+-----------+-----------+-----------+
//		|    31    | 30 ... 28 | 27 ... 16 | 15 ... 00 |
//		+----------+-----------+-----------+-----------+
//		| Severity | Reserved  | Component |   Code    |
//		+----------+-----------+-----------+-----------+
//
//		Severity:	0 for success or 1 for failure.
//		Reserved:	Always 0.
//		Component:	12 bits that identify who generated the result.
//		Code:		16 bits that identify the result code.
//
//---------------------------------------------------------------------------

typedef	I32	GPGRESULT;


/***************************************************************************
//	Enumerations
***************************************************************************/


//---------------------------------------------------------------------------
//	GPGResult_Severity
//
//	Defines the successful and failure severities.
//---------------------------------------------------------------------------

enum GPGResult_Severity
{
	GPGRESULT_SEVERITY_SUCCESS,
	GPGRESULT_SEVERITY_FAILURE
};

//---------------------------------------------------------------------------
//	GPGResult_Component
//
//	Defines the generic system component
//---------------------------------------------------------------------------

enum GPGResult_Component
{
	GPGRESULT_COMPONENT_GENERIC
};


/***************************************************************************
//	Defines.
***************************************************************************/


//---------------------------------------------------------------------------
//	GPGSUCCESS
//	GPGFAILURE
//
//	Return code testing macros.
//---------------------------------------------------------------------------

#define GPGSUCCESS( result ) \
			( GPGRESULT( result ) >= 0 )

#define GPGFAILURE( result ) \
			( GPGRESULT( result ) < 0 )

//---------------------------------------------------------------------------
//	MAKE_GPGRESULT
//	MAKE_GPGRESULT_SUCCESS
//	MAKE_GPGRESULT_FAILURE
//
//	Return code construction macros.
//---------------------------------------------------------------------------

#define MAKE_GPGRESULT( severity, component, code ) \
			GPGRESULT( ( U32( severity ) << 31 ) | ( U32( component ) << 16 ) | ( U32( code ) ) )

#define MAKE_GPGRESULT_PASS( component, code ) \
			MAKE_GPGRESULT( GPGRESULT_SEVERITY_SUCCESS, component, code )

#define MAKE_GPGRESULT_FAIL( component, code ) \
			MAKE_GPGRESULT( GPGRESULT_SEVERITY_FAILURE, component, code )

//---------------------------------------------------------------------------
//	PULL_GPGRESULT_SEVERITY
//	PULL_GPGRESULT_COMPONENT
//	PULL_GPGRESULT_CODE
//
//	Return code deconstruction macros.
//---------------------------------------------------------------------------

#define PULL_GPGRESULT_SEVERITY( result ) \
			( ( (result) >> 31 ) & 0x00000001L )

#define PULL_GPGRESULT_COMPONENT( result ) \
			( ( (result) >> 16 ) & 0x00000FFFL )

#define PULL_GPGRESULT_CODE( result ) \
			( ( result ) & 0x0000FFFFL )


//---------------------------------------------------------------------------
//	Successful return codes:  GPGRESULT_COMPONENT_GENERIC.
//---------------------------------------------------------------------------

// Successful.
#define GPG_OK								MAKE_GPGRESULT_PASS( GPGRESULT_COMPONENT_GENERIC, 0x0000 )

// Successful and the result is false.
#define GPG_FALSE							MAKE_GPGRESULT_PASS( GPGRESULT_COMPONENT_GENERIC, 0x0000 )

// Successful and the result is true.
#define GPG_TRUE							MAKE_GPGRESULT_PASS( GPGRESULT_COMPONENT_GENERIC, 0x0001 )

// Successful and the user requested cancel
#define GPG_CANCEL							MAKE_GPGRESULT_PASS( GPGRESULT_COMPONENT_GENERIC, 0x0002 )

//---------------------------------------------------------------------------
//	Warning return codes:  GPGRESULT_COMPONENT_GENERIC.
//---------------------------------------------------------------------------

// Successful, but the file version was different than expected
#define GPG_W_UNEXPECTED_FILE_VERSION		MAKE_GPGRESULT_PASS( GPGRESULT_COMPONENT_GENERIC, 0x0003 )

// Successful, but the item already exists
#define GPG_W_ALREADY_EXISTS				MAKE_GPGRESULT_PASS( GPGRESULT_COMPONENT_GENERIC, 0x0004 )

//---------------------------------------------------------------------------
//	Failure return codes:  GPGRESULT_COMPONENT_GENERIC.
//---------------------------------------------------------------------------

// An undefined error occurred.
#define GPG_E_UNDEFINED						MAKE_GPGRESULT_FAIL( GPGRESULT_COMPONENT_GENERIC, 0x0000 )

// Operation specified is not supported.
#define GPG_E_UNSUPPORTED					MAKE_GPGRESULT_FAIL( GPGRESULT_COMPONENT_GENERIC, 0x0001 )

// System memory request failed.
#define GPG_E_OUT_OF_MEMORY					MAKE_GPGRESULT_FAIL( GPGRESULT_COMPONENT_GENERIC, 0x0002 )

// File specified is invalid or cannot be found.
#define GPG_E_INVALID_FILE					MAKE_GPGRESULT_FAIL( GPGRESULT_COMPONENT_GENERIC, 0x0003 )

// Handle specified is invalid.
#define GPG_E_INVALID_HANDLE				MAKE_GPGRESULT_FAIL( GPGRESULT_COMPONENT_GENERIC, 0x0004 )

// Pointer specified is invalid.
#define GPG_E_INVALID_POINTER				MAKE_GPGRESULT_FAIL( GPGRESULT_COMPONENT_GENERIC, 0x0005 )

// One or more parameters are out of range.
#define GPG_E_INVALID_RANGE					MAKE_GPGRESULT_FAIL( GPGRESULT_COMPONENT_GENERIC, 0x0006 )

// Object is already initialized.
#define GPG_E_ALREADY_INITIALIZED			MAKE_GPGRESULT_FAIL( GPGRESULT_COMPONENT_GENERIC, 0x0007 )

// Object is not yet initialized.
#define GPG_E_NOT_INITIALIZED				MAKE_GPGRESULT_FAIL( GPGRESULT_COMPONENT_GENERIC, 0x0008 )

// Data is currently read-only
#define GPG_E_CANNOT_CHANGE					MAKE_GPGRESULT_FAIL( GPGRESULT_COMPONENT_GENERIC, 0x0009 )

// Function execution was canceled
#define GPG_E_ABORTED						MAKE_GPGRESULT_FAIL( GPGRESULT_COMPONENT_GENERIC, 0x000A )

// write failed (file or device), perhaps out of space
#define GPG_E_WRITE_FAILED					MAKE_GPGRESULT_FAIL( GPGRESULT_COMPONENT_GENERIC, 0x000B )

// read failed (file or device), perhaps end of file
#define GPG_E_READ_FAILED					MAKE_GPGRESULT_FAIL( GPGRESULT_COMPONENT_GENERIC, 0x000C )

// Could not find resource
#define GPG_E_CANNOT_FIND					MAKE_GPGRESULT_FAIL( GPGRESULT_COMPONENT_GENERIC, 0x000D )

// Subsystem (video card, etc) out of memory
#define GPG_E_SUBSYSTEM_OUT_OF_MEMORY		MAKE_GPGRESULT_FAIL( GPGRESULT_COMPONENT_GENERIC, 0x000E )

// subsystem failed
#define GPG_E_SUBSYSTEM_FAILURE				MAKE_GPGRESULT_FAIL( GPGRESULT_COMPONENT_GENERIC, 0x000F )
#endif
