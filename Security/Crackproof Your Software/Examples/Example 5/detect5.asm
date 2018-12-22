.386p
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc
Extrn SetUnhandledExceptionFilter : PROC

.data
message1        db "Detection of service distance INT 1h and INT 3h",0
message3        db "SoftICE found",0
message2        db "SoftICE not found",0
delayESP        dd 0    ;the ESP register saves here
previous        dd 0    ;the ESP register will save the address of the previous SEH service here
pIDT    db 6 dup (0)    ;IDT is saved here

.code
Start:
;------------------------------------------------------------------------------------------------
;Sets SEH in case of an error
;------------------------------------------------------------------------------------------------
        mov  [delayESP], esp
        push offset error
        call SetUnhandledExceptionFilter
        mov  [previous], eax
        sidt fword ptr pIDT             ;saves IDT
        mov  eax,dword ptr [pIDT+2]   ;puts a pointer to the interrupt table into eax
        add   eax,8     ;inserts the INT 1h vector address into eax
        mov  ebx, [eax] ;inserts the INT 1h service address into ebx
        add   eax,16    ;inserts the INT 3h vector address into eax
        mov  eax, [eax] ;inserts the INT 3h service address into eax
        and  eax, 0ffffh        ;selector will not be used
        and  ebx, 0ffffh        ;even with INT 1h
        sub  eax,ebx    ;calculates the distance between interrupt services
        push eax        ;saves the result
;------------------------------------------------------------------------------------------------
;Sets previous SEH service
;------------------------------------------------------------------------------------------------
        push dword ptr [previous]
        call SetUnhandledExceptionFilter
        pop  eax        ;restores the result
        cmp  eax, 01eh  ;when 01Eh is the value in eax, SoftICE is active in memory
        jz   jump       ;and the program jumps
continue:
        call MessageBoxA,0, offset message2, offset message1,0

        call ExitProcess, -1
jump:
        call MessageBoxA,0, offset message3, offset message1,0
        call ExitProcess, -1
                ;starts a new SEH service in case of an error
error:
        mov  esp, [delayESP]
        push offset continue
        ret
ends
end Start
