.386
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc

;------------------------------------------------------------------------------------------------
;While you do not need any API calls for detection, you will use them for printing out the result and for ending the program
;------------------------------------------------------------------------------------------------

.DATA

message1        db "Tracer detection using the Trap flag",0
message3        db "Tracer found",0
message2        db "Tracer not found",0
DelayESP        dd 0                    ;the ESP register saves here
Previous        dd 0                    ;the ESP register saves the address of the previous SEH service here
mark            db 0                    ;sets a mark showing whether it went into the xhandler
.CODE
Start:
        call real_start                 ;jumps to the real start of the program

;------------------------------------------------------------------------------------------------
;A new SEH service in case of an error (xhandler)
;------------------------------------------------------------------------------------------------
        inc  mark                       ;increases the eax by one to show that it isn't being traced
        sub  eax,eax                    ;nulls the eax because of the future setting of the original SEH service
        ret                             ;returns to the program
real_start:
        xor  eax,eax                    ;nulls the eax register
        push dword ptr fs:[eax]         ;saves the original SEH service
        mov  fs:[eax], esp              ;sets your new SEH service. Because you jumped by means of CALL, the following return address is saved into the stack. This will be your new SEH service.
        pushfd                          ;saves the flag registers
        or byte ptr [esp+1], 1          ;sets the Trap flag in Eflags, which means that the program will launch EXCEPTION SINGLE STEP, and your new SEH service will be called
        popfd                           ;restores the flag registers with the previously set Trap flag. The whole process runs
        nop                             ;and xhandler is called
        pop  dword ptr fs: [eax]
        pop  ebx                        ;sets the previous SEH service and clears the stack
        dec  mark                       ;decreases the mark by 1. If it is -1 (0FFFFFFFFh) after the decrease, then a tracer is active in memory because the xhandler hasn't been called
        js   jump                       ;and the program jumps
continue:
        call MessageBoxA,0, offset message2,\
        offset message1,0
        call ExitProcess, -1
jump:
        call MessageBoxA,0, offset message3,\
        offset message1,0
        call ExitProcess, -1
ends
end Start
