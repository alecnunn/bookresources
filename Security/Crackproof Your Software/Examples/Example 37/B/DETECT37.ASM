
.386p
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc


Extrn SetUnhandledExceptionFilter : PROC

Interrupt      equ 5                            ;interrupt number; 1 or 3 will make debugging more difficult.

.DATA

message1        db "An example of switching into Ring0 by using the IDT (EliCZ's method)",0
message2        db "An error occurred",0
message3        db "Ring0 was successfully activated",0
delayESP        dd 0                             ;the ESP register saves here
previous        dd 0                             ;ESP register saves the address of the previous SEH service here



.CODE
Start:

;----------------------------------------------------------------------------
;Sets SEH in case of an error
;----------------------------------------------------------------------------
                mov  [delayESP],esp
                push offset error
                call SetUnhandledExceptionFilter
                mov  [previous], eax
;----------------------------------------------------------------------------

                push edx
                sidt [esp-2]                    ;reads the IDT into the stack
                pop  edx

                add  edx,(Interrupt*8)+4        ;reads the vector of the required interrupt


                mov  ebx,[edx]
                mov  bx,word ptr [edx-4]        ;reads the address of the old service of the required interrupt

                lea  edi,InterruptHandler
                mov  [edx-4],di
                ror  edi,16                     ;sets a new interrupt service
                mov  [edx+2],di

                push ds                         ;saves the register for security
                push es


                int  Interrupt                  ;jumps into ring0 (the newly defined INT 5h service)

                pop  es                         ;restores registers
                pop  ds


                mov  [edx-4],bx                 ;sets the original interrupt service (INT 5h)
                ror  ebx,16
                mov  [edx+2],bx




;----------------------------------------------------------------------------
;Sets previous SEH service
;----------------------------------------------------------------------------
                push dword ptr [previous]
                call SetUnhandledExceptionFilter
;----------------------------------------------------------------------------

                jmp  jump                       ;jumps if the switch into ring0 was successful



continue:
                call MessageBoxA,0, offset message2, offset message1,0
                call ExitProcess, -1

jump:

                call MessageBoxA,0, offset message3, offset message1,0
                call ExitProcess, -1



error:                                          ;starts a new SEH service in case of an error

                mov  esp, [delayESP]
                push offset continue
                ret



;-----------------------------------------------------------------------------
;Your new INT 5h service (runs in Ring0)
;-----------------------------------------------------------------------------

InterruptHandler:

                mov  eax,dr7                    ;this instruction is functional only in ring0

                iretd                           ;jumps back into ring3



ends
end Start
