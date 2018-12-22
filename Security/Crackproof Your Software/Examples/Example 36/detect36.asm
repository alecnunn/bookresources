.386
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc
Extrn SetUnhandledExceptionFilter : PROC
.data
message1        db "Anti-ProcDump trick",0
message2        db "Increased the file size in its PE header",0
delayESP        dd 0                    ;the ESP register saves here.
previous        dd 0                    ;the ESP register will save the address of the previous SEH service here.
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
        mov  eax, fs:[30h]              ;reads the Windows version
        test eax,eax                    ;tests the Windows version
        js   found_win9x                ;the program will jump if Windows 9x is running

found_winNT:                            ;the procedure for Windows NT
        mov  eax,[eax+0ch]
        mov  eax,[eax+0ch]
        add  dword ptr [eax+20h], 3000h ;increases the program's size
        jmp  end                        ;jump, finished

found_win9x:                            ;the procedure for Windows 9x
        push 0
        call GetModuleHandleA           ;finds the module handle
        test edx,edx
        jns  end                        ;the program jumps if there was an error

        cmp  dword ptr [edx+08], -1     ;tests for the value -1
        jne  end                        ;if the value is not -1, there was an error, and the program jumps

        mov  edx, [edx+4]               ;finds the address of the file's PE header
        add  dword ptr [edx+50h], 3000  ;increases the size in such a way that it will change the item SizeOfImage in the PE header
end:
;------------------------------------------------------------------------------------------------
;Sets the previous SEH service
;------------------------------------------------------------------------------------------------
        push dword ptr [previous]
        call SetUnhandledExceptionFilter
;------------------------------------------------------------------------------------------------

continue:
        call MessageBoxA,0, offset message2,\
        offset message1,0
        call ExitProcess, -1
error:          ;sets a new SEH service in case of an error.
        mov  esp, [delayESP]
        push offset continue
        ret
ends
end Start
