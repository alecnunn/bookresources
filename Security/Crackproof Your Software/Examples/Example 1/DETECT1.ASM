.386
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc

Extrn SetUnhandledExceptionFilter : PROC

.data

message1        db "Detection by calling INT 68h",0
message3        db "SoftICE found",0
message2        db "SoftICE not found",0
delayESP        dd 0                             ;the ESP register saves here
previous        dd 0                             ;ESP register saves the address of the previous SEH service here


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


                mov  ah,43h                     ;service number
                int  68h                        ;calls INT 68h

                push eax                        ;saves the return value

;----------------------------------------------------------------------------
;Sets previous SEH service
;----------------------------------------------------------------------------
                push dword ptr [previous]
                call SetUnhandledExceptionFilter
;----------------------------------------------------------------------------

                pop  eax                        ;restores the return value

                cmp  ax,0f386h                  ;tests to see whether the return value is a "magic number"
                jz   jump                       ;if yes, the program jumps because SoftICE is active in memory


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
