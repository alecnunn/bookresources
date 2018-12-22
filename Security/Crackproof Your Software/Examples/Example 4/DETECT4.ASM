.386
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc


Extrn SetUnhandledExceptionFilter : PROC

.data

message1        db "Detection by means of CreateFileA",0
message3        db "SoftICE found",0
message2        db "SoftICE not found",0
delayESP        dd 0                          ;the ESP register saves here
previous        dd 0                          ;ESP register saves the address of the previous SEH service here
SOFT9x          db "\\.\SICE",0               ;the name of the SoftICE driver for Windows 9x
SOFTNT          db "\\.\NTICE",0              ;the name of the SoftICE driver for Windows NT and 2000



.code

Start:

;----------------------------------------------------------------------------
;Sets SEH in case of an error
;----------------------------------------------------------------------------
                mov  [delayESP],esp
                push offset error
                call SetUnhandledExceptionFilter
                mov  [previous], eax
;----------------------------------------------------------------------------
                call CreateFileA, OFFSET SOFT9x, FILE_FLAG_WRITE_THROUGH, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
                                                ;tries to open a file \\.\SICE
                cmp  eax, -1                    ;tests for success
                jz   noSOFT9x                   ;if not, the program jumps to the test for SoftICE NT and 2000

                push word ptr 1                 ;saves the value 1 into the stack to
                jmp  short found                ;show that SoftICE is active in memory

noSOFT9x:
                call CreateFileA, OFFSET SOFTNT, FILE_FLAG_WRITE_THROUGH, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
;                                               ;tries to open a file \\.\NTICE

                cmp  eax, -1                    ;tests for success
                push dword ptr 1                ;saves 1 into the stack to show that SoftICE is active in memory
                jnz  short found                ;if SoftICE for Windows NT and 2000 is active, the program ends

                pop  eax                        ;when SoftICE isn't active calls the value 1 from the stack
                push dword ptr 0                ;and saves 0 here to show that SoftICE isn't active

found:

;----------------------------------------------------------------------------
;Sets previous SEH service
;----------------------------------------------------------------------------
                push dword ptr [previous]
                call SetUnhandledExceptionFilter
;----------------------------------------------------------------------------

                pop  eax                        ;restores the return value

                test eax,eax                    ;tests the return value
                jnz  jump                       ;if eax is 1, the program jumps because SoftICE is active in memory

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
