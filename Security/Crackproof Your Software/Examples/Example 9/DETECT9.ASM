.MODEL TINY
.386P


.DATA

message            db "SoftICE detection by means of it's INT 41h", 0dh, 0ah, 24h
found              db 'SoftICE active',24h
notfound           db 'SoftICE not found',24h

.CODE
.STARTUP

                lea  dx, message
                mov  ah,9                       ;function number->Show string
                int  21h                        ;INT 21h call
                                                ;shows label


                xor  ax,ax                      ;nulls ax
                mov  es,ax                      ;puts 0 into es (sets interrupt vector segment)
                mov  bx,cs                      ;puts the program segment into bx
                lea  dx, new_int41             ;puts the offset part of the new int 41h service into dx
                xchg dx, es:[41h*4]             ;sets an offset part of the new int 41h service address and puts the
                                                ;offset part of the old service address into dx
                xchg bx, es:[41h*4+2]           ;sets the segment part of the new int 41h service address and puts
                                                ;the segment part of the old service address into bx
                in   al, 40h                    ;reads a value into al

                xor  cx,cx                      ;nulls cx
                int  41h                        ;calls INT 41h. If SoftICE isn't active in memory, the program will
                                                ;perform the new service

                xchg dx, es:[41h*4]             ;sets the offset part of the original INT 41h service address

                xchg bx, es:[41h*4+2]           ;sets the segment part of the original int 41h service address

                cmp  cl,al                      ;compares cl and al and if the result is negative SoftICE is active
                                                ;in memory
                jnz  short jump                 ;jumps if SoftICE is active in memory

                lea  dx, notfound
                jmp  short farther

jump:
                lea    dx, found

farther:
                mov    ah,9                     ;function number->Show string
                int    21h                      ;INT 21h call

                mov    ax,4c00h                 ;function number->Ends program
                int    21h                      ;INT 21h call

new_int41 PROC                                  ;this is your new INT 41h service

                mov cl,al                       ;moves the value from al into cl to show that SoftICE isn't active
                                                ;in memory

                iret                            ;returns from the service

new_int41 ENDP

END