.MODEL TINY
.386P


.DATA
message         db 'Detekcia Soft-Ice volanim Int 2fh a jeho funkcie GET DEVICE API ENTRY POINT pre VXD SIWVID',0dh, 0ah,24h
found           db 'SoftICE active',24h
notfound        db 'SoftICE not found',24h

.CODE
.STARTUP
                lea  dx, message
                mov  ah,9                       ;function number->Show string
                int  21h                        ;call INT 21h
                                                ;show label


                xor  di,di                      ;nulls di
                mov  es,di                      ;nulls es
                mov  ax, 1684h                  ;function number GET DEVICE API ENTRY POINT
                mov  bx, 7a5fh                  ;VxD ID pre SIWVID VXD
                int  2fh                        ;vola GET DEVICE API ENTRY POINT

                mov  ax, es                     ;hodnotu z es da do ax
                add  ax, di                     ;k ax pripocita di
                test ax,ax                      ;testuje, ci je ax=0
                jnz  short jump                 ;ak nie je skoci, pretoze Soft-Ice je aktivny v pamati

                lea  dx, notfound
                jmp  short farther

jump:
                lea  dx, found

farther:
                mov  ah,9                       ;function number->Show string
                int  21h                        ;call INT 21h


                mov  ax,4c00h                   ;function number->Ends program
                int  21h                        ;call INT 21h



END