.386
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc

include pe.inc
Extrn SetUnhandledExceptionFilter : PROC
.DATA
message1        db "Detection by means of the VxDCALL called through Kernel32!ORD_0001",0
message3        db "SoftICE found",0
message2        db "SoftICE not found",0
delayESP        dd 0                    ;the ESP register saves here
previous        dd 0                    ;the ESP register will save the address of the previous SEH service here
KernelAdress    dd 0                    ;the address of the beginning (base) of the KERNEL32 in memory
a_VxDCall       dd 0                    ;the address for calling VxDCall (Kernel32!ORD_0001)
.CODE
Start:
        mov  ecx,[esp]                  ;puts the address from which it was called into the ecx. (This is always somewhere in KERNEL32.DLL.)
        push ecx                        ;saves the address
;------------------------------------------------------------------------------------------------
;Sets SEH if there is an error
;------------------------------------------------------------------------------------------------
        mov  [delayESP],esp
        push offset error
        call SetUnhandledExceptionFilter
        mov  [previous], eax

        pop  ecx                        ;restores the address
GetKrnlBaseLoop:                        ;starts searching for the beginning address (base) of the KERNEL32.DLL in memory
        xor  edx,edx                    ;nulls the edx register
        dec  ecx                        ;searches backward
        mov  dx,[ecx+03ch]              ;Tries to read parts of the file header from the MZ start of the PE header
        test dx,0f800h                  ;tests to see if the correct value was found
        jnz  GetKrnlBaseLoop            ;if not, the program jumps to search farther
        cmp  ecx,[ecx+edx+34h]          ;compares the actual address with the address where this PE file (KERNEL32.DLL) was supposed to be loaded (its image base)
;------------------------------------------------------------------------------------------------
;Basically, this tests to see if it's found the beginning of KERNEL32.DLL
;------------------------------------------------------------------------------------------------
        jnz  GetKrnlBaseLoop            ;if divided, the program jumps to search farther

        mov  [KernelAdress],ecx         ;it found the beginning address (base) of the KERNEL32.DLL and saves it
        mov  ebx,ecx                    ;puts the beginning (base) address of the KERNEL32.DLL into EBX
        or   ebx,ebx
        jz   continue                   ;if the beginning of the KERNEL32.DLL wasn't found, there has been an error and it will jump (if EBX=0)

        mov  eax,dword ptr [ebx+3ch]    ;reads the file header from the MZ part of the PE header
        add   eax,ebx                   ;sets the EAX register at the beginning of the PE header
        mov  edi,dword ptr [eax+NT_OptionalHeader. \
                              OH_DirectoryEntries. \
                                        DE_Export. \
        DD_VirtualAddress]
;------------------------------------------------------------------------------------------------
;This is actually mov  edi, dword ptr [eax+78h]
;------------------------------------------------------------------------------------------------
                                        ;reads the relative virtual address (RVA) from the Export table
        add   edi,ebx                   ;sets on Export table in the KERNEL32.DLL
;------------------------------------------------------------------------------------------------
;The program searches for the Kernel32!ORD_0001 call address entrance values: ebx = base address of the KERNEL32.DLL, and edi = address of the export table in the KERNEL32.DLL file
;------------------------------------------------------------------------------------------------
        mov  esi,dword ptr [edi+ED_AddressOfFunctions]
;This is actually mov  esi, dword ptr [edi+1Ch]
                                        ;reads the RVA address for the list of exported functions
        add   esi,ebx                   ;sets on the list of the exported functions
        xor  edx,edx                    ;nulls the edx register (there will be a counter here).
address_loop:
        cmp  edx,dword ptr [edi+ED_NumberOfFunctions]
;------------------------------------------------------------------------------------------------
;This is actually mov  esi, dword ptr [edi+14h]
;------------------------------------------------------------------------------------------------
                                        ;tests edx (counter) against the number of exported functions to determine if all numbers have been tested
        jae continue                    ;if yes the program jumps because VxDCall (Kernel32!ORD_0001) wasn't found; this is an error

        mov  ecx,00000008h-01h          ;puts 7 into the cx register and, as a result, repeats 8 times
function_loop:
        inc  edx                        ;increases the edx register (counter) by 1
        lodsd                           ;reads the address of the exported function
        cmp  eax,dword ptr [esi]        ;tests to see if this is the beginning of the PE header of the KERNEL32.DLL
        jne address_loop                ;if not, the program jumps to search farther
        loop function_loop              ;if yes, it tests 7 more times

        add  eax,ebx                    ;the RVA address for the VxDCall (Kernel32!ORD_0001) is in the eax register. By adding the value from ebx, the beginning (base) address of the KERNEL32.DLL in memory, it sets on the real VxDCall address

        mov  dword ptr [a_VxDCall], eax ;saves the address for the VxDCall (Kernel32!ORD_0001)
;------------------------------------------------------------------------------------------------
; SoftICE detection starts here
;------------------------------------------------------------------------------------------------
        push 0000004fh                  ;4fh function, the same as in the "Detecting SoftICE by Detecting a Change in the INT 41h Service" example.
        push 002a002ah                  ;the upper word determines which type of VxD call is called (VWIN32); the lower word determines which function is called (VWIN32_Int41Dispatch)
        call [a_VxDCall]                ;calls VxDCall (KERNEL32!ORD_0001)

        push eax                        ;saves the return value
;------------------------------------------------------------------------------------------------
;Sets the previous SEH service
;------------------------------------------------------------------------------------------------
        push dword ptr [previous]
        call SetUnhandledExceptionFilter

        pop  eax                        ;restores the return value

        cmp  ax, 0f386h                 ;if SoftICE is active in memory, the return value will be 0f386h
        jz   jump                       ;and the program ends
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
ends
end Start
