.386
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc

Extrn SetUnhandledExceptionFilter : PROC

.DATA
message1        db "Breakpoint detection by searching for Int 3h",0
message3        db "Breakpoint found",0
message2        db "Breakpoint not found",0
delayESP        dd 0                    ;the ESP register saves here
previous        dd 0                    ;the ESP register will save the address of the previous SEH service here
current_import  dd 0                    ;the address of the imported function currently being tested is saved here

.code
Start:
;------------------------------------------------------------------------------------------------
;Sets SEH in case of an error
;------------------------------------------------------------------------------------------------
        mov  [delayESP], esp
        push offset error
        call SetUnhandledExceptionFilter
        mov  [previous], eax
;------------------------------------------------------------------------------------------------
        lea  edi, Start                 ;the starting address of the program will be put into the edi
        mov  ecx, End-Start+1           ;the length of the program, that is, the number of tested bytes, will be put into the ecx
        mov  eax,0BCh
        add eax, 10h                    ;puts 0CCh into the eax (this is the prefix of the INT 3h instruction).
;Note! Do not use mov  eax,0CCh or you will find this value in the program and think that it is a breakpoint.
        repnz scasb                     ;searches for a breakpoint in the program
        test ecx,ecx                    ;tests to see if ecx=0; if not, it has found a breakpoint
        jne  found                      ;and the program jumps
        lea  eax, Imports+2             ;puts the IAT (first function) into the eax register
        mov  dword ptr [current_import], eax
start_searching:
        mov  eax, dword ptr [current_import]   ;puts the imported function to be tested into the eax
        cmp  dword ptr [eax],0          ;tests to see if it is 0; if so, then this is the end of the Import table (the table of the imported functions)
        je   notfound                    ;the program jumps if it hasn't found any breakpoints
        mov  ecx, [eax]                 ;this reads the address where the imported function is located; that is the address of the beginning of its routine
        mov  ecx, [ecx]                 ;reads the address of the beginning of the routine of the imported function
        mov  eax, 0BCh
        add  eax, 10h                   ;puts 0CCh into the eax (the prefix of the INT 3h instruction).
;------------------------------------------------------------------------------------------------
;Note! Do not use mov  eax, 0CCh or you will find this value in the program and think that it is a breakpoint.
;------------------------------------------------------------------------------------------------
        cmp  byte ptr [ecx], al         ;tests for the breakpoint's presence in the tested imported function
                                        ;the program will search for 0CCh (actually Int 3h) at the beginning of its routine
        je   found                      ;and if a breakpoint is found it will end
        add  [current_import], 6        ;sets to another imported function in the IAT
        jmp start_searching             ;jumps to search for a breakpoint in the next imported function
found:
        mov  eax, 1                     ;sets eax to 1 to show that the program has found a breakpoint
        jmp farther
notfound:
        xor  eax, eax                   ;sets eax to 0 to show that it hasn't found a breakpoint
farther:
        push eax                        ;saves the return value
;------------------------------------------------------------------------------------------------
;Sets the previous SEH service
;------------------------------------------------------------------------------------------------
        push dword ptr [previous]
        call SetUnhandledExceptionFilter
;------------------------------------------------------------------------------------------------
        pop  eax                        ;restores the return value
        test eax, eax
        jnz  jump
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
End:
Imports:                ;this label must be at the very end of the program, since the IAT begins there
ends
end Start
