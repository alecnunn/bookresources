.386p
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc

Extrn SetUnhandledExceptionFilter : PROC
Interrupt       equ 5                   ;the interrupt numbers 1 or 3 will make debugging more difficult
.DATA
message1        db "Debug breakpoint detection",0
message2        db "Debug breakpoint not found",0
message3        db "Debug breakpoint found",0
delayESP        dd 0                    ;the ESP register saves here
previous        dd 0                    ;the ESP register will save the address of the previous SEH service here
.CODE
Start:
;------------------------------------------------------------------------------------------------
;Sets SEH in case of an error
;------------------------------------------------------------------------------------------------
        mov  [delayESP], esp
        push offset error
        call SetUnhandledExceptionFilter
        mov  [previous], eax
;------------------------------------------------------------------------------------------------
        push edx
        sidt [esp-2]               ;reads IDT into the stack
        pop  edx
        add  edx, (Interrupt*8)+4       ;reads the vector of the required interrupt

        mov  ebx,[edx]
        mov  bx,word ptr [edx-4]        ;reads the address of the old service of the required interrupt

        lea  edi,InterruptHandler
        mov  [edx-4],di
        ror  edi,16                     ;sets the new interrupt service
        mov  [edx+2],di

        push ds                         ;saves registers for security
        push es
        int Interrupt                   ;jumps into Ring0 (a newly defined INT 5h service)
        pop  es                         ;restores the registers
        pop  ds

        mov  [edx-4],bx                 ;sets the original INT 5h interrupt service
        ror ebx,16
        mov  [edx+2],bx

        push eax                        ;saves the return value
;------------------------------------------------------------------------------------------------
;Sets the previous SEH service
;------------------------------------------------------------------------------------------------
        push dword ptr [previous]
        call SetUnhandledExceptionFilter
;------------------------------------------------------------------------------------------------
        pop  eax                        ;restores the return value
        test eax,eax                    ;tests to see if eax=0
        jnz  jump                       ;if not, the program has found a debug breakpoint and it ends
continue:
        call MessageBoxA,0, offset message2,\
        offset message1,0
        call ExitProcess, -1
jump:
        call MessageBoxA,0, offset message3,\
        offset message1,0
        call ExitProcess, -1
error:          ;sets a new SEH service if there is an error
        mov  esp, [delayESP]
        push offset continue
        ret
;------------------------------------------------------------------------------------------------
;Your new service INT 5h (runs in Ring0)
;------------------------------------------------------------------------------------------------

InterruptHandler:
        mov  eax, dr0                   ;reads a value from the DR0 debug register
        test ax,ax                      ;tests to see if a breakpoint was set
        jnz  Debug_Breakpoint           ;if so, the program jumps

        mov  eax,dr1                    ;reads a value from the DR1 debug register
        test ax,ax                      ;tests to see if a breakpoint was set
        jnz  Debug_Breakpoint           ;if so, the program jumps

        mov  eax,dr2                    ;reads a value from the DR2 debug register
        test ax,ax                      ;tests to see if a breakpoint was set
        jnz  Debug_Breakpoint           ;if so, the program jumps

        mov  eax,dr3                    ;reads a value from the DR3 debug register
        test ax,ax                      ;tests to see if a breakpoint was set
        jnz  Debug_Breakpoint           ;if so, the program jumps
        iretd                           ;if a breakpoint was not set the program will return 0 into the eax register
Debug_Breakpoint:
        mov  eax,1                      ;sets the value 1 into the eax register to show that breakpoints are active
        iretd                           ;jump back into Ring3
ends
end Start
