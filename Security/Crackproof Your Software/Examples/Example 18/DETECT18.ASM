.386p
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc


Extrn SetUnhandledExceptionFilter : PROC

.DATA

message1        db "TRW detection using the distance between Int 1h and Int 3h",0
message3        db "TRW found",0
message2        db "TRW not found",0
delayESP        dd 0                             ;the ESP register saves here
previous        dd 0                             ;ESP register saves the address of the previous SEH service here
pIDT            db 6 dup  (0)                    ;IDT will be saved here

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

                sidt fword ptr pIDT             ;saves the IDT
                mov  eax, dword ptr [pIDT+2]    ;puts pointer to the interrupt table into eax
                add  eax,8                      ;puts int 1h vector address into eax
                mov  ebx, [eax]                 ;puts int 1h service address into ebx
                add  eax, 16                    ;puts int 3h vector address into eax
                mov  eax, [eax]                 ;puts Int 3h service address into eax
                and  eax, 0ffffh                ;selector will not be used
                and  ebx, 0ffffh                ;selector will not be used even with int 1h
                sub  eax, ebx                   ;calculates the distance between the interrupt services

                push eax                        ;saves the result

;----------------------------------------------------------------------------
;Sets previous SEH service
;----------------------------------------------------------------------------
                push dword ptr [previous]
                call SetUnhandledExceptionFilter
;----------------------------------------------------------------------------

                pop  eax                        ;restores the result

                cmp  eax, 2fch                  ;if eax is 2FCh, TRW is active in memory
                jz   jump                       ;and the program ends


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


ends
end Start
