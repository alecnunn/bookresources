.386p
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc

Extrn SetUnhandledExceptionFilter : PROC

.data

message1        db "An example of switching into Ring0 using the LDT",0
message2        db "An error occurred",0
message3        db "Ring0 was successfully activated",0

delayESP        dd 0                             ;the ESP register saves here
previous        dd 0                             ;ESP register saves the address of the previous SEH service here

gdt_           df 0
call_          dd 00
               dw 0Fh
o_gate         dw 0
               dw 028h                           ;a segment for ring0
               dw 0EC00h
               dw 0
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



                mov  eax, offset ring0          ;puts the offset of your service for ring0 into the eax

                mov  [o_gate],ax                ;sets the address of your new ring0 service into the "callgate"
                shr  eax,16
                mov  [o_gate+6],ax

                xor  eax, eax
                sgdt fword ptr gdt_             ;saves the GDT
                mov  ebx,dword ptr [gdt_+2]     ;gets the GDT base address
                sldt ax
                add  ebx,eax                    ;gets the descriptor address

                mov  al,[ebx+4]
                mov  ah,[ebx+7]
                shl  eax,16                     ;gets the LDT address in the eax register

                mov  ax,[ebx+2]                 ;add into the eax to get the descriptor callgate address

                add  eax,8
                mov  edi,eax                    ;sets location in the callgate where you will start making changes
                mov  esi,offset o_gate          ;puts the address of your "callgate" into the esi register
                movsd                           ;and moves it into the real callgate
                movsd                           ;which will prepare a jump into ring0

                call fword ptr [call_]          ;jumps your service into ring0

                xor  eax, eax
                sub  edi,8                      ;nulls your changes in the callgate
                stosd
                stosd


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
;Your new ring0 service
;-----------------------------------------------------------------------------
ring0:
                mov eax, dr7                    ;this instruction works only in ring0

                retf                            ;returns to ring3

ends
end Start
