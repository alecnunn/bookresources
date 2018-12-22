
.386
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc


Extrn SetUnhandledExceptionFilter : PROC

.data

message1        db "Detection by memory search",0
message2        db "SoftICE not found",0
message3        db "SoftICE found",0
delayESP       dd 0                             ;the ESP register saves here
previous dd 0                             ;ESP register saves the address of the previous SEH service here

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

                mov  al,  "W"                   ;searches for the WINICE.BR string in
						;V86 memory
                mov  edi, 10000h                ;begins the search here
                mov  ecx, 400000h-10000h        ;specifies the number of bytes to search

more:
                repnz SCASB                     ;searches for a "W" string in memory
                jecxz notfound                  ;if the string is not found, the memory search
                                                ;ends because SoftICE isn't active in memory.

                cmp  dword ptr [edi], "CINI"    ;when a "W" string is found, this tests to see
						;whether the "INIC" string follows.
                jz   found1                     ;ends when "INIC" is found

                jmp  more                       ;otherwise it searches all memory

found1:
                add  edi, 4                     ;move by 4 characters (bytes)
                cmp  dword ptr [edi], "RB.E"    ;when "WINIC" is found it checks to see if the
						;"E.RB" string follows
                jnz  more                       ;if it does not, the memory search ends

                push word ptr 1                 ;go here if SoftICE is active in memory and
                                                ;save 1 into the stack to show that SoftICE
						;was found.

                jmp  short found		

notfound:
                push word ptr 0                 ;Go here if SoftICE is not found in memory.

found:


;----------------------------------------------------------------------------
;Sets previous SEH service
;----------------------------------------------------------------------------
                push dword ptr [previous]
                call SetUnhandledExceptionFilter
;----------------------------------------------------------------------------

                pop  ax                         ;restores the return value

                test ax,ax                      ;testuje, ci je navratova hodnota 1
                jnz  jump                       ;if so, the program jumps, pretoze Soft-Ice je aktivny

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
