.386p
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc

Extrn SetUnhandledExceptionFilter : PROC
Interrupt       equ 5                   ;an interrupt number; numbers 1 or 3 will make debugging more difficult
.DATA
message1        db "Anti-FrogsICE trick",0
message2        db "FrogsICE isn't active",0
delayESP        dd 0                    ;the ESP register saves here
previous        dd 0                    ;the ESP register will save the address of the previous SEH service here
.CODE
Start:

;------------------------------------------------------------------------------------------------
;Sets SEH if there is an error
;------------------------------------------------------------------------------------------------
        mov  [delayESP],esp
        push offset error
        call SetUnhandledExceptionFilter
        mov  [previous], eax
;------------------------------------------------------------------------------------------------
        push edx
        sidt [esp-2]                    ;reads the IDT into the stack
        pop  edx
        add  edx,(Interrupt *8)+4       ;reads the vector of the required interrupt (Int 5h)

        mov  ebx,[edx]
        mov  bx,word ptr [edx-4]        ;reads the address of the old service of the required interrupt (INT 5h)

        lea  edi,InterruptHandler
        mov  [edx-4],di
        ror  edi,16                     ;sets a new interrupt service (INT 5h)
        mov  [edx+2], di

        push ds                         ;saves registers for security
        push es
        int Interrupt                   ;jump into Ring0 (a new service INT 5h that you have defined)
        pop  es                         ;restores registers
        pop  es

        mov  [edx-4],bx                 ;sets the original service of the INT 5h interrupt
        ror  ebx,16
        mov  [edx+2],bx
;------------------------------------------------------------------------------------------------
;Sets the previous SEH service
;------------------------------------------------------------------------------------------------
        push dword ptr [previous]
        call SetUnhandledExceptionFilter
;------------------------------------------------------------------------------------------------
continue:
        call MessageBoxA,0, offset message2,\
        offset message1,0
        call ExitProcess, -1
error:          ;sets a new SEH service in case of an error
        mov  esp, [delayESP]
        push offset continue
        ret
;------------------------------------------------------------------------------------------------
;Your new INT 5h service (runs in Ring0)
;------------------------------------------------------------------------------------------------
InterruptHandler:
        db 0cdh, 20h
        dd 00001013FH
;             VMMCall   VMM_GetDDBList  ;this call will cause FrogsICE to crash
        iretd                           ;if the program gets here FrogsICE isn't active in memory
ends
end Start
