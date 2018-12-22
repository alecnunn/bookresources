.386
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc


;There is no need for any API calls

.data

message1        db "Running commands of the BCHK interface of the SoftICE",0
message3        db "SoftICE commands functional",0
message2        db "SoftICE isn't active, an error occurred, the program was traced or breakpoints have been set",0
mark            db 0                             ;a sign that everything went OK

.code

Start:
                xor  eax,eax                    ;nulls the eax register
                call real_start                 ;jumps to the real start of the program

;----------------------------------------------------------------------------
;Starts a new SEH service in case of an error
;----------------------------------------------------------------------------
                mov  ecx, [esp+0ch]             ;reads the context address
                sub  eax,eax
                inc  dword ptr [ecx+0B8h]       ;increases the EIP register by 1 and moves 1 byte farther in the program
                inc  byte ptr mark              ;sets the mark on value 1 to show that everything went OK. If this
                                                ;is any value other than 1, your SEH service has been called
                                                ;repeatedly, which is bad since SoftICE isn't active in memory. If
                                                ;the mark is 0, your SEH service wasn't called at all, which means
                                                ;that the program has been traced.
                ret                             ;returns back to the program from the SEH service
;----------------------------------------------------------------------------

real_start:
                push dword ptr fs:[eax]         ;saves the original SEH service
                mov  dword ptr fs:[eax], esp    ;Sets your new SEH service (which you have ended by means of CALL) to be able to save the following return address into the stack. This is your new SEH service.


                mov  eax,4                      ;"magic" values for finding out
                mov  ebp,"BCHK"                 ;if SoftICE is active
                int  3h                         ;calls interrupt 3. (If SoftICE isn't active, an error will occur and our new SEH service will be called.)

                cmp  eax,4                      ;tests to see if the eax has been changed
                jz   continue                   ;if not, SoftICE isn't active in memory and the program jumps


                mov  eax, 0                     ;Get ID function (returns an ID for SoftICE)
                mov  ebp,"BCHK"                 ;calls the BCHK interface 
                int  3h                         ;calls SoftICE

                cmp  eax, 0ff01h                ;tests to see if the return value is 0FF01h
                jnz  continue                   ;if not, SoftICE isn't active in memory

                sub  edi,edi                    ;use the edi register as a mark if it is necessary to deactivate breakpoints. You will set this to 0 to say that it isn't necessary.
                mov  eax,8                      ;Deactivates the lowest breakpoint function (deactivates the nearest breakpoint)
                int  3h                         ;calls SoftICE

                inc  eax                        ;returns -1 (0FFFFFFFFh) if an error occurs
                jz   notneeded                 ;jumps if there was an error; it is not necessary to deactivate breakpoints

                dec  edi                        ;says that it is necessary to deactivate breakpoints
                dec  eax                        ;returns the error value (-1) into the eax
                xchg eax,ebx                    ;exchanges values in the unneeded registers
notneeded:

                mov  ecx, cs                    ;selector (segment) of the breakpoint
                lea  edx, breakpoint            ;address where you will set the breakpoint
                mov  bh, 0+0                    ;0 represents the breakpoint type (BT_EXECUTE) + 0 represents the length of the breakpoint 
                mov  eax, 0ah                   ;sets the breakpoint function (which sets a breakpoint)
                int  3h                         ;calls SoftICE

                mov  eax,9                      ;activate the breakpoint function to explicitly activate breakpoints
                int  3h                         ;calls SoftICE

                nop

breakpoint:
                nop                             ;SoftICE will stick here during tracing, or an error will occur here and the program will call your new SEH service.

                mov  eax, 0ch                   ;get breakpoint status function (saves the DR6 register into the eax register)
                int  3h                         ;calls SoftICE

                push eax                        ;saves the status (DR6)

                mov  eax, 0bh                   ;clears the breakpoint function (removes a breakpoint)
                int  3h                         ;calls SoftICE

                inc  edi
                je   notneeded2                 ;tests to see if it is necessary to deactivate the breakpoint

                mov  eax,8                      ;Deactivate Lowest Breakpoint function (deactivates the lowest breakpoint)
                int  3h                         ;calls SoftICE
notneeded2:

                pop  ecx                        ;restores the status (DR6)

                xor  eax,eax
                pop  dword ptr fs:[eax]         ;sets the original SEH service
                pop  eax                        ;clears the stack

                xchg eax,ecx                    ;puts the status into the eax

;                bsf  eax,eax                   ;because TASM has a problem translating this function, you will enter its byte form
                db   0fh, 0bch, 0c0h            ;tests if the program was traced. If so, there will be a value other than 0 in the eax 

                cmp  byte ptr mark, 1
                jnz  continue                   ;tests for an error

                cmp  al,bl                      ;tests to see if everything was OK
                jz   jump                       ;if so, the program jumps

continue:
                call MessageBoxA,0, offset message2, offset message1,0
                call ExitProcess, -1

jump:

                call MessageBoxA,0, offset message3, offset message1,0
                call ExitProcess, -1



ends
end Start
