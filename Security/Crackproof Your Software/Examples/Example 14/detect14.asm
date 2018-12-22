.386p
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc

Extrn SetUnhandledExceptionFilter : PROC
Interrupt       equ 5   ;interrupt numbers 1 or 3 will make debugging more difficult
.DATA

message1        db "Detection by means of VxDCALL - VMMCall Get_DDB",0
message2        db "Debugger not found",0
message3        db "Debugger found",0
delayESP        dd 0                    ;the ESP register saves here
previous        dd 0                    ;the ESP register will save the address of the previous SEH service here
Device_ID       dd 202h                 ;the VxD ID for SICE
Device_ID2      dd 7a5Fh                ;the VxD ID for SIWVID
Device_Name     dd 0                    ;the VxD name will not be needed

.CODE
Start:

;------------------------------------------------------------------------------------------------
;Sets SEH if there is an error
;------------------------------------------------------------------------------------------------
        mov  [delayESP],esp
        push offset error
        call SetUnhandledExceptionFilter
        mov  [previous], eax
        push edx
        sidt [esp-2]                    ;reads IDT into the stack
        pop  edx
        add   edx,(Interrupt*8)+4       ;reads the vector of the required interrupt (INT 5h)

        mov  ebx,[edx]
        mov  bx,word ptr [edx-4]        ;reads the address of the required interrupt's old service

        lea   edi,InterruptHandler
        mov  [edx-4],di
        ror  edi,16                     ;sets a new interrupt service
        mov  [edx+2],di

        push ds                         ;saves registers for security
        push es
        int  Interrupt                  ;jumps into ring0 (newly defined service INT 5h)
        pop  es                         ;restores registers
        pop  ds

        mov  [edx-4],bx                 ;sets the original service of the interrupt (INT 5h)
        ror  ebx,16
        mov  [edx+2],bx

        push eax                        ;saves the return value
;------------------------------------------------------------------------------------------------
;Sets the previous SEH service
;------------------------------------------------------------------------------------------------
        push dword ptr [previous]
        call SetUnhandledExceptionFilter
;------------------------------------------------------------------------------------------------
        pop  eax                        ;restores the return value
        cmp   eax,1                     ;tests to see if the return value is 1. If so, SoftICE is active in memory
        jz   jump                       ;and the program jumps
continue:
        call MessageBoxA,0, offset message2,\
        offset message1,0
        call ExitProcess, -1
jump:
        call MessageBoxA,0, offset message3,\
        offset message1,0
        call ExitProcess, -1
error:          ;sets a new SEH service in case of an error
        mov  esp, [delayESP]
        push offset continue
        ret
;------------------------------------------------------------------------------------------------
;Your new INT 5h service (runs in Ring0)
;------------------------------------------------------------------------------------------------

InterruptHandler:
        mov  eax, Device_ID             ;202h is for SICE VxD ID
        mov  edi, Device_Name           ;This will be set only if Device_ID is unknown, otherwise it will be 0 (as is the case here)
;        VMMCall Get_DDB                 ;calls the service
        db   0cdh, 20h                  ;this is actually INT 20h
        dd   000010146H                 ;0001h is a type of call=VMM 0146h Get_DDB function is called

        test ecx, ecx                   ;ecx=DDB or 0 if VxD is not installed
        jnz  Debugger_detect            ;if SoftICE wasn't found by the first method, the program will try the next one

        mov  eax, Device_ID2            ;7a5Fh for SIWVID VxD ID
        mov  edi, Device_Name           ;this will be set only if Device_ID is unknown, otherwise it is 0 (as is the case here)
;        VMMCall Get_DDB                 ;service call
        db   0cdh, 20h                  ;this is actually INT 20h
        dd   000010146H                 ;0001h is a type of call=VMM 0146h Get_DDB function is called
        test ecx, ecx                   ;ecx=DDB or 0 if VxD is not installed
        jnz  Debugger_detect
        xor  eax,eax                    ;nulls EAX to show that SoftICE isn't active in memory
        iretd                           ;jump back to ring3
Debugger_detect:
        xor  eax,eax
        inc  al                         ;sets value 1 into EAX to show that SoftICE is active in memory
        iretd                           ;jump back to ring3
ends
end Start
