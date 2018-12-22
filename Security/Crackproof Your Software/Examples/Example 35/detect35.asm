.386
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc

Extrn SetUnhandledExceptionFilter : PROC
Extrn VirtualQuery                : PROC
.data
message1        db "Detection of an API hook on the MessageBoxA function",0
message3        db "API hook found",0
message2        db "API hook not found",0
delayESP        dd 0                    ;the ESP register saves here
previous        dd 0                    ;saves the address of the previous SEH here
Memory_structure:                       ;saves the return values of the API function VirtualQuery here
pvBaseAddress   dd 0
pvAllocBase     dd 0
dwAllocProt     dd 0
dwRegionSize    dd 0
dwState dd 0
dwProtect       dd 0
dwType  dd 0

User    db "USER32.DLL",0               ;enter the source DLL for the tested API function here

.code
Start:

;------------------------------------------------------------------------------------------------
;Sets the SEH in case of an error
;------------------------------------------------------------------------------------------------
        mov  [delayESP], esp
        push offset error
        call SetUnhandledExceptionFilter
        mov  [previous], eax
;------------------------------------------------------------------------------------------------
        mov  eax, dword ptr [Detection+1]
                     ;searches for the IAT. This will put the offset of the jump to the API function MessageBoxA from the call MessageBoxA instruction into the eax call MessageBoxA= (E8xxxxxxxx). You will actually read the xxxxxxxx value.
        add eax, offset Detection+5+2
                     ;adds the offset of the beginning of the jump table +5=call MessageBoxA= (E8xxxxxxxx)+2= jmp first_API_function (FF25xxxxxxxx)
        mov  eax, [eax]
                     ;reads the offset of the jump jmp first_API_function (FF25xxxxxxxx); you will actually read the xxxxxxxx value.
        mov  eax, [eax]
                     ;the program will eventually read the final real address of the API function routine, MessageBoxA, in the memory from the Import table
        call VirtualQuery, eax, offset Memory_structure, 4*7
                     ;calls the API function VirtualQuery with the following parameters:
                ;Eax = the address of the MessageBoxA routine in the memory offset
                ;Memory_structure = the buffer offset for the return values
                ; 4*7 = the buffer size in dword (words). The program will use this call to find the address of the API function MessageBoxA user. The address will be in the buffer at the pvAllocBase offset.
        call GetModuleHandleA, offset User      ;finds the DLL address that is supposed to use the API function (USER32.DLL)

        push eax                        ;saves the return value
;------------------------------------------------------------------------------------------------
;Sets the previous SEH service
;------------------------------------------------------------------------------------------------
        push dword ptr [previous]
        call SetUnhandledExceptionFilter
;------------------------------------------------------------------------------------------------
        pop  eax                        ;restores the return value
        test eax,eax
        jz   continue                   ;the program will jump if there was an error and eax=0

        cmp  eax, [pvAllocBase]         ;compares the addresses to see if the right module is really the user
        jnz  jump                       ;if the addresses aren't equal, the program will jump because it has found an API hook of the particular function.
continue:
        call MessageBoxA,0, offset message2,\
        offset message1,0
        call ExitProcess, -1
jump:
        call MessageBoxA,0, offset message3,\
        offset message1,0
        call ExitProcess, -1
error:          ;sets a new SEH service in case of an error.
        mov  esp, [delayESP]
        push offset continue
        ret
Detection:              ;this label may be anywhere that the called API function MessageBoxA can be.
        call MessageBoxA
ends
end Start
