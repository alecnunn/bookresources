.386p
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc

Extrn SetUnhandledExceptionFilter : PROC
Interrupt       equ 5   ; interrupt number 3 makes debugging more difficult
.DATA

message1        db "Detection by means of the Int 2Fh_GET DEVICE API ENTRY POINT for VXD SIWVID",0
message2        db "Debugger not found",0
message3        db "Debugger found",0
delayESP        dd 0    ;the ESP register saves here
previous        dd 0    ;the ESP register will save the address of the previous SEH service here
mark    db 0    ;a value of 1 will be set here if SoftICE is active in memory
.CODE
Start:
;------------------------------------------------------------------------------------------------
;Sets SEH in case of an error
;------------------------------------------------------------------------------------------------
        mov  [delayESP], esp
        push offset error
        call SetUnhandledExceptionFilter
        mov  [previous], eax
;------------------------------------------------------------------------------------------------
        push edx
        sidt [esp-2]                    ;reads IDT into the stack
        pop  edx
        add  edx,(Interrupt*8)+4        ;reads the vector of the required interrupt (INT 5h)
        mov  ebx,[edx]
        mov  bx,word ptr [edx-4]        ;reads the address of the old service of the required interrupt (INT 5h)

        lea  edi, InterruptHandler
        mov  [edx-4],di
        ror  edi,16                     ;sets the new service of the interrupt (INT 5h)
        mov  [edx+2],di

        push ds                         ;saves registers
        push es                         ;for security
        int  Interrupt                  ;jump into the Ring0 (newly defined INT 5h service)
        pop  es                         ;restores registers
        pop  ds

        push ds                         ;saves registers
        push es                         ;for security
        mov  ah,43h
        int  68h                        ;calls INT 68h and the 43h function (your new INT 68h service will be called)
        stc
        int  Interrupt                  ;jump into Ring0 (your INT 5h service)
        pop  es                         ;restores registers
        pop  ds

        mov  [edx-4],bx                 ;sets the original service
        ror  ebx,16                     ;of the INT 5h interrupt
        mov  [edx+2],bx
;------------------------------------------------------------------------------------------------
;Sets the previous SEH service
;------------------------------------------------------------------------------------------------
        push dword ptr [previous]
        call SetUnhandledExceptionFilter
;------------------------------------------------------------------------------------------------
        cmp  byte ptr mark, 1           ;tests to see if a mark was set indicating that SoftICE is active in memory
        jz   jump
continue:
        call MessageBoxA,0, offset message2, offset message1,0
        call ExitProcess, -1
jump:
        call MessageBoxA,0, offset message3, offset message1,0
        call ExitProcess, -1
error:          ;sets a new SEH service in case of an error
        mov  esp, [delayESP]
        push offset continue
        ret
;------------------------------------------------------------------------------------------------
;The new INT 5h service (runs in Ring0)
;------------------------------------------------------------------------------------------------
InterruptHandler:
        pushad                          ;saves registers
        jc   uninstall                  ;tests our mark when it is set; jumps uninstall our service INT 68 h
        mov  eax, 68h                   ;the interrupt number where your new service has been set.
        mov  esi, offset HookInt68      ;the address of your new service
        db   0cdh,20h                   ;calls VxD
        dd   000010041H                 ;Hook_V86_Int_Chain
;           VMMCall Hook_V86_Int_Chain
        mov  eax, 68h                   ;the interrupt number where your new service has been set.
        mov  esi, OFFSET HookInt68
        db   0cdh,20h                   ;calls VxD
        dd   000010080H                 ;Hook_PM_Fault
;            VMMCall Hook_PM_Fault
        popad                           ;restores registers
        iretd                           ;jump back to ring3; jumps here when it has to uninstall our service INT 68 h
uninstall:
        mov  eax, 68h
        mov  esi, OFFSET HookInt68      ;the interrupt number where your new service has been set.
        db   0cdh,20h                   ;calls VxD
        dd   000010118H                 ;UnHook_V86_Int_Chain
;            VMMCall UnHook_V86_Int_Chain
        mov  eax, 68h                   ;the interrupt number where your new service has been set.
        mov  esi, OFFSET HookInt68      ;the address of your new service
        db   0cdh,20h   ;calls VxD
        dd   00001011AH ;UnHook_PM_Fault
;         VMMCall UnHook_PM_Fault
        popad                           ;restores registers
        iretd                           ;jumps back into ring3
;------------------------------------------------------------------------------------------------
;The new INT 68h service
;------------------------------------------------------------------------------------------------
HookInt68:
        pushfd                          ;
        pushad                          ;saves registers
        xor  di, di                     ;
        mov  es, di                     ;nulls es:di
        mov  bx, 7A5Fh                  ;VxD ID for SIWVID VxD
        mov  ax, 1684h                  ;number of the GET DEVICE API ENTRY POINT function
        int 2Fh                         ;calls the GET DEVICE API ENTRY POINT
;       mov  ax, es     ;puts the value from es into ax
;       add  ax, di     ;adds di to ax
;------------------------------------------------------------------------------------------------
;The preceding two lines must always be left out because the ES is always set to a value ;different from 0 when calling the INT 2Fh. This is the major difference between this and the DOS ;version of this detection.
;------------------------------------------------------------------------------------------------
        mov  ax, di                     ;saves only the DI register
        test ax, ax                     ;tests to see if ax=0
        jz   short none                 ;if yes it ends because SoftICE isn't active in memory
        mov  byte ptr mark, 1           ;sets a mark that SoftICE is active in memory
none:
        popad                           ;
        popfd                           ;restores registers
        ret                             ;jump back from the service
ends
end Start
