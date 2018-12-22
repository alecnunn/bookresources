.386p
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc

Extrn SetUnhandledExceptionFilter : PROC
.data
message1        db "CMPXCHG8B instruction usage with the LOCK prefix",0
message3        db "Program doesn't run properly, it has been changed while running! ",0
message2        db "SEH service was called (OK) ",0
delayESP        dd 0    ;the ESP register saves here
previous        dd 0    ;the ESP register will save the address of the previous SEH service here
.code
Start:
;------------------------------------------------------------------------------------------------
;Sets SEH if there is an error
;------------------------------------------------------------------------------------------------
        mov  [delayESP],esp
        push offset error
        call SetUnhandledExceptionFilter
        mov  [previous], eax
;------------------------------------------------------------------------------------------------
;LOCK CMPXCHG8B EAX
        db   0F0h, 0F0h, 0C7h, 0C8h     ;jumps to your SEH service, a label error. Normally the program should never get this far.
        jmp  jump
;------------------------------------------------------------------------------------------------
;Sets the previous SEH service
;------------------------------------------------------------------------------------------------
        push dword ptr [previous]
        call SetUnhandledExceptionFilter
continue:
        call MessageBoxA,0, offset message2,\
        offset message1,0
        call ExitProcess, -1
jump:
        call MessageBoxA,0, offset message3,\
        offset message1,0
        call ExitProcess, -1
error:          ;sets a new SEH service in case of an error
        mov  esp, [delayESP]            ;the program will jump here in case of an error in the LOCK CMPXCHG8B EAX instruction
        push offset continue
        ret
ends
end Start
