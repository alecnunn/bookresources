.386
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc

Extrn SetUnhandledExceptionFilter : PROC
Extrn UnhandledExceptionFilter    : PROC
.data
message1        db "Detecting SoftICE by searching for the Int 3h instruction in UnhandledExceptionFilter",0
message3        db "SoftICE found",0
message2        db "SoftICE not found",0
delayESP        dd 0                    ;the ESP register saves here
previous        dd 0                    ;the ESP register will save the address of the previous SEH service here.
.code
Start:
;------------------------------------------------------------------------------------------------
;Sets SEH if there is an error
;------------------------------------------------------------------------------------------------
        mov  [delayESP], esp
        push offset error
        call SetUnhandledExceptionFilter
        mov  [previous], eax
;------------------------------------------------------------------------------------------------
        mov  eax, offset UnhandledExceptionFilter ;puts the address for a jump into the UnhandledExceptionFilter into eax
        mov  eax, [eax+2]               ;reads the address of the API function of the UnhandledExceptionFilter in the Import table
        mov  eax, [eax]                 ;reads the address of the UnhandledExceptionFilter function from the Import table

        push eax                        ;saves the API address of the UnhandledExceptionFilter function
;------------------------------------------------------------------------------------------------
;Sets the previous SEH service
;------------------------------------------------------------------------------------------------
        push dword ptr [previous]
        call SetUnhandledExceptionFilter

        pop  eax                        ;restores the API address of the UnhandledExceptionFilter function
        cmp  byte ptr [eax], 0cch       ;tests to see if the Int 3h instruction is present
        jz   jump                       ;if so, the program will jump because SoftICE is active in memory
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
