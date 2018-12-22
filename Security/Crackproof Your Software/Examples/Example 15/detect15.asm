.386p
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc
Extrn SetUnhandledExceptionFilter : PROC
Interrupt equ 5                         ;the interrupt numbers 1 or 3 will make debugging more difficult

.DATA
message1        db "Detection by means of the DR7 debug register",0
message2        db "Debugger not found",0
message3        db "Debugger found",0
delayESP        dd 0                    ;the ESP register saves here
previous        dd 0                    ;the ESP register saves the address of the previous SEH service here

.CODE
Start:
;------------------------------------------------------------------------------------------------
;Sets SEH if there is an error
;------------------------------------------------------------------------------------------------
        mov  [delayESP],esp
        push offset error
        call SetUnhandledExceptionFilter
        mov  [previous], eax

        push edx
        sidt [esp-2]                    ;reads IDT into the stack
        pop  edx
        add   edx,(Interrupt*8)+4       ;reads a vector of the required interrupt

        mov  ebx,[edx]
        mov  bx, word ptr [edx-4]       ;reads the address of the required interrupt

        lea  edi,InterruptHandler
        mov  [edx-4],di
        ror  edi,16                     ;sets a new interrupt service
        mov  [edx+2],di

        push ds                         ;saves register for security
        push es
        int  Interrupt                  ;jumps into ring0 (the newly defined service INT 5h
        pop  es                         ;restores registers
        pop  ds

        mov  [edx-4],bx                 ;sets the original interrupt service (INT 5h)
        ror  ebx, 16
        mov  [edx+2],bx

        push eax                        ;saves the return value
;------------------------------------------------------------------------------------------------
;Sets the previous SEH service
;------------------------------------------------------------------------------------------------
        push dword ptr [previous]
        call SetUnhandledExceptionFilter

        pop  eax                        ;restores the return value

        cmp  eax, 400h                  ;tests to see if the return value is 400h, the correct value of the DR7 register. If it is, a debugger isn't active.
        jnz  jump                       ;if the return value is other than 400h, a debugger is active in memory.
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
;------------------------------------------------------------------------------------------------
;Your new service INT 5h (runs in Ring0)
;------------------------------------------------------------------------------------------------
InterruptHandler:
        mov  eax,dr7                    ;reads a value from DR7
        iretd                           ;jump back to ring3

ends
end Start
