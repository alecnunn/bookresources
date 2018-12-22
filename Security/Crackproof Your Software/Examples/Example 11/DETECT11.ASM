.MODEL TINY
.386P


.DATA
message          db 'SoftICE detection through calling Int 2fh and its function GET DEVICE API ENTRY POINT for VXD SICE',0dh, 0ah,24h
found            db 'SoftICE active',24h
notfound         db 'SoftICE not found',24h

.CODE
.STARTUP
                lea  dx, message
                mov  ah,9                       ;function number->Show string
                int  21h                        ;call INT 21h
                                                ;show label


                xor  di,di                      ;nulls di
                mov  es,di                      ;nulls es
                mov  ax, 1684h                  ;function number GET DEVICE API ENTRY POINT
                mov  bx, 202h                   ;VxD ID pre SICE VXD
                int  2fh                        ;calls GET DEVICE API ENTRY POINT

                mov  ax, es                     ;puts the value from es into ax
                add  ax, di                     ;adds di to ax
                test ax,ax                      ;tests whether ax=0
                jnz  short jump                 ;if not, it jumps since SoftICE is active in memory

                lea  dx, notfound
                jmp  short farther

jump:
                lea  dx,  found

farther:
                mov  ah,9                       ;function number->Show string
                int  21h                        ;call INT 21h


                mov  ax,4c00h                   ;function number->Ends program
                int  21h                        ;call INT 21h



END