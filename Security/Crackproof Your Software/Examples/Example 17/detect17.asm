.386
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
includelib advapi32.lib
include w32.inc
Extrn SetUnhandledExceptionFilter        : PROC
Extrn RegOpenKeyExA                      : PROC         ;a function exported from the ADVAPI32.DLL
Extrn RegQueryValueExA                   : PROC         ;a function exported from the ADVAPI32.DLL

.DATA
message1        db "Searching for SoftICE installation directory by means of registers",0
message3        db "SoftICE found",0
message2        db "SoftICE not found",0
message4        db "SoftICE installed in the following directory: ",0
message5        db "Installed SoftICE version: ",0
delayESP        dd 0                    ;the ESP register saves here
previous        dd 0                    ;the ESP register will save the address of the previous SEH service here
result  dd 0
size    dd 5                            ;the size of the data buffer for the SoftICE version
size2   dd 80h                          ;the size of the data buffer for the SoftICE installation directory
subkey  db "Software\NuMega\SoftICE\",0
current_ver     db "Current Version",0
install_dir     db "InstallDir",0
data_buffer     db 200 dup (0)          ;the SoftICE version number will be saved here
value_buffer    db  20 dup (0)
data_buffer2    db 200 dup (0)          ;the location where SoftICE is installed will be saved here

.CODE
Start:

;------------------------------------------------------------------------------------------------
;Sets SEH if there is an error
;------------------------------------------------------------------------------------------------
        mov  [delayESP], esp
        push offset error
        call SetUnhandledExceptionFilter
        mov  [previous], eax
;------------------------------------------------------------------------------------------------
        push offset result              ;the result will be saved here
        push 20016h                     ;access type
        push 0                          ;non-essential (must be 0)
        push offset subkey              ;string with the subkey name
        push 80000002h                  ;HKEY_LOCAL_MACHINE = 80000002 where a subkey will be opened
        call RegOpenKeyExA              ;opens the required access in registers and saves the result for further handling
        test eax,eax
        jnz  notfound                   ;jump if error

        push offset size                ;size of the data buffer
        push offset data_buffer         ;address of the data buffer
        push offset value_buffer        ;address of the value buffer
        push 0                          ;non-essential (must be 0)
        push offset current_ver         ;name of the value for comparison (Current Version)
        push result                     ;where the result was saved
        call RegQueryValueExA           ;reads the installed version of SoftICE into the data_buffer
        test eax,eax
        jnz  notfound                   ;if an error occurred, the program jumps

        push offset size2               ;size of the data buffer
        push offset data_buffer2        ;address of the data buffer
        push offset value_buffer        ;address of the value buffer
        push 0                          ;non-essential (must be 0)
        push offset install_dir         ;name of the value for comparison
        push result                     ;where the result was saved
        call RegQueryValueExA           ;reads the SoftICE installation directory into the data_buffer
        test eax,eax
        jnz  notfound                   ;if an error occurred, the program jumps

        inc  al                         ;increases the al register by 1 (if there was no error eax=0) to show that everything is OK
        jmp  short ok

notfound:
        xor eax,eax                     ;nulls the eax register to show that there was an error
ok:
        push eax                        ;saves the result
;------------------------------------------------------------------------------------------------
;Sets the previous SEH service
;------------------------------------------------------------------------------------------------
        push dword ptr [previous]
        call SetUnhandledExceptionFilter

        pop  eax                        ;restores the result
        test eax,eax
        jnz  jump                       ;jumps if everything was OK

continue:
        call MessageBoxA,0, offset message2,\
        offset message1,0               ;prints out an error message
        call ExitProcess, -1
jump:
        call MessageBoxA,0, offset message3,\
        offset message1,0               ;prints out a message that SoftICE was found
        call MessageBoxA,0, offset data_buffer,\
        offset message5,0               ;prints out the SoftICE version number
        call MessageBoxA,0, offset data_buffer2,\
        offset message4,0               ;prints out location where SoftICE is installed
        call ExitProcess, -1
error:          ;sets a new SEH service in case of an error
        mov  esp, [delayESP]
        push offset continue
        ret
ends
end Start
