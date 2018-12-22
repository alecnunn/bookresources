.MODEL TINY
.386P


.DATA

message          db "SoftICE detection by means of it's INT 68h service identification",0dh, 0ah,24h
found            db 'SoftICE active',24h
notfound         db 'SoftICE not found',24h

.CODE
.STARTUP
                lea    dx, message
                mov    ah,9                      ;function number->Show string
                int    21h                       ;call INT 21h
                                                 ;show label

                xor    ax,ax                     ;nulls ax
                mov    es,eax                    ;sets 0 into es
                mov    bx, word ptr es:[68h*4]   ;puts the offset part of the INT 68h service address into bx
                mov    es, word ptr es:[68h*4+2] ;puts the segment part of the INT 68h service address into es
                mov    eax, 0f43fc80h            ;puts the first bytes of the SoftICE INT 68h service into eax
                cmp    eax, dword ptr es:[ebx]   ;tests to see if SoftICE is active in memory
                jz     short jump                ;jumps if SoftICE is active in memory

                lea    dx, notfound
                jmp    short farther

jump:
                lea    dx, found

farther:
                mov    ah,9                      ;function number->Show string
                int    21h                       ;call INT 21h



                mov    ax,4c00h                  ;function number->Ends program
                int    21h                       ;call INT 21h


END