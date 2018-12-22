.386
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc

Extrn SetUnhandledExceptionFilter : PROC


anti_dis1       macro abc
                push  offset $+8
                ret
                int   20h
                endm

anti_dis2       macro abc
                push  offset $+16
                push  offset $+8
                ret
                int   20h
                ret
                int   20h
                endm

anti_dis3       macro abc
                pushad
                call  $+8
                db    0d2h
                jmp   $+12
                pop   eax
                jmp   $+3
                dec   eax
                inc   eax
                jmp   $+3
                db    35h
                jmp   eax
                popad
                endm

anti_dis4       macro abc
                call  jump1
                ret
                jmp   $+6
                db    0c7h, 45h, 1ah, 64h
                push  eax
                push  ebx
                call  $+9
                db    8dh, 0b5h, 0d4h, 66h
                pop   ax
                cmp   esi,esi
                pop   bx
                jnz   $+5
                pop   eax
                jmp   $+5
                db    0e8h, 94h, 0d9h
                call  jump3
                jmp   esp
                cmp   edi,edi
                pop   ebx
                xchg  eax,ebx
                jnz   $-11
                jmp   koniec
                db    0c7h, 85h, 0efh

jump1:          call  jump2
                db    0ffh
                inc   dword ptr [esp]
                jmp   $+4
                int   20h
                ret

jump2:          push  ebp
                mov   ebp,esp
                sub   esp, 80h
                jnz   $+3
                db    0ffh
                add   dword ptr [esp+84h], 01
                jnz   $+4
                db    68h, 58h
                mov   esp, ebp
                pop   ebp
                ret

jump3:          call  jump2
                db    0ffh
                sub   dword ptr [esp], -02
                jnz   $+4
                db    8dh, 87h
                jmp   $+4
                int   20h
                ret
koniec:
                endm

.data

message1        db "Example of anti-disassembling macros",0
message3        db "Everything was OK. Now try debug me :)",0
message2        db "Error",0
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

                anti_dis1

                push 0
                push offset message1

                anti_dis2

                push offset message3
                push 0

                anti_dis3

                call MessageBoxA

                anti_dis4

                call ExitProcess, -1




;----------------------------------------------------------------------------
;Sets previous SEH service
;----------------------------------------------------------------------------
                push dword ptr [previous]
                call SetUnhandledExceptionFilter
;----------------------------------------------------------------------------







continue:
                call MessageBoxA,0, offset message2, offset message1,0
                call ExitProcess, -1



error:                                          ;starts a new SEH service in case of an error

                mov  esp, [delayESP]
                push offset continue
                ret

ends
end Start
