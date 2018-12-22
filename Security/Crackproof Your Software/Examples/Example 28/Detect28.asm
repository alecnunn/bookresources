.386p
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc



.data

message1        db "Detecting SoftICE through Int 1h",0
message2        db "SoftICE not found",0
message3        db "Soft-Ice found",0
mark            db 0

.code

Start:

                xor  eax,eax                    ;nulls the eax register before setting the new SEH service

                push offset xhandler            ;address for your new SEH service (xhandler)

                push dword ptr fs:[eax]         ;saves the address of the original SEH service

                mov  dword ptr fs:[eax],esp     ;sets your new SEH service


                mov  eax, cs
                test al,100b
                jne  Win9x                      ;jumps if it is active because this trick is only for Windows NT and 2000

                int  1h                         ;this causes an error so the program calls your new SEH service (xhandler)

                nop
                nop

                pop  dword ptr fs:[0]           ;sets the original SEH service
                add  esp,4                      ;clears the stack

                cmp  mark, 1
                jz   jump

Win9x:
                call MessageBoxA,0, offset message2, offset message1,0
                call ExitProcess, -1            ;Ends program
jump:

                call MessageBoxA,0, offset message3, offset message1,0
                call ExitProcess, -1            ;Ends program



;-----------------------------------------------------------------------------
;Your new SEH service (xhandler)
;-----------------------------------------------------------------------------
xhandler:
                mov  eax, [esp+04]              ;finds the exception number
                mov  ecx, [esp+0ch]             ;reads the address of the beginning of the context
                inc  dword ptr [ecx+0b8h]       ;increases EIP by 1
                mov  eax, [eax]                 ;reads the exception number
                sub  eax, 80000003h
                jz   end                        ;if the exception number is the Exception Breakpoint the program will end
                inc  dword ptr [ecx+0b8h]       ;increases EIP by 1
                dec  eax
                jnz  jump                       ;if it wasn't the Exception Access Violation then the program will end
                inc  mark                       ;SoftICE is active and the program will set the mark

end:
                xor  eax,eax
                ret                             ;jump back into the program



ends
end Start
