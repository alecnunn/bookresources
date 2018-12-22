.386p
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc

Extrn SetUnhandledExceptionFilter : PROC
.data
message1        db "TRW detection by calling Int 3h",0
message3        db "TRW found",0
message2        db "TRW not found",0
delayESP        dd 0                    ;the ESP register saves here
previous        dd 0                    ;the ESP register saves the address of the previous SEH service here
.code
Start:
;------------------------------------------------------------------------------------------------
;Sets SEH in case of an error
;------------------------------------------------------------------------------------------------
        mov  [delayESP],esp
        push offset error
        call SetUnhandledExceptionFilter
        mov  [previous], eax
;------------------------------------------------------------------------------------------------
        call test                       ;jumps to the test; if the program continues on TRW is active
;------------------------------------------------------------------------------------------------
;Sets the previous SEH service
;------------------------------------------------------------------------------------------------
        push dword ptr [previous]
        call SetUnhandledExceptionFilter
;------------------------------------------------------------------------------------------------
        jmp jump                        ;the program gets here only if it has been traced in TRW
continue:
        call MessageBoxA,0, offset message2,\
        offset message1,0
        call ExitProcess, -1
jump:
        call MessageBoxA,0, offset message3,\
        offset message1,0
        call ExitProcess, -1
error:          ;sets a new SEH service in case of an error
        mov  esp, [delayESP]
        push offset continue
        ret
test:
        int  3h                         ;calls int 3h. If the program has been traced in TRW it will continue past INT 3h without calling the SEH service, which is an error
        ret                             ;the program will get here only if TRW is active
ends
end Start
