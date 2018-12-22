.386
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc


Extrn SetUnhandledExceptionFilter : PROC

.data

message1        db "Detection2 by means of CreateFileA",0
message3        db "SoftICE found",0
message2        db "SoftICE not found",0
delayESP        dd 0                             ;the ESP register saves here
previous        dd 0                             ;ESP register saves the address of the previous SEH service here
SOFTVIDEO       db "\\.\SIWVID",0                ;the name of the SoftICE graphic driver

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

                call CreateFileA, OFFSET SOFTVIDEO, FILE_FLAG_WRITE_THROUGH, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
                                                ;tries to open a file \\.\SIWVID

                push eax                        ;saves the return value

;----------------------------------------------------------------------------
;Sets previous SEH service
;----------------------------------------------------------------------------
                push dword ptr [previous]
                call SetUnhandledExceptionFilter
;----------------------------------------------------------------------------

                pop  eax                        ;restores the return value

                cmp  eax, -1                    ;tests for success
                jnz  jump                       ;if found, the program ends because SoftICE is active in memory


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
