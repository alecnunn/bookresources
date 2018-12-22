;*******************************************************************************


;this file contains information, constants and structures needed for SEH coding
;it's designed for Borland Turbo Assembler (v4.0+, MASM mode)

;Usage: INCLUDE SEH.inc


;----------------------------------documented-----------------------------------

_CONTEXT 			STRUC
cx_ContextFlags 		DD ?

;CONTEXT_DEBUG_REGISTERS
cx_Dr0				DD ?		 ;04
cx_Dr1				DD ?		 ;08
cx_Dr2				DD ?		 ;0C
cx_Dr3				DD ?		 ;10
cx_Dr6				DD ?		 ;14
cx_Dr7				DD ?		 ;18

;CONTEXT_FLOATING_POINT
cx_ControlWord			DD ?
cx_StatusWord			DD ?
cx_TagWord			DD ?
cx_ErrorOffset			DD ?
cx_ErrorSelector		DD ?
cx_DataOffset			DD ?
cx_DataSelector 		DD ?
SIZE_OF_80387_REGISTERS         EQU 80
cx_RegisterArea 		DB SIZE_OF_80387_REGISTERS DUP (?)
cx_Cr0NpxState			DD ?

;CONTEXT_SEGMENTS
cx_SegGs			DD ?		 ;8C
cx_SegFs			DD ?		 ;90
cx_SegEs			DD ?		 ;94
cx_SegDs			DD ?		 ;98

;CONTEXT_INTEGER
cx_Edi				DD ?		 ;9C
cx_Esi				DD ?		 ;A0
cx_Ebx				DD ?		 ;A4
cx_Edx				DD ?		 ;A8
cx_Ecx				DD ?		 ;AC
cx_Eax				DD ?		 ;B0

;CONTEXT_CONTROL
cx_Ebp				DD ?		 ;B4
cx_Eip				DD ?		 ;B8
cx_SegCs			DD ?		 ;BC
cx_EFlags			DD ?		 ;C0
cx_Esp				DD ?		 ;C4
cx_SegSs			DD ?		 ;C8
_CONTEXT 			ENDS
;size of CONTEXT is 0CCH bytes


;---cx_ContextFlags { used by _VWIN32_Set_Thread_Context(95) or NTCALL 13H(NT) }
CONTEXT_i386			EQU 000010000H
CONTEXT_i486			EQU 000010000H
CONTEXT_CONTROL 		EQU CONTEXT_i386+00000001H
CONTEXT_INTEGER 		EQU CONTEXT_i386+00000002H
CONTEXT_SEGMENTS		EQU CONTEXT_i386+00000004H
CONTEXT_FLOATING_POINT		EQU CONTEXT_i386+00000008H
CONTEXT_DEBUG_REGISTERS 	EQU CONTEXT_i386+00000010H

;there is a bug (+ instead of OR) in W32Main.inc (from Walk32 package) in EQU
;for CONTEXT_FULL, so better (when you don't need to update debug registers
;and FPU) use
CONTEXT_NORMAL                  EQU CONTEXT_CONTROL OR CONTEXT_INTEGER OR \
                                    CONTEXT_SEGMENTS

CONTEXT_ALL                     EQU CONTEXT_CONTROL OR CONTEXT_INTEGER OR \
                                    CONTEXT_SEGMENTS OR CONTEXT_FLOATING_POINT \
                                    OR CONTEXT_DEBUG_REGISTERS  
;note that CONTEXT_ALL (01001FH) is in cx_ContextFlags by default!

;-------------------------------------------------------------------------------

_EXCEPTION_POINTERS             STRUC ;parameter of top-level exception handler
        ExceptionRecord         DD ?  ;pointer to _EXCEPTION_RECORD
        ContextRecord           DD ?  ;pointer to _CONTEXT
_EXCEPTION_POINTERS             ENDS
;size of _EXCEPTION_POINTERS is 008H bytes


_EXCEPTION_RECORD		STRUC
	ExceptionCode		DD ?
	ExceptionFlags		DD ?             
	NextExceptionRecord	DD ?  ;pointer (for nested exceptions)
	ExceptionAddress	DD ?
	NumberParameters	DD ?
	EXCEPTION_MAXIMUM_PARAMETERS  EQU 15     
	ExceptionInformation	DD EXCEPTION_MAXIMUM_PARAMETERS DUP (?)
_EXCEPTION_RECORD		ENDS
;size of _EXCEPTION_RECORD is 014H..050H bytes

;---ExceptionInformation for EXCEPTION_ACCESS_VIOLATION
;NumberParameters = 2
;ExceptionInformation[0] = ACCESS_VIOLATION_READ or ACCESS_VIOLATION_WRITE
;ExceptionInformation[1] = virtual address of inaccessible data
;                          (Win95 usually = -1)  
 ACCESS_VIOLATION_READ             EQU 000000000H  ; for CMP instruction
 ACCESS_VIOLATION_WRITE            EQU 000000001H
;of course it's true for NT only
;in 95 is all READ ! :(

;---ExceptionInformation passed by RaiseException function
;it's up to you 

;RaisedExceptionCode has bit 28 set to 0 by system, hence
RaisedExceptionCodeMask            EQU 0EFFFFFFFH  ; for AND instruction
;of course it's true for NT only
;in 95 is not ExceptionCode changed ! :(

;other exceptions have some "information" too, but you can't believe it
;(for example breakpoint(in 95) has 1 parameter (usually), while
; breakpoint(in NT) has 3 parameters; they contain trash)


;---ExceptionFlags   for TEST, AND  or CMP instructions
EXCEPTION_CONTINUABLE              EQU 000000000H
EXCEPTION_NONCONTINUABLE           EQU 000000001H
UNWIND_STACK                       EQU 000000006H   ; ? 

;---ExceptionCodes   for CMP  instruction
EXCEPTION_WAIT_0		   EQU 000000000H
EXCEPTION_ABANDONED_WAIT_0 	   EQU 000000080H
EXCEPTION_USER_APC 		   EQU 0000000C0H
EXCEPTION_TIMEOUT		   EQU 000000102H
EXCEPTION_PENDING		   EQU 000000103H
EXCEPTION_SEGMENT_NOTIFICATION	   EQU 040000005H
EXCEPTION_GUARD_PAGE_VIOLATION	   EQU 080000001H
EXCEPTION_DATATYPE_MISALIGNMENT	   EQU 080000002H
EXCEPTION_BREAKPOINT		   EQU 080000003H   ; exception 3
EXCEPTION_SINGLE_STEP		   EQU 080000004H   ; exception 1
EXCEPTION_ACCESS_VIOLATION 	   EQU 0C0000005H   ; typically exception 13
EXCEPTION_IN_PAGE_ERROR		   EQU 0C0000006H
EXCEPTION_NO_MEMORY		   EQU 0C0000017H
EXCEPTION_ILLEGAL_INSTRUCTION	   EQU 0C000001DH
EXCEPTION_NONCONTINUABLE_EXCEPTION EQU 0C0000025H
EXCEPTION_INVALID_DISPOSITION	   EQU 0C0000026H
EXCEPTION_ARRAY_BOUNDS_EXCEEDED	   EQU 0C000008CH   ; exception 5
EXCEPTION_FLOAT_DENORMAL_OPERAND   EQU 0C000008DH   
EXCEPTION_FLT_DENORMAL_OPERAND     EQU 0C000008DH   
EXCEPTION_FLOAT_DIVIDE_BY_ZERO	   EQU 0C000008EH
EXCEPTION_FLT_DIVIDE_BY_ZERO	   EQU 0C000008EH
EXCEPTION_FLOAT_INEXACT_RESULT	   EQU 0C000008FH
EXCEPTION_FLT_INEXACT_RESULT	   EQU 0C000008FH
EXCEPTION_FLOAT_INVALID_OPERATION  EQU 0C0000090H
EXCEPTION_FLT_INVALID_OPERATION    EQU 0C0000090H
EXCEPTION_FLOAT_OVERFLOW           EQU 0C0000091H
EXCEPTION_FLT_OVERFLOW             EQU 0C0000091H
EXCEPTION_FLOAT_STACK_CHECK	   EQU 0C0000092H
EXCEPTION_FLT_STACK_CHECK	   EQU 0C0000092H
EXCEPTION_FLOAT_UNDERFLOW	   EQU 0C0000093H
EXCEPTION_FLT_UNDERFLOW	           EQU 0C0000093H
EXCEPTION_INTEGER_DIVIDE_BY_ZERO   EQU 0C0000094H   ; exception 0
EXCEPTION_INT_DIVIDE_BY_ZERO       EQU 0C0000094H   
EXCEPTION_INTEGER_OVERFLOW 	   EQU 0C0000095H   ; exception 4
EXCEPTION_INT_OVERFLOW 	           EQU 0C0000095H   
EXCEPTION_PRIVILEGED_INSTRUCTION   EQU 0C0000096H   ; typically exception 13
EXCEPTION_PRIV_INSTRUCTION         EQU 0C0000096H   
EXCEPTION_STACK_OVERFLOW	   EQU 0C00000FDH
EXCEPTION_CONTROL_C_EXIT	   EQU 0C000013AH

; there are many other STATUSes (watch code in NTice)


;---SEH API functions
; AbnormalTermination              ;doesn't exist, interpreted as C++ keyword
; GetExceptionCode                 ;doesn't exist, interpreted as C++ keyword
; GetExceptionInformation          ;doesn't exist, interpreted as C++ keyword
; UnhandledExceptionFilter         ;exists; called from kernel xFrame.filter

EXTRN STDCALL RaiseException:PROC                   ; has 4 parameters:
; 1st parameter is ExceptionCode   ;see _EXCEPTION_RECORD
; 2nd parameter is ExceptionFlags
; 3rd parameter is NumberParameters
; 4th parameter points to ExceptionInformation 

EXTRN STDCALL SetUnhandledExceptionFilter:PROC      ; has 1 parameter:
; 1st parameter points to top-level exception handler

; additional:
EXTRN STDCALL SetErrorMode:PROC                     ; has 1 parameter:
SEM_FAILCRITICALERRORS             EQU 000000001H
SEM_NOGPFAULTERRORBOX              EQU 000000002H   ; quiet exit vs. ErrorBox
SEM_NOALIGNMENTFAULTEXCEPT         EQU 000000004H   ; no effect on x86
SEM_NOOPENFILEERRORBOX             EQU 080000000H
; default ErrorMode is 0 

;---return codes for top-level exception handler   (EAX)
EXCEPTION_CONTINUE_EXECUTION       EQU -1
EXCEPTION_CONTINUE_SEARCH          EQU  0 
EXCEPTION_EXECUTE_HANDLER          EQU  1 

;---return codes for try-except exception handler  (EAX)
ExceptionContinueExecution         EQU  0
ExceptionContinueSearch            EQU  1
ExceptionNestedException           EQU  2
ExceptionCollidedUnwind            EQU  3




;----------------------------------my research----------------------------------

_xFrame_RECORD			STRUC 
       NextxFrame		DD ?  ; pointer or -1 for last member
       xHandler 		DD ?  ; pointer to code
       xTable			DD ?  ; pointer to xTableArray
       xScope			DD ?  ; 1st index of _xTable_RECORD or -1 for end
_xFrame_RECORD			ENDS
; size of _xFrame_RECORD = 010H bytes

; xTableArray			_xTable_RECORD  MAX_xTable_RECORDs  DUP (?)

_xTable_RECORD			STRUC
       NextxScope		DD ?  ; index of next xTable_RECORD or -1 for end
       filter			DD ?  ; pointer to code
       handler			DD ?  ; pointer to code
_xTable_RECORD			ENDS
; size of _xTable_RECORD = 00CH bytes

; xTable, xScope and _xTable_RECORD are used by kernel try-except exception
; handler only (or by C startup codes)

; for new thread is constructed NEW kernel xFrame with xScope=0; 
; xTable_RECORD's filter  calls UnhandledExceptionFilter and  handler 
; calls ExitProcess or ExitThread

;---constants for xScope, NextxScope and NextxFrame
;searching for filter function stops when xScope or NextxScope has this value:
xScopeEnd                       EQU -1
; when xTable points to nonexisting region (i.e. xTable = -2)
; softice's xframe says : (local unwind) 

;searching for xFrame stops when NextxFrame has this value:
xFrameEnd                       EQU -1

;---pointer to the 1st xFrame_RECORD
xFrameHead                      EQU DWORD PTR FS:0


;---procedure types
; try-except exception handler  (=xHandler)
  xHandler_PROC  PROCTYPE  C    :DWORD, :DWORD, :DWORD, :DWORD
; 1st parameter points to _EXCEPTION_RECORD
; 2nd parameter points to _xFrame_RECORD (belonging to this xHandler)
; 3rd parameter points to _CONTEXT
; 4th parameter points to _DISPATCHER_CONTEXT (various content, unusable on x86) 

; top-level exception handler  (called by UnhandledExceptionFilter)
  TopLevelHandler_PROC  PROCTYPE  STDCALL  :DWORD
; 1st parameter points to _EXCEPTION_POINTERS

COMMENT $
Comparison of try-except and top-level handlers

           Thing              try-except                top-level
Number of parameters              4                         1
Calling convention              C (ret)               STDCALL (ret 4)
API dependent                     no                       yes
Relationship                    father                     son
Must preserve regs         EBP, EBX, ESI, EDI            EBP, ESI
Called from                     kernel            UnhandledExceptionFilter
Where can be found        everywhere in system,         sometimes
                          in C startup codes..           in apps

In C++ startup codes is used copy of kernel xHandler to provide keywords:
try, except, finally, GetExceptionCode, GetExceptionInformation,
AbnormalTermination,..
$

;---lame macros
Inst_xFrame   MACRO ___PxHndl, ___PxTbl, ___PxSc
    PUSH ___PxSc OFFSET ___PxTbl OFFSET ___PxHndl xFrameHead
    MOV  xFrameHead, ESP
              ENDM

Rest_xFrame   MACRO
    POP  xFrameHead
    ADD  ESP, 4*3
              ENDM

Inst_TopLevel MACRO ___PTLhandler, ___OldTLhandler
    CALL SetUnhandledExceptionFilter, OFFSET ___PTLhandler
    MOV  DWORD PTR ___OldTLhandler, EAX
              ENDM

Rest_TopLevel MACRO ___PTLhandler, ___OldTLhandler
    CALL SetUnhandledExceptionFilter, DWORD PTR ___OldTLhandler
              ENDM

BeginxHandler MACRO ___Nomen, __P1, __P2, __P3, __P4
    ___Nomen  PROC  C __P1, __P2, __P3, __P4
              USES  EBX, ESI, EDI
              ENDM

EndxHandler   MACRO ___Nomen
    ___Nomen  ENDP
              ENDM

BeginTopLevelHandler MACRO ___Nomen, __P1
    ___Nomen  PROC   STDCALL __P1
              USES   ESI
              ENDM

EndTopLevelHandler   MACRO ___Nomen
    ___Nomen  ENDP
              ENDM

;*******************************************************************************
