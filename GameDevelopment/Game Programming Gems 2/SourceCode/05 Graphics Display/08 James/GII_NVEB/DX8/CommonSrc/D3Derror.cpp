/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#include <windows.h>
#include <adserr.h>
#include <subsmgr.h>
#include <d3d8.h>
#include <d3dx8.h>


char * D3DErrorToString(HRESULT hr)
{
    switch(hr)
    {
        
        // WINDOWS errors (non d3d specific)
    case E_FAIL                           : // (0x80000008L)
        return "E_FAIL                           ";
        
    case E_INVALIDARG                     : // (0x80000003L)
        return "E_INVALIDARG                     ";
        
    case E_OUTOFMEMORY                    : // (0x80000002L)
        return "E_OUTOFMEMORY                    ";
        
    case E_NOTIMPL                        : // (0x80000001L)
        return "E_NOTIMPL                        ";
        
    case E_NOINTERFACE                    : // (0x80000004L)
        return "E_NOINTERFACE                    ";
        //case MK_S_ASYNCHRONOUS    : // (0x000401E8L)                  
        //    return "MK_S_ASYNCHRONOUS    ";
        
    case S_ASYNCHRONOUS  : // (0x401e8L)
        return "S_ASYNCHRONOUS  ";
        
        // same
    case INET_E_ERROR_FIRST               : // (0x800C0002L)      
        return "INET_E_ERROR_FIRST               ";
        
        //case INET_E_INVALID_URL               : // (0x800C0002L)      
        //    return "INET_E_INVALID_URL               ";
        
        //case INET_E_REDIRECTING                 : // (0x800C0014L)      
        //    return "INET_E_REDIRECTING                 ";
        
    case INET_E_REDIRECT_FAILED           : // (0x800C0014L)      
        return "INET_E_REDIRECT_FAILED           ";
        
    case E_ADS_BAD_PATHNAME               : // (0x80005000L)
        return "E_ADS_BAD_PATHNAME               ";
        
    case E_ADS_INVALID_DOMAIN_OBJECT      : // (0x80005001L)
        return "E_ADS_INVALID_DOMAIN_OBJECT      ";
        
    case E_ADS_INVALID_USER_OBJECT        : // (0x80005002L)
        return "E_ADS_INVALID_USER_OBJECT        ";
        
    case E_ADS_INVALID_COMPUTER_OBJECT    : // (0x80005003L)
        return "E_ADS_INVALID_COMPUTER_OBJECT    ";
        
    case E_ADS_UNKNOWN_OBJECT             : // (0x80005004L)
        return "E_ADS_UNKNOWN_OBJECT             ";
        
    case E_ADS_PROPERTY_NOT_SET           : // (0x80005005L)
        return "E_ADS_PROPERTY_NOT_SET           ";
        
    case E_ADS_PROPERTY_NOT_SUPPORTED     : // (0x80005006L)
        return "E_ADS_PROPERTY_NOT_SUPPORTED     ";
        
    case E_ADS_PROPERTY_INVALID           : // (0x80005007L)
        return "E_ADS_PROPERTY_INVALID           ";
        
    case E_ADS_BAD_PARAMETER              : // (0x80005008L)
        return "E_ADS_BAD_PARAMETER              ";
        
    case E_ADS_OBJECT_UNBOUND             : // (0x80005009L)
        return "E_ADS_OBJECT_UNBOUND             ";
        
    case E_ADS_PROPERTY_NOT_MODIFIED      : // (0x8000500AL)
        return "E_ADS_PROPERTY_NOT_MODIFIED      ";
        
    case E_ADS_PROPERTY_MODIFIED          : // (0x8000500BL)
        return "E_ADS_PROPERTY_MODIFIED          ";
        
    case E_ADS_CANT_CONVERT_DATATYPE      : // (0x8000500CL)
        return "E_ADS_CANT_CONVERT_DATATYPE      ";
        
    case E_ADS_PROPERTY_NOT_FOUND         : // (0x8000500DL)
        return "E_ADS_PROPERTY_NOT_FOUND         ";
        
    case E_ADS_OBJECT_EXISTS              : // (0x8000500EL)
        return "E_ADS_OBJECT_EXISTS              ";
        
    case E_ADS_SCHEMA_VIOLATION           : // (0x8000500FL)
        return "E_ADS_SCHEMA_VIOLATION           ";
        
    case E_ADS_COLUMN_NOT_SET             : // (0x80005010L)
        return "E_ADS_COLUMN_NOT_SET             ";
        
    case S_ADS_ERRORSOCCURRED             : // (0x00005011L)
        return "S_ADS_ERRORSOCCURRED             ";
        
    case S_ADS_NOMORE_ROWS                : // (0x00005012L)
        return "S_ADS_NOMORE_ROWS                ";
        
    case S_ADS_NOMORE_COLUMNS             : // (0x00005013L)
        return "S_ADS_NOMORE_COLUMNS             ";
        
    case E_ADS_INVALID_FILTER             : // (0x80005014L)
        return "E_ADS_INVALID_FILTER             ";
        
        
    case INET_E_AGENT_MAX_SIZE_EXCEEDED       : // (0x800C0F80L)      
        return "INET_E_AGENT_MAX_SIZE_EXCEEDED       ";
        
    case INET_E_AGENT_CACHE_SIZE_EXCEEDED     : // (0x800C0F82L)      
        return "INET_E_AGENT_CACHE_SIZE_EXCEEDED     ";
        
    case INET_E_AGENT_CONNECTION_FAILED       : // (0x800C0F83L)      
        return "INET_E_AGENT_CONNECTION_FAILED       ";
        
    case INET_S_AGENT_PART_FAIL               : // (0x000C0F81L)      
        return "INET_S_AGENT_PART_FAIL               ";
        
        
    case E_PENDING : // (0x8000000AL)                             
        return "E_PENDING ";
        
        
    case INET_E_NO_SESSION                : // (0x800C0003L)      
        return "INET_E_NO_SESSION                ";
        
    case INET_E_CANNOT_CONNECT            : // (0x800C0004L)      
        return "INET_E_CANNOT_CONNECT            ";
        
    case INET_E_RESOURCE_NOT_FOUND        : // (0x800C0005L)      
        return "INET_E_RESOURCE_NOT_FOUND        ";
        
    case INET_E_OBJECT_NOT_FOUND          : // (0x800C0006L)      
        return "INET_E_OBJECT_NOT_FOUND          ";
        
    case INET_E_DATA_NOT_AVAILABLE        : // (0x800C0007L)      
        return "INET_E_DATA_NOT_AVAILABLE        ";
        
    case INET_E_DOWNLOAD_FAILURE          : // (0x800C0008L)      
        return "INET_E_DOWNLOAD_FAILURE          ";
        
    case INET_E_AUTHENTICATION_REQUIRED   : // (0x800C0009L)      
        return "INET_E_AUTHENTICATION_REQUIRED   ";
        
    case INET_E_NO_VALID_MEDIA            : // (0x800C000AL)      
        return "INET_E_NO_VALID_MEDIA            ";
        
    case INET_E_CONNECTION_TIMEOUT        : // (0x800C000BL)      
        return "INET_E_CONNECTION_TIMEOUT        ";
        
    case INET_E_INVALID_REQUEST           : // (0x800C000CL)      
        return "INET_E_INVALID_REQUEST           ";
        
    case INET_E_UNKNOWN_PROTOCOL          : // (0x800C000DL)      
        return "INET_E_UNKNOWN_PROTOCOL          ";
        
    case INET_E_SECURITY_PROBLEM          : // (0x800C000EL)      
        return "INET_E_SECURITY_PROBLEM          ";
        
    case INET_E_CANNOT_LOAD_DATA          : // (0x800C000FL)      
        return "INET_E_CANNOT_LOAD_DATA          ";
        
    case INET_E_CANNOT_INSTANTIATE_OBJECT : // (0x800C0010L)      
        return "INET_E_CANNOT_INSTANTIATE_OBJECT ";
        
        
    case INET_E_REDIRECT_TO_DIR           : // (0x800C0015L)      
        return "INET_E_REDIRECT_TO_DIR           ";
        
    case INET_E_CANNOT_LOCK_REQUEST       : // (0x800C0016L)      
        return "INET_E_CANNOT_LOCK_REQUEST       ";
        
        
    case INET_E_USE_DEFAULT_PROTOCOLHANDLER : // (0x800C0011L)      
        return "INET_E_USE_DEFAULT_PROTOCOLHANDLER ";
        
    case INET_E_USE_DEFAULT_SETTING         : // (0x800C0012L)      
        return "INET_E_USE_DEFAULT_SETTING         ";
        
    case INET_E_QUERYOPTION_UNKNOWN         : // (0x800C0013L)      
        return "INET_E_QUERYOPTION_UNKNOWN         ";
        
        
    case E_UNEXPECTED                     : // (0x8000FFFFL)
        return "E_UNEXPECTED                     ";
        
        //case E_NOTIMPL                        : // (0x80004001L)
        //    return "E_NOTIMPL                        ";
        
        //case E_OUTOFMEMORY                    : // (0x8007000EL)
        //    return "E_OUTOFMEMORY                    ";
        
        //case E_INVALIDARG                     : // (0x80070057L)
        //    return "E_INVALIDARG                     ";
        
        //case E_NOINTERFACE                    : // (0x80004002L)
        //    return "E_NOINTERFACE                    ";
        
    case E_POINTER                        : // (0x80004003L)
        return "E_POINTER                        ";
        
    case E_HANDLE                         : // (0x80070006L)
        return "E_HANDLE                         ";
        
    case E_ABORT                          : // (0x80004004L)
        return "E_ABORT                          ";
        
        //case E_FAIL                           : // (0x80004005L)
        //    return "E_FAIL                           ";
        
    case E_ACCESSDENIED                   : // (0x80070005L)
        return "E_ACCESSDENIED                   ";
        
        
        
        
        //case E_POINTER                        : // (0x80000005L)
        //    return "E_POINTER                        ";
        
        //case E_HANDLE                         : // (0x80000006L)
        //    return "E_HANDLE                         ";
        
        //case E_ABORT                          : // (0x80000007L)
        //    return "E_ABORT                          ";
        
        
        //case E_ACCESSDENIED                   : // (0x80000009L)
        //    return "E_ACCESSDENIED                   ";
        
        //case E_PENDING                        : // (0x8000000AL)
        //    return "E_PENDING                        ";
        
    case CO_E_INIT_TLS                    : // (0x80004006L)
        return "CO_E_INIT_TLS                    ";
        
    case CO_E_INIT_SHARED_ALLOCATOR       : // (0x80004007L)
        return "CO_E_INIT_SHARED_ALLOCATOR       ";
        
    case CO_E_INIT_MEMORY_ALLOCATOR       : // (0x80004008L)
        return "CO_E_INIT_MEMORY_ALLOCATOR       ";
        
    case CO_E_INIT_CLASS_CACHE            : // (0x80004009L)
        return "CO_E_INIT_CLASS_CACHE            ";
        
    case CO_E_INIT_RPC_CHANNEL            : // (0x8000400AL)
        return "CO_E_INIT_RPC_CHANNEL            ";
        
    case CO_E_INIT_TLS_SET_CHANNEL_CONTROL : // (0x8000400BL)
        return "CO_E_INIT_TLS_SET_CHANNEL_CONTROL ";
        
    case CO_E_INIT_TLS_CHANNEL_CONTROL    : // (0x8000400CL)
        return "CO_E_INIT_TLS_CHANNEL_CONTROL    ";
        
    case CO_E_INIT_UNACCEPTED_USER_ALLOCATOR : // (0x8000400DL)
        return "CO_E_INIT_UNACCEPTED_USER_ALLOCATOR ";
        
    case CO_E_INIT_SCM_MUTEX_EXISTS       : // (0x8000400EL)
        return "CO_E_INIT_SCM_MUTEX_EXISTS       ";
        
    case CO_E_INIT_SCM_FILE_MAPPING_EXISTS : // (0x8000400FL)
        return "CO_E_INIT_SCM_FILE_MAPPING_EXISTS ";
        
    case CO_E_INIT_SCM_MAP_VIEW_OF_FILE   : // (0x80004010L)
        return "CO_E_INIT_SCM_MAP_VIEW_OF_FILE   ";
        
    case CO_E_INIT_SCM_EXEC_FAILURE       : // (0x80004011L)
        return "CO_E_INIT_SCM_EXEC_FAILURE       ";
        
    case CO_E_INIT_ONLY_SINGLE_THREADED   : // (0x80004012L)
        return "CO_E_INIT_ONLY_SINGLE_THREADED   ";
        
    case CO_E_CANT_REMOTE                 : // (0x80004013L)
        return "CO_E_CANT_REMOTE                 ";
        
    case CO_E_BAD_SERVER_NAME             : // (0x80004014L)
        return "CO_E_BAD_SERVER_NAME             ";
        
    case CO_E_WRONG_SERVER_IDENTITY       : // (0x80004015L)
        return "CO_E_WRONG_SERVER_IDENTITY       ";
        
    case CO_E_OLE1DDE_DISABLED            : // (0x80004016L)
        return "CO_E_OLE1DDE_DISABLED            ";
        
    case CO_E_RUNAS_SYNTAX                : // (0x80004017L)
        return "CO_E_RUNAS_SYNTAX                ";
        
    case CO_E_CREATEPROCESS_FAILURE       : // (0x80004018L)
        return "CO_E_CREATEPROCESS_FAILURE       ";
        
    case CO_E_RUNAS_CREATEPROCESS_FAILURE : // (0x80004019L)
        return "CO_E_RUNAS_CREATEPROCESS_FAILURE ";
        
    case CO_E_RUNAS_LOGON_FAILURE         : // (0x8000401AL)
        return "CO_E_RUNAS_LOGON_FAILURE         ";
        
    case CO_E_LAUNCH_PERMSSION_DENIED     : // (0x8000401BL)
        return "CO_E_LAUNCH_PERMSSION_DENIED     ";
        
    case CO_E_START_SERVICE_FAILURE       : // (0x8000401CL)
        return "CO_E_START_SERVICE_FAILURE       ";
        
    case CO_E_REMOTE_COMMUNICATION_FAILURE : // (0x8000401DL)
        return "CO_E_REMOTE_COMMUNICATION_FAILURE ";
        
    case CO_E_SERVER_START_TIMEOUT        : // (0x8000401EL)
        return "CO_E_SERVER_START_TIMEOUT        ";
        
    case CO_E_CLSREG_INCONSISTENT         : // (0x8000401FL)
        return "CO_E_CLSREG_INCONSISTENT         ";
        
    case CO_E_IIDREG_INCONSISTENT         : // (0x80004020L)
        return "CO_E_IIDREG_INCONSISTENT         ";
        
    case CO_E_NOT_SUPPORTED               : // (0x80004021L)
        return "CO_E_NOT_SUPPORTED               ";
        
    case CO_E_RELOAD_DLL                  : // (0x80004022L)
        return "CO_E_RELOAD_DLL                  ";
        
    case CO_E_MSI_ERROR                   : // (0x80004023L)
        return "CO_E_MSI_ERROR                   ";
        
    case OLE_E_OLEVERB                    : // (0x80040000L)
        return "OLE_E_OLEVERB                    ";
        
    case OLE_E_ADVF                       : // (0x80040001L)
        return "OLE_E_ADVF                       ";
        
    case OLE_E_ENUM_NOMORE                : // (0x80040002L)
        return "OLE_E_ENUM_NOMORE                ";
        
    case OLE_E_ADVISENOTSUPPORTED         : // (0x80040003L)
        return "OLE_E_ADVISENOTSUPPORTED         ";
        
    case OLE_E_NOCONNECTION               : // (0x80040004L)
        return "OLE_E_NOCONNECTION               ";
        
    case OLE_E_NOTRUNNING                 : // (0x80040005L)
        return "OLE_E_NOTRUNNING                 ";
        
    case OLE_E_NOCACHE                    : // (0x80040006L)
        return "OLE_E_NOCACHE                    ";
        
    case OLE_E_BLANK                      : // (0x80040007L)
        return "OLE_E_BLANK                      ";
        
    case OLE_E_CLASSDIFF                  : // (0x80040008L)
        return "OLE_E_CLASSDIFF                  ";
        
    case OLE_E_CANT_GETMONIKER            : // (0x80040009L)
        return "OLE_E_CANT_GETMONIKER            ";
        
    case OLE_E_CANT_BINDTOSOURCE          : // (0x8004000AL)
        return "OLE_E_CANT_BINDTOSOURCE          ";
        
    case OLE_E_STATIC                     : // (0x8004000BL)
        return "OLE_E_STATIC                     ";
        
    case OLE_E_PROMPTSAVECANCELLED        : // (0x8004000CL)
        return "OLE_E_PROMPTSAVECANCELLED        ";
        
    case OLE_E_INVALIDRECT                : // (0x8004000DL)
        return "OLE_E_INVALIDRECT                ";
        
    case OLE_E_WRONGCOMPOBJ               : // (0x8004000EL)
        return "OLE_E_WRONGCOMPOBJ               ";
        
    case OLE_E_INVALIDHWND                : // (0x8004000FL)
        return "OLE_E_INVALIDHWND                ";
        
    case OLE_E_NOT_INPLACEACTIVE          : // (0x80040010L)
        return "OLE_E_NOT_INPLACEACTIVE          ";
        
    case OLE_E_CANTCONVERT                : // (0x80040011L)
        return "OLE_E_CANTCONVERT                ";
        
    case OLE_E_NOSTORAGE                  : // (0x80040012L)
        return "OLE_E_NOSTORAGE                  ";
        
    case DV_E_FORMATETC                   : // (0x80040064L)
        return "DV_E_FORMATETC                   ";
        
    case DV_E_DVTARGETDEVICE              : // (0x80040065L)
        return "DV_E_DVTARGETDEVICE              ";
        
    case DV_E_STGMEDIUM                   : // (0x80040066L)
        return "DV_E_STGMEDIUM                   ";
        
    case DV_E_STATDATA                    : // (0x80040067L)
        return "DV_E_STATDATA                    ";
        
    case DV_E_LINDEX                      : // (0x80040068L)
        return "DV_E_LINDEX                      ";
        
    case DV_E_TYMED                       : // (0x80040069L)
        return "DV_E_TYMED                       ";
        
    case DV_E_CLIPFORMAT                  : // (0x8004006AL)
        return "DV_E_CLIPFORMAT                  ";
        
    case DV_E_DVASPECT                    : // (0x8004006BL)
        return "DV_E_DVASPECT                    ";
        
    case DV_E_DVTARGETDEVICE_SIZE         : // (0x8004006CL)
        return "DV_E_DVTARGETDEVICE_SIZE         ";
        
    case DV_E_NOIVIEWOBJECT               : // (0x8004006DL)
        return "DV_E_NOIVIEWOBJECT               ";
        
    case DRAGDROP_E_NOTREGISTERED         : // (0x80040100L)
        return "DRAGDROP_E_NOTREGISTERED         ";
        
    case DRAGDROP_E_ALREADYREGISTERED     : // (0x80040101L)
        return "DRAGDROP_E_ALREADYREGISTERED     ";
        
    case DRAGDROP_E_INVALIDHWND           : // (0x80040102L)
        return "DRAGDROP_E_INVALIDHWND           ";
        
    case CLASS_E_NOAGGREGATION            : // (0x80040110L)
        return "CLASS_E_NOAGGREGATION            ";
        
    case CLASS_E_CLASSNOTAVAILABLE        : // (0x80040111L)
        return "CLASS_E_CLASSNOTAVAILABLE        ";
        
    case CLASS_E_NOTLICENSED              : // (0x80040112L)
        return "CLASS_E_NOTLICENSED              ";
        
    case VIEW_E_DRAW                      : // (0x80040140L)
        return "VIEW_E_DRAW                      ";
        
    case REGDB_E_READREGDB                : // (0x80040150L)
        return "REGDB_E_READREGDB                ";
        
    case REGDB_E_WRITEREGDB               : // (0x80040151L)
        return "REGDB_E_WRITEREGDB               ";
        
    case REGDB_E_KEYMISSING               : // (0x80040152L)
        return "REGDB_E_KEYMISSING               ";
        
    case REGDB_E_INVALIDVALUE             : // (0x80040153L)
        return "REGDB_E_INVALIDVALUE             ";
        
    case REGDB_E_CLASSNOTREG              : // (0x80040154L)
        return "REGDB_E_CLASSNOTREG              ";
        
    case REGDB_E_IIDNOTREG                : // (0x80040155L)
        return "REGDB_E_IIDNOTREG                ";
        
    case CAT_E_CATIDNOEXIST               : // (0x80040160L)
        return "CAT_E_CATIDNOEXIST               ";
        
    case CAT_E_NODESCRIPTION              : // (0x80040161L)
        return "CAT_E_NODESCRIPTION              ";
        
    case CS_E_PACKAGE_NOTFOUND            : // (0x80040164L)
        return "CS_E_PACKAGE_NOTFOUND            ";
        
    case CS_E_NOT_DELETABLE               : // (0x80040165L)
        return "CS_E_NOT_DELETABLE               ";
        
    case CS_E_CLASS_NOTFOUND              : // (0x80040166L)
        return "CS_E_CLASS_NOTFOUND              ";
        
    case CS_E_INVALID_VERSION             : // (0x80040167L)
        return "CS_E_INVALID_VERSION             ";
        
    case CS_E_NO_CLASSSTORE               : // (0x80040168L)
        return "CS_E_NO_CLASSSTORE               ";
        
    case CACHE_E_NOCACHE_UPDATED          : // (0x80040170L)
        return "CACHE_E_NOCACHE_UPDATED          ";
        
    case OLEOBJ_E_NOVERBS                 : // (0x80040180L)
        return "OLEOBJ_E_NOVERBS                 ";
        
    case OLEOBJ_E_INVALIDVERB             : // (0x80040181L)
        return "OLEOBJ_E_INVALIDVERB             ";
        
    case INPLACE_E_NOTUNDOABLE            : // (0x800401A0L)
        return "INPLACE_E_NOTUNDOABLE            ";
        
    case INPLACE_E_NOTOOLSPACE            : // (0x800401A1L)
        return "INPLACE_E_NOTOOLSPACE            ";
        
    case CONVERT10_E_OLESTREAM_GET        : // (0x800401C0L)
        return "CONVERT10_E_OLESTREAM_GET        ";
        
    case CONVERT10_E_OLESTREAM_PUT        : // (0x800401C1L)
        return "CONVERT10_E_OLESTREAM_PUT        ";
        
    case CONVERT10_E_OLESTREAM_FMT        : // (0x800401C2L)
        return "CONVERT10_E_OLESTREAM_FMT        ";
        
    case CONVERT10_E_OLESTREAM_BITMAP_TO_DIB : // (0x800401C3L)
        return "CONVERT10_E_OLESTREAM_BITMAP_TO_DIB ";
        
    case CONVERT10_E_STG_FMT              : // (0x800401C4L)
        return "CONVERT10_E_STG_FMT              ";
        
    case CONVERT10_E_STG_NO_STD_STREAM    : // (0x800401C5L)
        return "CONVERT10_E_STG_NO_STD_STREAM    ";
        
    case CONVERT10_E_STG_DIB_TO_BITMAP    : // (0x800401C6L)
        return "CONVERT10_E_STG_DIB_TO_BITMAP    ";
        
    case CLIPBRD_E_CANT_OPEN              : // (0x800401D0L)
        return "CLIPBRD_E_CANT_OPEN              ";
        
    case CLIPBRD_E_CANT_EMPTY             : // (0x800401D1L)
        return "CLIPBRD_E_CANT_EMPTY             ";
        
    case CLIPBRD_E_CANT_SET               : // (0x800401D2L)
        return "CLIPBRD_E_CANT_SET               ";
        
    case CLIPBRD_E_BAD_DATA               : // (0x800401D3L)
        return "CLIPBRD_E_BAD_DATA               ";
        
    case CLIPBRD_E_CANT_CLOSE             : // (0x800401D4L)
        return "CLIPBRD_E_CANT_CLOSE             ";
        
    case MK_E_CONNECTMANUALLY             : // (0x800401E0L)
        return "MK_E_CONNECTMANUALLY             ";
        
    case MK_E_EXCEEDEDDEADLINE            : // (0x800401E1L)
        return "MK_E_EXCEEDEDDEADLINE            ";
        
    case MK_E_NEEDGENERIC                 : // (0x800401E2L)
        return "MK_E_NEEDGENERIC                 ";
        
    case MK_E_UNAVAILABLE                 : // (0x800401E3L)
        return "MK_E_UNAVAILABLE                 ";
        
    case MK_E_SYNTAX                      : // (0x800401E4L)
        return "MK_E_SYNTAX                      ";
        
    case MK_E_NOOBJECT                    : // (0x800401E5L)
        return "MK_E_NOOBJECT                    ";
        
    case MK_E_INVALIDEXTENSION            : // (0x800401E6L)
        return "MK_E_INVALIDEXTENSION            ";
        
    case MK_E_INTERMEDIATEINTERFACENOTSUPPORTED : // (0x800401E7L)
        return "MK_E_INTERMEDIATEINTERFACENOTSUPPORTED ";
        
    case MK_E_NOTBINDABLE                 : // (0x800401E8L)
        return "MK_E_NOTBINDABLE                 ";
        
    case MK_E_NOTBOUND                    : // (0x800401E9L)
        return "MK_E_NOTBOUND                    ";
        
    case MK_E_CANTOPENFILE                : // (0x800401EAL)
        return "MK_E_CANTOPENFILE                ";
        
    case MK_E_MUSTBOTHERUSER              : // (0x800401EBL)
        return "MK_E_MUSTBOTHERUSER              ";
        
    case MK_E_NOINVERSE                   : // (0x800401ECL)
        return "MK_E_NOINVERSE                   ";
        
    case MK_E_NOSTORAGE                   : // (0x800401EDL)
        return "MK_E_NOSTORAGE                   ";
        
    case MK_E_NOPREFIX                    : // (0x800401EEL)
        return "MK_E_NOPREFIX                    ";
        
    case MK_E_ENUMERATION_FAILED          : // (0x800401EFL)
        return "MK_E_ENUMERATION_FAILED          ";
        
    case CO_E_NOTINITIALIZED              : // (0x800401F0L)
        return "CO_E_NOTINITIALIZED              ";
        
    case CO_E_ALREADYINITIALIZED          : // (0x800401F1L)
        return "CO_E_ALREADYINITIALIZED          ";
        
    case CO_E_CANTDETERMINECLASS          : // (0x800401F2L)
        return "CO_E_CANTDETERMINECLASS          ";
        
    case CO_E_CLASSSTRING                 : // (0x800401F3L)
        return "CO_E_CLASSSTRING                 ";
        
    case CO_E_IIDSTRING                   : // (0x800401F4L)
        return "CO_E_IIDSTRING                   ";
        
    case CO_E_APPNOTFOUND                 : // (0x800401F5L)
        return "CO_E_APPNOTFOUND                 ";
        
    case CO_E_APPSINGLEUSE                : // (0x800401F6L)
        return "CO_E_APPSINGLEUSE                ";
        
    case CO_E_ERRORINAPP                  : // (0x800401F7L)
        return "CO_E_ERRORINAPP                  ";
        
    case CO_E_DLLNOTFOUND                 : // (0x800401F8L)
        return "CO_E_DLLNOTFOUND                 ";
        
    case CO_E_ERRORINDLL                  : // (0x800401F9L)
        return "CO_E_ERRORINDLL                  ";
        
    case CO_E_WRONGOSFORAPP               : // (0x800401FAL)
        return "CO_E_WRONGOSFORAPP               ";
        
    case CO_E_OBJNOTREG                   : // (0x800401FBL)
        return "CO_E_OBJNOTREG                   ";
        
    case CO_E_OBJISREG                    : // (0x800401FCL)
        return "CO_E_OBJISREG                    ";
        
    case CO_E_OBJNOTCONNECTED             : // (0x800401FDL)
        return "CO_E_OBJNOTCONNECTED             ";
        
    case CO_E_APPDIDNTREG                 : // (0x800401FEL)
        return "CO_E_APPDIDNTREG                 ";
        
    case CO_E_RELEASED                    : // (0x800401FFL)
        return "CO_E_RELEASED                    ";
        
    case CO_E_FAILEDTOIMPERSONATE         : // (0x80040200L)
        return "CO_E_FAILEDTOIMPERSONATE         ";
        
    case CO_E_FAILEDTOGETSECCTX           : // (0x80040201L)
        return "CO_E_FAILEDTOGETSECCTX           ";
        
    case CO_E_FAILEDTOOPENTHREADTOKEN     : // (0x80040202L)
        return "CO_E_FAILEDTOOPENTHREADTOKEN     ";
        
    case CO_E_FAILEDTOGETTOKENINFO        : // (0x80040203L)
        return "CO_E_FAILEDTOGETTOKENINFO        ";
        
    case CO_E_TRUSTEEDOESNTMATCHCLIENT    : // (0x80040204L)
        return "CO_E_TRUSTEEDOESNTMATCHCLIENT    ";
        
    case CO_E_FAILEDTOQUERYCLIENTBLANKET  : // (0x80040205L)
        return "CO_E_FAILEDTOQUERYCLIENTBLANKET  ";
        
    case CO_E_FAILEDTOSETDACL             : // (0x80040206L)
        return "CO_E_FAILEDTOSETDACL             ";
        
    case CO_E_ACCESSCHECKFAILED           : // (0x80040207L)
        return "CO_E_ACCESSCHECKFAILED           ";
        
    case CO_E_NETACCESSAPIFAILED          : // (0x80040208L)
        return "CO_E_NETACCESSAPIFAILED          ";
        
    case CO_E_WRONGTRUSTEENAMESYNTAX      : // (0x80040209L)
        return "CO_E_WRONGTRUSTEENAMESYNTAX      ";
        
    case CO_E_INVALIDSID                  : // (0x8004020AL)
        return "CO_E_INVALIDSID                  ";
        
    case CO_E_CONVERSIONFAILED            : // (0x8004020BL)
        return "CO_E_CONVERSIONFAILED            ";
        
    case CO_E_NOMATCHINGSIDFOUND          : // (0x8004020CL)
        return "CO_E_NOMATCHINGSIDFOUND          ";
        
    case CO_E_LOOKUPACCSIDFAILED          : // (0x8004020DL)
        return "CO_E_LOOKUPACCSIDFAILED          ";
        
    case CO_E_NOMATCHINGNAMEFOUND         : // (0x8004020EL)
        return "CO_E_NOMATCHINGNAMEFOUND         ";
        
    case CO_E_LOOKUPACCNAMEFAILED         : // (0x8004020FL)
        return "CO_E_LOOKUPACCNAMEFAILED         ";
        
    case CO_E_SETSERLHNDLFAILED           : // (0x80040210L)
        return "CO_E_SETSERLHNDLFAILED           ";
        
    case CO_E_FAILEDTOGETWINDIR           : // (0x80040211L)
        return "CO_E_FAILEDTOGETWINDIR           ";
        
    case CO_E_PATHTOOLONG                 : // (0x80040212L)
        return "CO_E_PATHTOOLONG                 ";
        
    case CO_E_FAILEDTOGENUUID             : // (0x80040213L)
        return "CO_E_FAILEDTOGENUUID             ";
        
    case CO_E_FAILEDTOCREATEFILE          : // (0x80040214L)
        return "CO_E_FAILEDTOCREATEFILE          ";
        
    case CO_E_FAILEDTOCLOSEHANDLE         : // (0x80040215L)
        return "CO_E_FAILEDTOCLOSEHANDLE         ";
        
    case CO_E_EXCEEDSYSACLLIMIT           : // (0x80040216L)
        return "CO_E_EXCEEDSYSACLLIMIT           ";
        
    case CO_E_ACESINWRONGORDER            : // (0x80040217L)
        return "CO_E_ACESINWRONGORDER            ";
        
    case CO_E_INCOMPATIBLESTREAMVERSION   : // (0x80040218L)
        return "CO_E_INCOMPATIBLESTREAMVERSION   ";
        
    case CO_E_FAILEDTOOPENPROCESSTOKEN    : // (0x80040219L)
        return "CO_E_FAILEDTOOPENPROCESSTOKEN    ";
        
    case CO_E_DECODEFAILED                : // (0x8004021AL)
        return "CO_E_DECODEFAILED                ";
        
    case CO_E_ACNOTINITIALIZED            : // (0x8004021BL)
        return "CO_E_ACNOTINITIALIZED            ";
        
    case OLE_S_USEREG                     : // (0x00040000L)
        return "OLE_S_USEREG                     ";
        
    case OLE_S_STATIC                     : // (0x00040001L)
        return "OLE_S_STATIC                     ";
        
    case OLE_S_MAC_CLIPFORMAT             : // (0x00040002L)
        return "OLE_S_MAC_CLIPFORMAT             ";
        
    case DRAGDROP_S_DROP                  : // (0x00040100L)
        return "DRAGDROP_S_DROP                  ";
        
    case DRAGDROP_S_CANCEL                : // (0x00040101L)
        return "DRAGDROP_S_CANCEL                ";
        
    case DRAGDROP_S_USEDEFAULTCURSORS     : // (0x00040102L)
        return "DRAGDROP_S_USEDEFAULTCURSORS     ";
        
    case DATA_S_SAMEFORMATETC             : // (0x00040130L)
        return "DATA_S_SAMEFORMATETC             ";
        
    case VIEW_S_ALREADY_FROZEN            : // (0x00040140L)
        return "VIEW_S_ALREADY_FROZEN            ";
        
    case CACHE_S_FORMATETC_NOTSUPPORTED   : // (0x00040170L)
        return "CACHE_S_FORMATETC_NOTSUPPORTED   ";
        
    case CACHE_S_SAMECACHE                : // (0x00040171L)
        return "CACHE_S_SAMECACHE                ";
        
    case CACHE_S_SOMECACHES_NOTUPDATED    : // (0x00040172L)
        return "CACHE_S_SOMECACHES_NOTUPDATED    ";
        
    case OLEOBJ_S_INVALIDVERB             : // (0x00040180L)
        return "OLEOBJ_S_INVALIDVERB             ";
        
    case OLEOBJ_S_CANNOT_DOVERB_NOW       : // (0x00040181L)
        return "OLEOBJ_S_CANNOT_DOVERB_NOW       ";
        
    case OLEOBJ_S_INVALIDHWND             : // (0x00040182L)
        return "OLEOBJ_S_INVALIDHWND             ";
        
    case INPLACE_S_TRUNCATED              : // (0x000401A0L)
        return "INPLACE_S_TRUNCATED              ";
        
    case CONVERT10_S_NO_PRESENTATION      : // (0x000401C0L)
        return "CONVERT10_S_NO_PRESENTATION      ";
        
    case MK_S_REDUCED_TO_SELF             : // (0x000401E2L)
        return "MK_S_REDUCED_TO_SELF             ";
        
    case MK_S_ME                          : // (0x000401E4L)
        return "MK_S_ME                          ";
        
    case MK_S_HIM                         : // (0x000401E5L)
        return "MK_S_HIM                         ";
        
    case MK_S_US                          : // (0x000401E6L)
        return "MK_S_US                          ";
        
    case MK_S_MONIKERALREADYREGISTERED    : // (0x000401E7L)
        return "MK_S_MONIKERALREADYREGISTERED    ";
        
    case CO_E_CLASS_CREATE_FAILED         : // (0x80080001L)
        return "CO_E_CLASS_CREATE_FAILED         ";
        
    case CO_E_SCM_ERROR                   : // (0x80080002L)
        return "CO_E_SCM_ERROR                   ";
        
    case CO_E_SCM_RPC_FAILURE             : // (0x80080003L)
        return "CO_E_SCM_RPC_FAILURE             ";
        
    case CO_E_BAD_PATH                    : // (0x80080004L)
        return "CO_E_BAD_PATH                    ";
        
    case CO_E_SERVER_EXEC_FAILURE         : // (0x80080005L)
        return "CO_E_SERVER_EXEC_FAILURE         ";
        
    case CO_E_OBJSRV_RPC_FAILURE          : // (0x80080006L)
        return "CO_E_OBJSRV_RPC_FAILURE          ";
        
    case MK_E_NO_NORMALIZED               : // (0x80080007L)
        return "MK_E_NO_NORMALIZED               ";
        
    case CO_E_SERVER_STOPPING             : // (0x80080008L)
        return "CO_E_SERVER_STOPPING             ";
        
    case MEM_E_INVALID_ROOT               : // (0x80080009L)
        return "MEM_E_INVALID_ROOT               ";
        
    case MEM_E_INVALID_LINK               : // (0x80080010L)
        return "MEM_E_INVALID_LINK               ";
        
    case MEM_E_INVALID_SIZE               : // (0x80080011L)
        return "MEM_E_INVALID_SIZE               ";
        
    case CO_S_NOTALLINTERFACES            : // (0x00080012L)
        return "CO_S_NOTALLINTERFACES            ";
        
    case DISP_E_UNKNOWNINTERFACE          : // (0x80020001L)
        return "DISP_E_UNKNOWNINTERFACE          ";
        
    case DISP_E_MEMBERNOTFOUND            : // (0x80020003L)
        return "DISP_E_MEMBERNOTFOUND            ";
        
    case DISP_E_PARAMNOTFOUND             : // (0x80020004L)
        return "DISP_E_PARAMNOTFOUND             ";
        
    case DISP_E_TYPEMISMATCH              : // (0x80020005L)
        return "DISP_E_TYPEMISMATCH              ";
        
    case DISP_E_UNKNOWNNAME               : // (0x80020006L)
        return "DISP_E_UNKNOWNNAME               ";
        
    case DISP_E_NONAMEDARGS               : // (0x80020007L)
        return "DISP_E_NONAMEDARGS               ";
        
    case DISP_E_BADVARTYPE                : // (0x80020008L)
        return "DISP_E_BADVARTYPE                ";
        
    case DISP_E_EXCEPTION                 : // (0x80020009L)
        return "DISP_E_EXCEPTION                 ";
        
    case DISP_E_OVERFLOW                  : // (0x8002000AL)
        return "DISP_E_OVERFLOW                  ";
        
    case DISP_E_BADINDEX                  : // (0x8002000BL)
        return "DISP_E_BADINDEX                  ";
        
    case DISP_E_UNKNOWNLCID               : // (0x8002000CL)
        return "DISP_E_UNKNOWNLCID               ";
        
    case DISP_E_ARRAYISLOCKED             : // (0x8002000DL)
        return "DISP_E_ARRAYISLOCKED             ";
        
    case DISP_E_BADPARAMCOUNT             : // (0x8002000EL)
        return "DISP_E_BADPARAMCOUNT             ";
        
    case DISP_E_PARAMNOTOPTIONAL          : // (0x8002000FL)
        return "DISP_E_PARAMNOTOPTIONAL          ";
        
    case DISP_E_BADCALLEE                 : // (0x80020010L)
        return "DISP_E_BADCALLEE                 ";
        
    case DISP_E_NOTACOLLECTION            : // (0x80020011L)
        return "DISP_E_NOTACOLLECTION            ";
        
    case DISP_E_DIVBYZERO                 : // (0x80020012L)
        return "DISP_E_DIVBYZERO                 ";
        
    case TYPE_E_BUFFERTOOSMALL            : // (0x80028016L)
        return "TYPE_E_BUFFERTOOSMALL            ";
        
    case TYPE_E_FIELDNOTFOUND             : // (0x80028017L)
        return "TYPE_E_FIELDNOTFOUND             ";
        
    case TYPE_E_INVDATAREAD               : // (0x80028018L)
        return "TYPE_E_INVDATAREAD               ";
        
    case TYPE_E_UNSUPFORMAT               : // (0x80028019L)
        return "TYPE_E_UNSUPFORMAT               ";
        
    case TYPE_E_REGISTRYACCESS            : // (0x8002801CL)
        return "TYPE_E_REGISTRYACCESS            ";
        
    case TYPE_E_LIBNOTREGISTERED          : // (0x8002801DL)
        return "TYPE_E_LIBNOTREGISTERED          ";
        
    case TYPE_E_UNDEFINEDTYPE             : // (0x80028027L)
        return "TYPE_E_UNDEFINEDTYPE             ";
        
    case TYPE_E_QUALIFIEDNAMEDISALLOWED   : // (0x80028028L)
        return "TYPE_E_QUALIFIEDNAMEDISALLOWED   ";
        
    case TYPE_E_INVALIDSTATE              : // (0x80028029L)
        return "TYPE_E_INVALIDSTATE              ";
        
    case TYPE_E_WRONGTYPEKIND             : // (0x8002802AL)
        return "TYPE_E_WRONGTYPEKIND             ";
        
    case TYPE_E_ELEMENTNOTFOUND           : // (0x8002802BL)
        return "TYPE_E_ELEMENTNOTFOUND           ";
        
    case TYPE_E_AMBIGUOUSNAME             : // (0x8002802CL)
        return "TYPE_E_AMBIGUOUSNAME             ";
        
    case TYPE_E_NAMECONFLICT              : // (0x8002802DL)
        return "TYPE_E_NAMECONFLICT              ";
        
    case TYPE_E_UNKNOWNLCID               : // (0x8002802EL)
        return "TYPE_E_UNKNOWNLCID               ";
        
    case TYPE_E_DLLFUNCTIONNOTFOUND       : // (0x8002802FL)
        return "TYPE_E_DLLFUNCTIONNOTFOUND       ";
        
    case TYPE_E_BADMODULEKIND             : // (0x800288BDL)
        return "TYPE_E_BADMODULEKIND             ";
        
    case TYPE_E_SIZETOOBIG                : // (0x800288C5L)
        return "TYPE_E_SIZETOOBIG                ";
        
    case TYPE_E_DUPLICATEID               : // (0x800288C6L)
        return "TYPE_E_DUPLICATEID               ";
        
    case TYPE_E_INVALIDID                 : // (0x800288CFL)
        return "TYPE_E_INVALIDID                 ";
        
    case TYPE_E_TYPEMISMATCH              : // (0x80028CA0L)
        return "TYPE_E_TYPEMISMATCH              ";
        
    case TYPE_E_OUTOFBOUNDS               : // (0x80028CA1L)
        return "TYPE_E_OUTOFBOUNDS               ";
        
    case TYPE_E_IOERROR                   : // (0x80028CA2L)
        return "TYPE_E_IOERROR                   ";
        
    case TYPE_E_CANTCREATETMPFILE         : // (0x80028CA3L)
        return "TYPE_E_CANTCREATETMPFILE         ";
        
    case TYPE_E_CANTLOADLIBRARY           : // (0x80029C4AL)
        return "TYPE_E_CANTLOADLIBRARY           ";
        
    case TYPE_E_INCONSISTENTPROPFUNCS     : // (0x80029C83L)
        return "TYPE_E_INCONSISTENTPROPFUNCS     ";
        
    case TYPE_E_CIRCULARTYPE              : // (0x80029C84L)
        return "TYPE_E_CIRCULARTYPE              ";
        
    case STG_E_INVALIDFUNCTION            : // (0x80030001L)
        return "STG_E_INVALIDFUNCTION            ";
        
    case STG_E_FILENOTFOUND               : // (0x80030002L)
        return "STG_E_FILENOTFOUND               ";
        
    case STG_E_PATHNOTFOUND               : // (0x80030003L)
        return "STG_E_PATHNOTFOUND               ";
        
    case STG_E_TOOMANYOPENFILES           : // (0x80030004L)
        return "STG_E_TOOMANYOPENFILES           ";
        
    case STG_E_ACCESSDENIED               : // (0x80030005L)
        return "STG_E_ACCESSDENIED               ";
        
    case STG_E_INVALIDHANDLE              : // (0x80030006L)
        return "STG_E_INVALIDHANDLE              ";
        
    case STG_E_INSUFFICIENTMEMORY         : // (0x80030008L)
        return "STG_E_INSUFFICIENTMEMORY         ";
        
    case STG_E_INVALIDPOINTER             : // (0x80030009L)
        return "STG_E_INVALIDPOINTER             ";
        
    case STG_E_NOMOREFILES                : // (0x80030012L)
        return "STG_E_NOMOREFILES                ";
        
    case STG_E_DISKISWRITEPROTECTED       : // (0x80030013L)
        return "STG_E_DISKISWRITEPROTECTED       ";
        
    case STG_E_SEEKERROR                  : // (0x80030019L)
        return "STG_E_SEEKERROR                  ";
        
    case STG_E_WRITEFAULT                 : // (0x8003001DL)
        return "STG_E_WRITEFAULT                 ";
        
    case STG_E_READFAULT                  : // (0x8003001EL)
        return "STG_E_READFAULT                  ";
        
    case STG_E_SHAREVIOLATION             : // (0x80030020L)
        return "STG_E_SHAREVIOLATION             ";
        
    case STG_E_LOCKVIOLATION              : // (0x80030021L)
        return "STG_E_LOCKVIOLATION              ";
        
    case STG_E_FILEALREADYEXISTS          : // (0x80030050L)
        return "STG_E_FILEALREADYEXISTS          ";
        
    case STG_E_INVALIDPARAMETER           : // (0x80030057L)
        return "STG_E_INVALIDPARAMETER           ";
        
    case STG_E_MEDIUMFULL                 : // (0x80030070L)
        return "STG_E_MEDIUMFULL                 ";
        
    case STG_E_PROPSETMISMATCHED          : // (0x800300F0L)
        return "STG_E_PROPSETMISMATCHED          ";
        
    case STG_E_ABNORMALAPIEXIT            : // (0x800300FAL)
        return "STG_E_ABNORMALAPIEXIT            ";
        
    case STG_E_INVALIDHEADER              : // (0x800300FBL)
        return "STG_E_INVALIDHEADER              ";
        
    case STG_E_INVALIDNAME                : // (0x800300FCL)
        return "STG_E_INVALIDNAME                ";
        
    case STG_E_UNKNOWN                    : // (0x800300FDL)
        return "STG_E_UNKNOWN                    ";
        
    case STG_E_UNIMPLEMENTEDFUNCTION      : // (0x800300FEL)
        return "STG_E_UNIMPLEMENTEDFUNCTION      ";
        
    case STG_E_INVALIDFLAG                : // (0x800300FFL)
        return "STG_E_INVALIDFLAG                ";
        
    case STG_E_INUSE                      : // (0x80030100L)
        return "STG_E_INUSE                      ";
        
    case STG_E_NOTCURRENT                 : // (0x80030101L)
        return "STG_E_NOTCURRENT                 ";
        
    case STG_E_REVERTED                   : // (0x80030102L)
        return "STG_E_REVERTED                   ";
        
    case STG_E_CANTSAVE                   : // (0x80030103L)
        return "STG_E_CANTSAVE                   ";
        
    case STG_E_OLDFORMAT                  : // (0x80030104L)
        return "STG_E_OLDFORMAT                  ";
        
    case STG_E_OLDDLL                     : // (0x80030105L)
        return "STG_E_OLDDLL                     ";
        
    case STG_E_SHAREREQUIRED              : // (0x80030106L)
        return "STG_E_SHAREREQUIRED              ";
        
    case STG_E_NOTFILEBASEDSTORAGE        : // (0x80030107L)
        return "STG_E_NOTFILEBASEDSTORAGE        ";
        
    case STG_E_EXTANTMARSHALLINGS         : // (0x80030108L)
        return "STG_E_EXTANTMARSHALLINGS         ";
        
    case STG_E_DOCFILECORRUPT             : // (0x80030109L)
        return "STG_E_DOCFILECORRUPT             ";
        
    case STG_E_BADBASEADDRESS             : // (0x80030110L)
        return "STG_E_BADBASEADDRESS             ";
        
    case STG_E_INCOMPLETE                 : // (0x80030201L)
        return "STG_E_INCOMPLETE                 ";
        
    case STG_E_TERMINATED                 : // (0x80030202L)
        return "STG_E_TERMINATED                 ";
        
    case STG_S_CONVERTED                  : // (0x00030200L)
        return "STG_S_CONVERTED                  ";
        
    case STG_S_BLOCK                      : // (0x00030201L)
        return "STG_S_BLOCK                      ";
        
    case STG_S_RETRYNOW                   : // (0x00030202L)
        return "STG_S_RETRYNOW                   ";
        
    case STG_S_MONITORING                 : // (0x00030203L)
        return "STG_S_MONITORING                 ";
        
    case STG_S_MULTIPLEOPENS              : // (0x00030204L)
        return "STG_S_MULTIPLEOPENS              ";
        
    case STG_S_CONSOLIDATIONFAILED        : // (0x00030205L)
        return "STG_S_CONSOLIDATIONFAILED        ";
        
    case STG_S_CANNOTCONSOLIDATE          : // (0x00030206L)
        return "STG_S_CANNOTCONSOLIDATE          ";
        
    case RPC_E_CALL_REJECTED              : // (0x80010001L)
        return "RPC_E_CALL_REJECTED              ";
        
    case RPC_E_CALL_CANCELED              : // (0x80010002L)
        return "RPC_E_CALL_CANCELED              ";
        
    case RPC_E_CANTPOST_INSENDCALL        : // (0x80010003L)
        return "RPC_E_CANTPOST_INSENDCALL        ";
        
    case RPC_E_CANTCALLOUT_INASYNCCALL    : // (0x80010004L)
        return "RPC_E_CANTCALLOUT_INASYNCCALL    ";
        
    case RPC_E_CANTCALLOUT_INEXTERNALCALL : // (0x80010005L)
        return "RPC_E_CANTCALLOUT_INEXTERNALCALL ";
        
    case RPC_E_CONNECTION_TERMINATED      : // (0x80010006L)
        return "RPC_E_CONNECTION_TERMINATED      ";
        
    case RPC_E_SERVER_DIED                : // (0x80010007L)
        return "RPC_E_SERVER_DIED                ";
        
    case RPC_E_CLIENT_DIED                : // (0x80010008L)
        return "RPC_E_CLIENT_DIED                ";
        
    case RPC_E_INVALID_DATAPACKET         : // (0x80010009L)
        return "RPC_E_INVALID_DATAPACKET         ";
        
    case RPC_E_CANTTRANSMIT_CALL          : // (0x8001000AL)
        return "RPC_E_CANTTRANSMIT_CALL          ";
        
    case RPC_E_CLIENT_CANTMARSHAL_DATA    : // (0x8001000BL)
        return "RPC_E_CLIENT_CANTMARSHAL_DATA    ";
        
    case RPC_E_CLIENT_CANTUNMARSHAL_DATA  : // (0x8001000CL)
        return "RPC_E_CLIENT_CANTUNMARSHAL_DATA  ";
        
    case RPC_E_SERVER_CANTMARSHAL_DATA    : // (0x8001000DL)
        return "RPC_E_SERVER_CANTMARSHAL_DATA    ";
        
    case RPC_E_SERVER_CANTUNMARSHAL_DATA  : // (0x8001000EL)
        return "RPC_E_SERVER_CANTUNMARSHAL_DATA  ";
        
    case RPC_E_INVALID_DATA               : // (0x8001000FL)
        return "RPC_E_INVALID_DATA               ";
        
    case RPC_E_INVALID_PARAMETER          : // (0x80010010L)
        return "RPC_E_INVALID_PARAMETER          ";
        
    case RPC_E_CANTCALLOUT_AGAIN          : // (0x80010011L)
        return "RPC_E_CANTCALLOUT_AGAIN          ";
        
    case RPC_E_SERVER_DIED_DNE            : // (0x80010012L)
        return "RPC_E_SERVER_DIED_DNE            ";
        
    case RPC_E_SYS_CALL_FAILED            : // (0x80010100L)
        return "RPC_E_SYS_CALL_FAILED            ";
        
    case RPC_E_OUT_OF_RESOURCES           : // (0x80010101L)
        return "RPC_E_OUT_OF_RESOURCES           ";
        
    case RPC_E_ATTEMPTED_MULTITHREAD      : // (0x80010102L)
        return "RPC_E_ATTEMPTED_MULTITHREAD      ";
        
    case RPC_E_NOT_REGISTERED             : // (0x80010103L)
        return "RPC_E_NOT_REGISTERED             ";
        
    case RPC_E_FAULT                      : // (0x80010104L)
        return "RPC_E_FAULT                      ";
        
    case RPC_E_SERVERFAULT                : // (0x80010105L)
        return "RPC_E_SERVERFAULT                ";
        
    case RPC_E_CHANGED_MODE               : // (0x80010106L)
        return "RPC_E_CHANGED_MODE               ";
        
    case RPC_E_INVALIDMETHOD              : // (0x80010107L)
        return "RPC_E_INVALIDMETHOD              ";
        
    case RPC_E_DISCONNECTED               : // (0x80010108L)
        return "RPC_E_DISCONNECTED               ";
        
    case RPC_E_RETRY                      : // (0x80010109L)
        return "RPC_E_RETRY                      ";
        
    case RPC_E_SERVERCALL_RETRYLATER      : // (0x8001010AL)
        return "RPC_E_SERVERCALL_RETRYLATER      ";
        
    case RPC_E_SERVERCALL_REJECTED        : // (0x8001010BL)
        return "RPC_E_SERVERCALL_REJECTED        ";
        
    case RPC_E_INVALID_CALLDATA           : // (0x8001010CL)
        return "RPC_E_INVALID_CALLDATA           ";
        
    case RPC_E_CANTCALLOUT_ININPUTSYNCCALL : // (0x8001010DL)
        return "RPC_E_CANTCALLOUT_ININPUTSYNCCALL ";
        
    case RPC_E_WRONG_THREAD               : // (0x8001010EL)
        return "RPC_E_WRONG_THREAD               ";
        
    case RPC_E_THREAD_NOT_INIT            : // (0x8001010FL)
        return "RPC_E_THREAD_NOT_INIT            ";
        
    case RPC_E_VERSION_MISMATCH           : // (0x80010110L)
        return "RPC_E_VERSION_MISMATCH           ";
        
    case RPC_E_INVALID_HEADER             : // (0x80010111L)
        return "RPC_E_INVALID_HEADER             ";
        
    case RPC_E_INVALID_EXTENSION          : // (0x80010112L)
        return "RPC_E_INVALID_EXTENSION          ";
        
    case RPC_E_INVALID_IPID               : // (0x80010113L)
        return "RPC_E_INVALID_IPID               ";
        
    case RPC_E_INVALID_OBJECT             : // (0x80010114L)
        return "RPC_E_INVALID_OBJECT             ";
        
    case RPC_S_CALLPENDING                : // (0x80010115L)
        return "RPC_S_CALLPENDING                ";
        
    case RPC_S_WAITONTIMER                : // (0x80010116L)
        return "RPC_S_WAITONTIMER                ";
        
    case RPC_E_CALL_COMPLETE              : // (0x80010117L)
        return "RPC_E_CALL_COMPLETE              ";
        
    case RPC_E_UNSECURE_CALL              : // (0x80010118L)
        return "RPC_E_UNSECURE_CALL              ";
        
    case RPC_E_TOO_LATE                   : // (0x80010119L)
        return "RPC_E_TOO_LATE                   ";
        
    case RPC_E_NO_GOOD_SECURITY_PACKAGES  : // (0x8001011AL)
        return "RPC_E_NO_GOOD_SECURITY_PACKAGES  ";
        
    case RPC_E_ACCESS_DENIED              : // (0x8001011BL)
        return "RPC_E_ACCESS_DENIED              ";
        
    case RPC_E_REMOTE_DISABLED            : // (0x8001011CL)
        return "RPC_E_REMOTE_DISABLED            ";
        
    case RPC_E_INVALID_OBJREF             : // (0x8001011DL)
        return "RPC_E_INVALID_OBJREF             ";
        
    case RPC_E_NO_CONTEXT                 : // (0x8001011EL)
        return "RPC_E_NO_CONTEXT                 ";
        
    case RPC_E_TIMEOUT                    : // (0x8001011FL)
        return "RPC_E_TIMEOUT                    ";
        
    case RPC_E_NO_SYNC                    : // (0x80010120L)
        return "RPC_E_NO_SYNC                    ";
        
    case RPC_E_UNEXPECTED                 : // (0x8001FFFFL)
        return "RPC_E_UNEXPECTED                 ";
        
    case NTE_BAD_UID                      : // (0x80090001L)
        return "NTE_BAD_UID                      ";
        
    case NTE_BAD_HASH                     : // (0x80090002L)
        return "NTE_BAD_HASH                     ";
        
    case NTE_BAD_KEY                      : // (0x80090003L)
        return "NTE_BAD_KEY                      ";
        
    case NTE_BAD_LEN                      : // (0x80090004L)
        return "NTE_BAD_LEN                      ";
        
    case NTE_BAD_DATA                     : // (0x80090005L)
        return "NTE_BAD_DATA                     ";
        
    case NTE_BAD_SIGNATURE                : // (0x80090006L)
        return "NTE_BAD_SIGNATURE                ";
        
    case NTE_BAD_VER                      : // (0x80090007L)
        return "NTE_BAD_VER                      ";
        
    case NTE_BAD_ALGID                    : // (0x80090008L)
        return "NTE_BAD_ALGID                    ";
        
    case NTE_BAD_FLAGS                    : // (0x80090009L)
        return "NTE_BAD_FLAGS                    ";
        
    case NTE_BAD_TYPE                     : // (0x8009000AL)
        return "NTE_BAD_TYPE                     ";
        
    case NTE_BAD_KEY_STATE                : // (0x8009000BL)
        return "NTE_BAD_KEY_STATE                ";
        
    case NTE_BAD_HASH_STATE               : // (0x8009000CL)
        return "NTE_BAD_HASH_STATE               ";
        
    case NTE_NO_KEY                       : // (0x8009000DL)
        return "NTE_NO_KEY                       ";
        
    case NTE_NO_MEMORY                    : // (0x8009000EL)
        return "NTE_NO_MEMORY                    ";
        
    case NTE_EXISTS                       : // (0x8009000FL)
        return "NTE_EXISTS                       ";
        
    case NTE_PERM                         : // (0x80090010L)
        return "NTE_PERM                         ";
        
    case NTE_NOT_FOUND                    : // (0x80090011L)
        return "NTE_NOT_FOUND                    ";
        
    case NTE_DOUBLE_ENCRYPT               : // (0x80090012L)
        return "NTE_DOUBLE_ENCRYPT               ";
        
    case NTE_BAD_PROVIDER                 : // (0x80090013L)
        return "NTE_BAD_PROVIDER                 ";
        
    case NTE_BAD_PROV_TYPE                : // (0x80090014L)
        return "NTE_BAD_PROV_TYPE                ";
        
    case NTE_BAD_PUBLIC_KEY               : // (0x80090015L)
        return "NTE_BAD_PUBLIC_KEY               ";
        
    case NTE_BAD_KEYSET                   : // (0x80090016L)
        return "NTE_BAD_KEYSET                   ";
        
    case NTE_PROV_TYPE_NOT_DEF            : // (0x80090017L)
        return "NTE_PROV_TYPE_NOT_DEF            ";
        
    case NTE_PROV_TYPE_ENTRY_BAD          : // (0x80090018L)
        return "NTE_PROV_TYPE_ENTRY_BAD          ";
        
    case NTE_KEYSET_NOT_DEF               : // (0x80090019L)
        return "NTE_KEYSET_NOT_DEF               ";
        
    case NTE_KEYSET_ENTRY_BAD             : // (0x8009001AL)
        return "NTE_KEYSET_ENTRY_BAD             ";
        
    case NTE_PROV_TYPE_NO_MATCH           : // (0x8009001BL)
        return "NTE_PROV_TYPE_NO_MATCH           ";
        
    case NTE_SIGNATURE_FILE_BAD           : // (0x8009001CL)
        return "NTE_SIGNATURE_FILE_BAD           ";
        
    case NTE_PROVIDER_DLL_FAIL            : // (0x8009001DL)
        return "NTE_PROVIDER_DLL_FAIL            ";
        
    case NTE_PROV_DLL_NOT_FOUND           : // (0x8009001EL)
        return "NTE_PROV_DLL_NOT_FOUND           ";
        
    case NTE_BAD_KEYSET_PARAM             : // (0x8009001FL)
        return "NTE_BAD_KEYSET_PARAM             ";
        
    case NTE_FAIL                         : // (0x80090020L)
        return "NTE_FAIL                         ";
        
    case NTE_SYS_ERR                      : // (0x80090021L)
        return "NTE_SYS_ERR                      ";
        
    case CRYPT_E_MSG_ERROR                : // (0x80091001L)
        return "CRYPT_E_MSG_ERROR                ";
        
    case CRYPT_E_UNKNOWN_ALGO             : // (0x80091002L)
        return "CRYPT_E_UNKNOWN_ALGO             ";
        
    case CRYPT_E_OID_FORMAT               : // (0x80091003L)
        return "CRYPT_E_OID_FORMAT               ";
        
    case CRYPT_E_INVALID_MSG_TYPE         : // (0x80091004L)
        return "CRYPT_E_INVALID_MSG_TYPE         ";
        
    case CRYPT_E_UNEXPECTED_ENCODING      : // (0x80091005L)
        return "CRYPT_E_UNEXPECTED_ENCODING      ";
        
    case CRYPT_E_AUTH_ATTR_MISSING        : // (0x80091006L)
        return "CRYPT_E_AUTH_ATTR_MISSING        ";
        
    case CRYPT_E_HASH_VALUE               : // (0x80091007L)
        return "CRYPT_E_HASH_VALUE               ";
        
    case CRYPT_E_INVALID_INDEX            : // (0x80091008L)
        return "CRYPT_E_INVALID_INDEX            ";
        
    case CRYPT_E_ALREADY_DECRYPTED        : // (0x80091009L)
        return "CRYPT_E_ALREADY_DECRYPTED        ";
        
    case CRYPT_E_NOT_DECRYPTED            : // (0x8009100AL)
        return "CRYPT_E_NOT_DECRYPTED            ";
        
    case CRYPT_E_RECIPIENT_NOT_FOUND      : // (0x8009100BL)
        return "CRYPT_E_RECIPIENT_NOT_FOUND      ";
        
    case CRYPT_E_CONTROL_TYPE             : // (0x8009100CL)
        return "CRYPT_E_CONTROL_TYPE             ";
        
    case CRYPT_E_ISSUER_SERIALNUMBER      : // (0x8009100DL)
        return "CRYPT_E_ISSUER_SERIALNUMBER      ";
        
    case CRYPT_E_SIGNER_NOT_FOUND         : // (0x8009100EL)
        return "CRYPT_E_SIGNER_NOT_FOUND         ";
        
    case CRYPT_E_ATTRIBUTES_MISSING       : // (0x8009100FL)
        return "CRYPT_E_ATTRIBUTES_MISSING       ";
        
    case CRYPT_E_STREAM_MSG_NOT_READY     : // (0x80091010L)
        return "CRYPT_E_STREAM_MSG_NOT_READY     ";
        
    case CRYPT_E_STREAM_INSUFFICIENT_DATA : // (0x80091011L)
        return "CRYPT_E_STREAM_INSUFFICIENT_DATA ";
        
    case CRYPT_E_BAD_LEN                  : // (0x80092001L)
        return "CRYPT_E_BAD_LEN                  ";
        
    case CRYPT_E_BAD_ENCODE               : // (0x80092002L)
        return "CRYPT_E_BAD_ENCODE               ";
        
    case CRYPT_E_FILE_ERROR               : // (0x80092003L)
        return "CRYPT_E_FILE_ERROR               ";
        
    case CRYPT_E_NOT_FOUND                : // (0x80092004L)
        return "CRYPT_E_NOT_FOUND                ";
        
    case CRYPT_E_EXISTS                   : // (0x80092005L)
        return "CRYPT_E_EXISTS                   ";
        
    case CRYPT_E_NO_PROVIDER              : // (0x80092006L)
        return "CRYPT_E_NO_PROVIDER              ";
        
    case CRYPT_E_SELF_SIGNED              : // (0x80092007L)
        return "CRYPT_E_SELF_SIGNED              ";
        
    case CRYPT_E_DELETED_PREV             : // (0x80092008L)
        return "CRYPT_E_DELETED_PREV             ";
        
    case CRYPT_E_NO_MATCH                 : // (0x80092009L)
        return "CRYPT_E_NO_MATCH                 ";
        
    case CRYPT_E_UNEXPECTED_MSG_TYPE      : // (0x8009200AL)
        return "CRYPT_E_UNEXPECTED_MSG_TYPE      ";
        
    case CRYPT_E_NO_KEY_PROPERTY          : // (0x8009200BL)
        return "CRYPT_E_NO_KEY_PROPERTY          ";
        
    case CRYPT_E_NO_DECRYPT_CERT          : // (0x8009200CL)
        return "CRYPT_E_NO_DECRYPT_CERT          ";
        
    case CRYPT_E_BAD_MSG                  : // (0x8009200DL)
        return "CRYPT_E_BAD_MSG                  ";
        
    case CRYPT_E_NO_SIGNER                : // (0x8009200EL)
        return "CRYPT_E_NO_SIGNER                ";
        
    case CRYPT_E_PENDING_CLOSE            : // (0x8009200FL)
        return "CRYPT_E_PENDING_CLOSE            ";
        
    case CRYPT_E_REVOKED                  : // (0x80092010L)
        return "CRYPT_E_REVOKED                  ";
        
    case CRYPT_E_NO_REVOCATION_DLL        : // (0x80092011L)
        return "CRYPT_E_NO_REVOCATION_DLL        ";
        
    case CRYPT_E_NO_REVOCATION_CHECK      : // (0x80092012L)
        return "CRYPT_E_NO_REVOCATION_CHECK      ";
        
    case CRYPT_E_REVOCATION_OFFLINE       : // (0x80092013L)
        return "CRYPT_E_REVOCATION_OFFLINE       ";
        
    case CRYPT_E_NOT_IN_REVOCATION_DATABASE : // (0x80092014L)
        return "CRYPT_E_NOT_IN_REVOCATION_DATABASE ";
        
    case CRYPT_E_INVALID_NUMERIC_STRING   : // (0x80092020L)
        return "CRYPT_E_INVALID_NUMERIC_STRING   ";
        
    case CRYPT_E_INVALID_PRINTABLE_STRING : // (0x80092021L)
        return "CRYPT_E_INVALID_PRINTABLE_STRING ";
        
    case CRYPT_E_INVALID_IA5_STRING       : // (0x80092022L)
        return "CRYPT_E_INVALID_IA5_STRING       ";
        
    case CRYPT_E_INVALID_X500_STRING      : // (0x80092023L)
        return "CRYPT_E_INVALID_X500_STRING      ";
        
    case CRYPT_E_NOT_CHAR_STRING          : // (0x80092024L)
        return "CRYPT_E_NOT_CHAR_STRING          ";
        
    case CRYPT_E_FILERESIZED              : // (0x80092025L)
        return "CRYPT_E_FILERESIZED              ";
        
    case CRYPT_E_SECURITY_SETTINGS        : // (0x80092026L)
        return "CRYPT_E_SECURITY_SETTINGS        ";
        
    case CRYPT_E_NO_VERIFY_USAGE_DLL      : // (0x80092027L)
        return "CRYPT_E_NO_VERIFY_USAGE_DLL      ";
        
    case CRYPT_E_NO_VERIFY_USAGE_CHECK    : // (0x80092028L)
        return "CRYPT_E_NO_VERIFY_USAGE_CHECK    ";
        
    case CRYPT_E_VERIFY_USAGE_OFFLINE     : // (0x80092029L)
        return "CRYPT_E_VERIFY_USAGE_OFFLINE     ";
        
    case CRYPT_E_NOT_IN_CTL               : // (0x8009202AL)
        return "CRYPT_E_NOT_IN_CTL               ";
        
    case CRYPT_E_NO_TRUSTED_SIGNER        : // (0x8009202BL)
        return "CRYPT_E_NO_TRUSTED_SIGNER        ";
        
    case CRYPT_E_OSS_ERROR                : // (0x80093000L)
        return "CRYPT_E_OSS_ERROR                ";
        
    case CERTSRV_E_BAD_REQUESTSUBJECT     : // (0x80094001L)
        return "CERTSRV_E_BAD_REQUESTSUBJECT     ";
        
    case CERTSRV_E_NO_REQUEST             : // (0x80094002L)
        return "CERTSRV_E_NO_REQUEST             ";
        
    case CERTSRV_E_BAD_REQUESTSTATUS      : // (0x80094003L)
        return "CERTSRV_E_BAD_REQUESTSTATUS      ";
        
    case CERTSRV_E_PROPERTY_EMPTY         : // (0x80094004L)
        return "CERTSRV_E_PROPERTY_EMPTY         ";
        
    case CERTDB_E_JET_ERROR               : // (0x80095000L)
        return "CERTDB_E_JET_ERROR               ";
        
    case TRUST_E_SYSTEM_ERROR             : // (0x80096001L)
        return "TRUST_E_SYSTEM_ERROR             ";
        
    case TRUST_E_NO_SIGNER_CERT           : // (0x80096002L)
        return "TRUST_E_NO_SIGNER_CERT           ";
        
    case TRUST_E_COUNTER_SIGNER           : // (0x80096003L)
        return "TRUST_E_COUNTER_SIGNER           ";
        
    case TRUST_E_CERT_SIGNATURE           : // (0x80096004L)
        return "TRUST_E_CERT_SIGNATURE           ";
        
    case TRUST_E_TIME_STAMP               : // (0x80096005L)
        return "TRUST_E_TIME_STAMP               ";
        
    case TRUST_E_BAD_DIGEST               : // (0x80096010L)
        return "TRUST_E_BAD_DIGEST               ";
        
    case TRUST_E_BASIC_CONSTRAINTS        : // (0x80096019L)
        return "TRUST_E_BASIC_CONSTRAINTS        ";
        
    case TRUST_E_FINANCIAL_CRITERIA       : // (0x8009601EL)
        return "TRUST_E_FINANCIAL_CRITERIA       ";
        
    case TRUST_E_PROVIDER_UNKNOWN         : // (0x800B0001L)
        return "TRUST_E_PROVIDER_UNKNOWN         ";
        
    case TRUST_E_ACTION_UNKNOWN           : // (0x800B0002L)
        return "TRUST_E_ACTION_UNKNOWN           ";
        
    case TRUST_E_SUBJECT_FORM_UNKNOWN     : // (0x800B0003L)
        return "TRUST_E_SUBJECT_FORM_UNKNOWN     ";
        
    case TRUST_E_SUBJECT_NOT_TRUSTED      : // (0x800B0004L)
        return "TRUST_E_SUBJECT_NOT_TRUSTED      ";
        
    case DIGSIG_E_ENCODE                  : // (0x800B0005L)
        return "DIGSIG_E_ENCODE                  ";
        
    case DIGSIG_E_DECODE                  : // (0x800B0006L)
        return "DIGSIG_E_DECODE                  ";
        
    case DIGSIG_E_EXTENSIBILITY           : // (0x800B0007L)
        return "DIGSIG_E_EXTENSIBILITY           ";
        
    case DIGSIG_E_CRYPTO                  : // (0x800B0008L)
        return "DIGSIG_E_CRYPTO                  ";
        
    case PERSIST_E_SIZEDEFINITE           : // (0x800B0009L)
        return "PERSIST_E_SIZEDEFINITE           ";
        
    case PERSIST_E_SIZEINDEFINITE         : // (0x800B000AL)
        return "PERSIST_E_SIZEINDEFINITE         ";
        
    case PERSIST_E_NOTSELFSIZING          : // (0x800B000BL)
        return "PERSIST_E_NOTSELFSIZING          ";
        
    case TRUST_E_NOSIGNATURE              : // (0x800B0100L)
        return "TRUST_E_NOSIGNATURE              ";
        
    case CERT_E_EXPIRED                   : // (0x800B0101L)
        return "CERT_E_EXPIRED                   ";
        
    case CERT_E_VALIDITYPERIODNESTING     : // (0x800B0102L)
        return "CERT_E_VALIDITYPERIODNESTING     ";
        
    case CERT_E_ROLE                      : // (0x800B0103L)
        return "CERT_E_ROLE                      ";
        
    case CERT_E_PATHLENCONST              : // (0x800B0104L)
        return "CERT_E_PATHLENCONST              ";
        
    case CERT_E_CRITICAL                  : // (0x800B0105L)
        return "CERT_E_CRITICAL                  ";
        
    case CERT_E_PURPOSE                   : // (0x800B0106L)
        return "CERT_E_PURPOSE                   ";
        
    case CERT_E_ISSUERCHAINING            : // (0x800B0107L)
        return "CERT_E_ISSUERCHAINING            ";
        
    case CERT_E_MALFORMED                 : // (0x800B0108L)
        return "CERT_E_MALFORMED                 ";
        
    case CERT_E_UNTRUSTEDROOT             : // (0x800B0109L)
        return "CERT_E_UNTRUSTEDROOT             ";
        
    case CERT_E_CHAINING                  : // (0x800B010AL)
        return "CERT_E_CHAINING                  ";
        
    case TRUST_E_FAIL                     : // (0x800B010BL)
        return "TRUST_E_FAIL                     ";
        
    case CERT_E_REVOKED                   : // (0x800B010CL)
        return "CERT_E_REVOKED                   ";
        
    case CERT_E_UNTRUSTEDTESTROOT         : // (0x800B010DL)
        return "CERT_E_UNTRUSTEDTESTROOT         ";
        
    case CERT_E_REVOCATION_FAILURE        : // (0x800B010EL)
        return "CERT_E_REVOCATION_FAILURE        ";
        
    case CERT_E_CN_NO_MATCH               : // (0x800B010FL)
        return "CERT_E_CN_NO_MATCH               ";
        
    case CERT_E_WRONG_USAGE               : // (0x800B0110L)
        return "CERT_E_WRONG_USAGE               ";
        
    case SPAPI_E_EXPECTED_SECTION_NAME    : // (0x800F0000L)
        return "SPAPI_E_EXPECTED_SECTION_NAME    ";
        
    case SPAPI_E_BAD_SECTION_NAME_LINE    : // (0x800F0001L)
        return "SPAPI_E_BAD_SECTION_NAME_LINE    ";
        
    case SPAPI_E_SECTION_NAME_TOO_LONG    : // (0x800F0002L)
        return "SPAPI_E_SECTION_NAME_TOO_LONG    ";
        
    case SPAPI_E_GENERAL_SYNTAX           : // (0x800F0003L)
        return "SPAPI_E_GENERAL_SYNTAX           ";
        
    case SPAPI_E_WRONG_INF_STYLE          : // (0x800F0100L)
        return "SPAPI_E_WRONG_INF_STYLE          ";
        
    case SPAPI_E_SECTION_NOT_FOUND        : // (0x800F0101L)
        return "SPAPI_E_SECTION_NOT_FOUND        ";
        
    case SPAPI_E_LINE_NOT_FOUND           : // (0x800F0102L)
        return "SPAPI_E_LINE_NOT_FOUND           ";
        
    case SPAPI_E_NO_ASSOCIATED_CLASS      : // (0x800F0200L)
        return "SPAPI_E_NO_ASSOCIATED_CLASS      ";
        
    case SPAPI_E_CLASS_MISMATCH           : // (0x800F0201L)
        return "SPAPI_E_CLASS_MISMATCH           ";
        
    case SPAPI_E_DUPLICATE_FOUND          : // (0x800F0202L)
        return "SPAPI_E_DUPLICATE_FOUND          ";
        
    case SPAPI_E_NO_DRIVER_SELECTED       : // (0x800F0203L)
        return "SPAPI_E_NO_DRIVER_SELECTED       ";
        
    case SPAPI_E_KEY_DOES_NOT_EXIST       : // (0x800F0204L)
        return "SPAPI_E_KEY_DOES_NOT_EXIST       ";
        
    case SPAPI_E_INVALID_DEVINST_NAME     : // (0x800F0205L)
        return "SPAPI_E_INVALID_DEVINST_NAME     ";
        
    case SPAPI_E_INVALID_CLASS            : // (0x800F0206L)
        return "SPAPI_E_INVALID_CLASS            ";
        
    case SPAPI_E_DEVINST_ALREADY_EXISTS   : // (0x800F0207L)
        return "SPAPI_E_DEVINST_ALREADY_EXISTS   ";
        
    case SPAPI_E_DEVINFO_NOT_REGISTERED   : // (0x800F0208L)
        return "SPAPI_E_DEVINFO_NOT_REGISTERED   ";
        
    case SPAPI_E_INVALID_REG_PROPERTY     : // (0x800F0209L)
        return "SPAPI_E_INVALID_REG_PROPERTY     ";
        
    case SPAPI_E_NO_INF                   : // (0x800F020AL)
        return "SPAPI_E_NO_INF                   ";
        
    case SPAPI_E_NO_SUCH_DEVINST          : // (0x800F020BL)
        return "SPAPI_E_NO_SUCH_DEVINST          ";
        
    case SPAPI_E_CANT_LOAD_CLASS_ICON     : // (0x800F020CL)
        return "SPAPI_E_CANT_LOAD_CLASS_ICON     ";
        
    case SPAPI_E_INVALID_CLASS_INSTALLER  : // (0x800F020DL)
        return "SPAPI_E_INVALID_CLASS_INSTALLER  ";
        
    case SPAPI_E_DI_DO_DEFAULT            : // (0x800F020EL)
        return "SPAPI_E_DI_DO_DEFAULT            ";
        
    case SPAPI_E_DI_NOFILECOPY            : // (0x800F020FL)
        return "SPAPI_E_DI_NOFILECOPY            ";
        
    case SPAPI_E_INVALID_HWPROFILE        : // (0x800F0210L)
        return "SPAPI_E_INVALID_HWPROFILE        ";
        
    case SPAPI_E_NO_DEVICE_SELECTED       : // (0x800F0211L)
        return "SPAPI_E_NO_DEVICE_SELECTED       ";
        
    case SPAPI_E_DEVINFO_LIST_LOCKED      : // (0x800F0212L)
        return "SPAPI_E_DEVINFO_LIST_LOCKED      ";
        
    case SPAPI_E_DEVINFO_DATA_LOCKED      : // (0x800F0213L)
        return "SPAPI_E_DEVINFO_DATA_LOCKED      ";
        
    case SPAPI_E_DI_BAD_PATH              : // (0x800F0214L)
        return "SPAPI_E_DI_BAD_PATH              ";
        
    case SPAPI_E_NO_CLASSINSTALL_PARAMS   : // (0x800F0215L)
        return "SPAPI_E_NO_CLASSINSTALL_PARAMS   ";
        
    case SPAPI_E_FILEQUEUE_LOCKED         : // (0x800F0216L)
        return "SPAPI_E_FILEQUEUE_LOCKED         ";
        
    case SPAPI_E_BAD_SERVICE_INSTALLSECT  : // (0x800F0217L)
        return "SPAPI_E_BAD_SERVICE_INSTALLSECT  ";
        
    case SPAPI_E_NO_CLASS_DRIVER_LIST     : // (0x800F0218L)
        return "SPAPI_E_NO_CLASS_DRIVER_LIST     ";
        
    case SPAPI_E_NO_ASSOCIATED_SERVICE    : // (0x800F0219L)
        return "SPAPI_E_NO_ASSOCIATED_SERVICE    ";
        
    case SPAPI_E_NO_DEFAULT_DEVICE_INTERFACE : // (0x800F021AL)
        return "SPAPI_E_NO_DEFAULT_DEVICE_INTERFACE ";
        
    case SPAPI_E_DEVICE_INTERFACE_ACTIVE  : // (0x800F021BL)
        return "SPAPI_E_DEVICE_INTERFACE_ACTIVE  ";
        
    case SPAPI_E_DEVICE_INTERFACE_REMOVED : // (0x800F021CL)
        return "SPAPI_E_DEVICE_INTERFACE_REMOVED ";
        
    case SPAPI_E_BAD_INTERFACE_INSTALLSECT : // (0x800F021DL)
        return "SPAPI_E_BAD_INTERFACE_INSTALLSECT ";
        
    case SPAPI_E_NO_SUCH_INTERFACE_CLASS  : // (0x800F021EL)
        return "SPAPI_E_NO_SUCH_INTERFACE_CLASS  ";
        
    case SPAPI_E_INVALID_REFERENCE_STRING : // (0x800F021FL)
        return "SPAPI_E_INVALID_REFERENCE_STRING ";
        
    case SPAPI_E_INVALID_MACHINENAME      : // (0x800F0220L)
        return "SPAPI_E_INVALID_MACHINENAME      ";
        
    case SPAPI_E_REMOTE_COMM_FAILURE      : // (0x800F0221L)
        return "SPAPI_E_REMOTE_COMM_FAILURE      ";
        
    case SPAPI_E_MACHINE_UNAVAILABLE      : // (0x800F0222L)
        return "SPAPI_E_MACHINE_UNAVAILABLE      ";
        
    case SPAPI_E_NO_CONFIGMGR_SERVICES    : // (0x800F0223L)
        return "SPAPI_E_NO_CONFIGMGR_SERVICES    ";
        
    case SPAPI_E_INVALID_PROPPAGE_PROVIDER : // (0x800F0224L)
        return "SPAPI_E_INVALID_PROPPAGE_PROVIDER ";
        
    case SPAPI_E_NO_SUCH_DEVICE_INTERFACE : // (0x800F0225L)
        return "SPAPI_E_NO_SUCH_DEVICE_INTERFACE ";
        
    case SPAPI_E_DI_POSTPROCESSING_REQUIRED : // (0x800F0226L)
        return "SPAPI_E_DI_POSTPROCESSING_REQUIRED ";
        
    case SPAPI_E_INVALID_COINSTALLER      : // (0x800F0227L)
        return "SPAPI_E_INVALID_COINSTALLER      ";
        
    case SPAPI_E_NO_COMPAT_DRIVERS        : // (0x800F0228L)
        return "SPAPI_E_NO_COMPAT_DRIVERS        ";
        
    case SPAPI_E_NO_DEVICE_ICON           : // (0x800F0229L)
        return "SPAPI_E_NO_DEVICE_ICON           ";
        
    case SPAPI_E_INVALID_INF_LOGCONFIG    : // (0x800F022AL)
        return "SPAPI_E_INVALID_INF_LOGCONFIG    ";
        
    case SPAPI_E_DI_DONT_INSTALL          : // (0x800F022BL)
        return "SPAPI_E_DI_DONT_INSTALL          ";
        
    case SPAPI_E_INVALID_FILTER_DRIVER    : // (0x800F022CL)
        return "SPAPI_E_INVALID_FILTER_DRIVER    ";
        
    case SPAPI_E_ERROR_NOT_INSTALLED      : // (0x800F1000L)
        return "SPAPI_E_ERROR_NOT_INSTALLED      ";
        
        
        // D3D error
    case D3DERR_WRONGTEXTUREFORMAT      : //(2072):
        return"D3DERR_WRONGTEXTUREFORMAT";
        
    case D3DERR_UNSUPPORTEDCOLOROPERATION  : //(2073):
        return"D3DERR_UNSUPPORTEDCOLOROPERATION";
        
    case D3DERR_UNSUPPORTEDCOLORARG     : //(2074):
        return"D3DERR_UNSUPPORTEDCOLORARG";
        
    case D3DERR_UNSUPPORTEDALPHAOPERATION  : //(2075):
        return"D3DERR_UNSUPPORTEDALPHAOPERATION";
        
    case D3DERR_UNSUPPORTEDALPHAARG     : //(2076):
        return"D3DERR_UNSUPPORTEDALPHAARG";
        
    case D3DERR_TOOMANYOPERATIONS    : //(2077):
        return"D3DERR_TOOMANYOPERATIONS";
        
    case D3DERR_CONFLICTINGTEXTUREFILTER   : //(2078):
        return"D3DERR_CONFLICTINGTEXTUREFILTER";
        
    case D3DERR_UNSUPPORTEDFACTORVALUE     : //(2079):
        return"D3DERR_UNSUPPORTEDFACTORVALUE";
        
    case D3DERR_CONFLICTINGRENDERSTATE     : //(2081):
        return"D3DERR_CONFLICTINGRENDERSTATE";
        
    case D3DERR_UNSUPPORTEDTEXTUREFILTER   : //(2082):
        return"D3DERR_UNSUPPORTEDTEXTUREFILTER";
        
    case D3DERR_CONFLICTINGTEXTUREPALETTE  : //(2086):
        return"D3DERR_CONFLICTINGTEXTUREPALETTE";
        
        
        
    case D3DXERR_CANNOTMODIFYINDEXBUFFER  : //(2900),:
        return"D3DXERR_CANNOTMODIFYINDEXBUFFER";
        
    case D3DXERR_INVALIDMESH     : //(2901),:
        return"D3DXERR_INVALIDMESH";
        
    case D3DXERR_CANNOTATTRSORT     : //(2902),:
        return"D3DXERR_CANNOTATTRSORT";
        
    case D3DXERR_SKINNINGNOTSUPPORTED  : //(2903),:
        return"D3DXERR_SKINNINGNOTSUPPORTED		";
        
    case D3DXERR_TOOMANYINFLUENCES     : //(2904),:
        return"D3DXERR_TOOMANYINFLUENCES";
        
    case D3DXERR_INVALIDDATA     : //(2905),:
        return"D3DXERR_INVALIDDATA";
        
        
        
    case DXFILEERR_BADOBJECT     : //(850):
        return"DXFILEERR_BADOBJECT";
        
    case DXFILEERR_BADVALUE      : //(851):
        return"DXFILEERR_BADVALUE";
        
    case DXFILEERR_BADTYPE       : //(852):
        return"DXFILEERR_BADTYPE";
        
    case DXFILEERR_BADSTREAMHANDLE     : //(853):
        return"DXFILEERR_BADSTREAMHANDLE";
        
    case DXFILEERR_BADALLOC      : //(854):
        return"DXFILEERR_BADALLOC";
        
    case DXFILEERR_NOTFOUND      : //(855):
        return"DXFILEERR_NOTFOUND";
        
    case DXFILEERR_NOTDONEYET    : //(856):
        return"DXFILEERR_NOTDONEYET";
        
    case DXFILEERR_FILENOTFOUND     : //(857):
        return"DXFILEERR_FILENOTFOUND";
        
    case DXFILEERR_RESOURCENOTFOUND    : //(858):
        return"DXFILEERR_RESOURCENOTFOUND";
        
    case DXFILEERR_URLNOTFOUND      : //(859):
        return"DXFILEERR_URLNOTFOUND";
        
    case DXFILEERR_BADRESOURCE      : //(860):
        return"DXFILEERR_BADRESOURCE";
        
    case DXFILEERR_BADFILETYPE      : //(861):
        return"DXFILEERR_BADFILETYPE";
        
    case DXFILEERR_BADFILEVERSION   : //(862):
        return"DXFILEERR_BADFILEVERSION";
        
    case DXFILEERR_BADFILEFLOATSIZE    : //(863):
        return"DXFILEERR_BADFILEFLOATSIZE";
        
    case DXFILEERR_BADFILECOMPRESSIONTYPE : //(864):
        return"DXFILEERR_BADFILECOMPRESSIONTYPE";
        
    case DXFILEERR_BADFILE       : //(865):
        return"DXFILEERR_BADFILE";
        
    case DXFILEERR_PARSEERROR    : //(866):
        return"DXFILEERR_PARSEERROR";
        
    case DXFILEERR_NOTEMPLATE    : //(867):
        return"DXFILEERR_NOTEMPLATE";
        
    case DXFILEERR_BADARRAYSIZE     : //(868):
        return"DXFILEERR_BADARRAYSIZE";
        
    case DXFILEERR_BADDATAREFERENCE    : //(869):
        return"DXFILEERR_BADDATAREFERENCE";
        
    case DXFILEERR_INTERNALERROR    : //(870):
        return"DXFILEERR_INTERNALERROR";
        
    case DXFILEERR_NOMOREOBJECTS    : //(871):
        return"DXFILEERR_NOMOREOBJECTS";
        
    case DXFILEERR_BADINTRINSICS    : //(872):
        return"DXFILEERR_BADINTRINSICS";
        
    case DXFILEERR_NOMORESTREAMHANDLES    : //(873):
        return"DXFILEERR_NOMORESTREAMHANDLES";
        
    case DXFILEERR_NOMOREDATA    : //(874):
        return"DXFILEERR_NOMOREDATA";
        
    case DXFILEERR_BADCACHEFILE     : //(875):
        return"DXFILEERR_BADCACHEFILE";
        
    case DXFILEERR_NOINTERNET    : //(876):
        return"DXFILEERR_NOINTERNET";
        
    case D3DERR_DRIVERINTERNALERROR     : //(2087):
        return"D3DERR_DRIVERINTERNALERROR";
        
    case D3DERR_NOTFOUND       : //(2150):
        return"D3DERR_NOTFOUND";
        
    case D3DERR_MOREDATA       : //(2151):
        return"D3DERR_MOREDATA";
        
    case D3DERR_DEVICELOST        : //(2152):
        return"D3DERR_DEVICELOST";
        
    case D3DERR_DEVICENOTRESET       : //(2153):
        return"D3DERR_DEVICENOTRESET";
        
    case D3DERR_NOTAVAILABLE      : //(2154):
        return"D3DERR_NOTAVAILABLE";
        
    case D3DERR_OUTOFVIDEOMEMORY     : //(380):
        return"D3DERR_OUTOFVIDEOMEMORY";
        
    case D3DERR_INVALIDDEVICE     : //(2155):
        return"D3DERR_INVALIDDEVICE";
        
    case D3DERR_INVALIDCALL       : //(2156):
        return"D3DERR_INVALIDCALL";
        
    case D3DERR_DRIVERINVALIDCALL    : //(2157):
        return"D3DERR_DRIVERINVALIDCALL";
        
        
    case 0x8007002:
        return "The system cannot find the file specified";
        
    default:
        return"Unknown Error";
    }
}

