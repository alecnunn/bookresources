.386p
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc

Extrn SetUnhandledExceptionFilter : PROC
Interrupt       equ 5   ;interrupt 3 will make debugging more difficult

.DATA
message1        db "Detection by means of the Int 2Fh_GET DEVICE API ENTRY POINT for VxD SICE",0

message2        db "Debugger not found",0
message3        db "Debugger found",0
delayESP        dd 0    ;the ESP register will be saved here
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
        sidt [esp-2]                   ;reads IDT into the stack
        pop  edx
        add  edx, (Interrupt*8)+4      ;reads a vector of the required interrupt (INT 5h)

        mov  ebx,[edx]
        mov  bx,word ptr [edx-4]       ;reads an address of the old service of the required interrupt (INT 5h)
        lea  edi,InterruptHandler
        mov  [edx-4],di
        ror  edi,16                    ;sets a new service for the interrupt (INT 5h)
        mov  [edx+2],di

        push ds                        ;saves registers for
        push es                        ;security
        int Interrupt                  ;jump into Ring0 (the newly defined service INT 5h)
        pop  es                        ;restores registers
        pop  ds

        push ds                        ;saves registers for
        push es                        ;security
        mov  ah, 43h
        int  68h                       ;calls INT 68h and function 43h (which will call your new service INT 68h)

        stc                            ;sets a mark that your service INT 68h may be cancelled
        int  Interrupt                 ;jumps into Ring0 (your new service INT 5h)
        pop  es                        ;restores registers
        pop  ds

        mov  [edx-4], bx               ;sets the original service
        ror  ebx,16                    ;of the INT 5h interrupt
        mov  [edx+2], bx
;------------------------------------------------------------------------------------------------
;Sets the previous SEH service
;------------------------------------------------------------------------------------------------
        push dword ptr [previous]
        call SetUnhandledExceptionFilter
;------------------------------------------------------------------------------------------------
        cmp  byte ptr mark, 1          ;tests to see whether a mark has been set that SoftICE is active in memory
        jz   jump                      ;if yes the program will end
continue:
        call MessageBoxA,0, offset message2, offset message1,0
        call ExitProcess, -1
jump:
        call MessageBox, 0, offset message3, offset message1,0
        call ExitProcess, -1
error:                                 ;sets a new SEH service in case of an error
        mov  esp, [delayESP]
        push offset continue
        ret
;------------------------------------------------------------------------------------------------
;A new INT 5h service (runs in Ring0)
;------------------------------------------------------------------------------------------------
InterruptHandler:
        pushad                         ;saves registers
        jc   uninstall                 ;tests your mark; if it is set it will jump to uninstall your INT 68h service
        mov  eax, 68h                  ;interrupt number where it will set your new service
        mov  esi, offset HookInt68     ;address of your new service
        db   0cdh, 20h                 ;calls the VxD call
        dd   000010041H                ;Hook_V86_Int_Chain
;      VMMCall Hook_V86_Int_Chain
        mov  eax, 68h                  ;interrupt number where it will set your new service
        mov  esi, OFFSET HookInt68     ;address of your new service
        db   0cdh, 20h                 ;calls the VxD call
        dd   000010080H                ;Hook_PM_Fault
;       VMMCall Hook_PM_Fault
        popad                          ;restores registers
        iretd                          ;jump back to Ring3
uninstall:                             ;the program will jump here if it is to uninstall your INT 68h service
        mov  eax, 68h                  ;interrupt number where your new service has been set
        mov  esi, OFFSET HookInt68     ;the address of your new service
        db   0cdh, 20h                 ;calls the VxD call
        dd   000010118H                ;UnHook_V86_Int_Chain
;        VMMCall        UnHook_V86_Int_Chain
        mov  eax, 68h                  ;the interrupt number where your new service has been set
        mov  esi, OFFSET HookInt68      ;the address of your new service
        db   0cdh, 20h                  ;calls the VxD call
        dd   00001011AH                 ;UnHook_PM_Fault
;        VMMCall UnHook_PM_Fault
        popad                           ;restores registers
        iretd                           ;jump back to ring3
;------------------------------------------------------------------------------------------------
;The new service INT 68h
;------------------------------------------------------------------------------------------------
HookInt68:
        pushfd                          ;
        pushad                          ;saves registers
        xor  di, di                     ;
        mov  es, di                     ;nulls es:di
        mov  bx, 202h                   ;VxD ID for SICE VxD
        mov  ax, 1684h                  ;GET DEVICE API ENTRY POINT function number
        int  2Fh                        ;calls the GET DEVICE API ;ENTRY POINT
;------------------------------------------------------------------------------------------------
;The preceding two lines must always be left out because the ES is always set to a value other than 0 when calling the INT 2Fh. This is the main difference between the DOS version and the 32-bit Windows version of this detection.
;------------------------------------------------------------------------------------------------
        mov  ax, di                     ;saves only the DI register
        test ax, ax                     ;tests if ax=0
        jz   short none                 ;if yes, it will jump because SoftICE isn't active in memory
        mov  byte ptr mark, 1           ;sets a mark that SoftICE is active
none:
        popad                           ;
        popfd                           ;restores registers
        ret                             ;jumps back from the service
ends
end Start
