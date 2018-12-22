.386
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc

Extrn SetUnhandledExceptionFilter : PROC
.DATA
message1        db "User debugger detection",0
message3        db "Debugger found",0
message2        db "Debugger not found",0
delayESP        dd 0                    ;the ESP register saves here
previous        dd 0                    ;the ESP register will save the address of the previous SEH service here
.code
Start:
;------------------------------------------------------------------------------------------------
;Sets SEH in case of an error
;------------------------------------------------------------------------------------------------
        mov  [delayESP], esp
        push offset error
        call SetUnhandledExceptionFilter
        mov  [previous], eax
;------------------------------------------------------------------------------------------------
        xor  eax,eax                    ;nulls the eax register
        mov  al, fs:[20h]               ;reads a value. If the value is other than 0, a debugger is active

        push eax                        ;saves the value
;------------------------------------------------------------------------------------------------
;Sets the previous SEH service
;------------------------------------------------------------------------------------------------
        push dword ptr [previous]
        call SetUnhandledExceptionFilter
;------------------------------------------------------------------------------------------------
        pop  eax                        ;restores the value
        test eax,eax                    ;tests to see if the read value was 0
        jnz  jump                       ;if not, then a debugger is active in memory and the program jumps
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
ends
end Start
