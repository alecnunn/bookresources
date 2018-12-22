.386
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc

Extrn SetUnhandledExceptionFilter : PROC

.DATA
message1        db "Breakpoint detection by CRC",0
message3        db "Breakpoint or a change in the program found",0
message2        db "Breakpoint not found",0
delayESP        dd 0                    ;the ESP register saves here
previous        dd 0                    ;the ESP register will save the address of the previous SEH service here
CRCTable        dd    00000000h,  77073096h, 0EE0E612Ch,  990951BAh
                dd    076DC419h,  706AF48Fh, 0E963A535h,  9E6495A3h
                dd    0EDB8832h,  79DCB8A4h, 0E0D5E91Eh,  97D2D988h
                dd    09B64C2Bh,  7EB17CBDh, 0E7B82D07h,  90BF1D91h
                dd    1DB71064h,  6AB020F2h, 0F3B97148h,  84BE41DEh
                dd    1ADAD47Dh,  6DDDE4EBh, 0F4D4B551h,  83D385C7h
                dd    136C9856h,  646BA8C0h, 0FD62F97Ah,  8A65C9ECh
                dd    14015C4Fh,  63066CD9h, 0FA0F3D63h,  8D080DF5h
                dd    3B6E20C8h,  4C69105Eh, 0D56041E4h, 0A2677172h
                dd    3C03E4D1h,  4B04D447h, 0D20D85FDh, 0A50AB56Bh
                dd    35B5A8FAh,  42B2986Ch, 0DBBBC9D6h, 0ACBCF940h
                dd    32D86CE3h,  45DF5C75h, 0DCD60DCFh, 0ABD13D59h
                dd    26D930ACh,  51DE003Ah, 0C8D75180h, 0BFD06116h
                dd    21B4F4B5h,  56B3C423h, 0CFBA9599h, 0B8BDA50Fh
                dd    2802B89Eh,  5F058808h, 0C60CD9B2h, 0B10BE924h
                dd    2F6F7C87h,  58684C11h, 0C1611DABh, 0B6662D3Dh

                dd    76DC4190h,  01DB7106h,  98D220BCh, 0EFD5102Ah
                dd    71B18589h,  06B6B51Fh,  9FBFE4A5h, 0E8B8D433h
                dd    7807C9A2h,  0F00F934h,  9609A88Eh, 0E10E9818h
                dd    7F6A0DBBh,  086D3D2Dh,  91646C97h, 0E6635C01h
                dd    6B6B51F4h,  1C6C6162h,  856530D8h, 0F262004Eh
                dd    6C0695EDh,  1B01A57Bh,  8208F4C1h, 0F50FC457h
                dd    65B0D9C6h,  12B7E950h,  8BBEB8EAh, 0FCB9887Ch
                dd    62DD1DDFh,  15DA2D49h,  8CD37CF3h, 0FBD44C65h
                dd    4DB26158h,  3AB551CEh, 0A3BC0074h, 0D4BB30E2h
                dd    4ADFA541h,  3DD895D7h, 0A4D1C46Dh, 0D3D6F4FBh
                dd    4369E96Ah,  346ED9FCh, 0AD678846h, 0DA60B8D0h
                dd    44042D73h,  33031DE5h, 0AA0A4C5Fh, 0DD0D7CC9h
                dd    5005713Ch,  270241AAh, 0BE0B1010h, 0C90C2086h
                dd    5768B525h,  206F85B3h, 0B966D409h, 0CE61E49Fh
                dd    5EDEF90Eh,  29D9C998h, 0B0D09822h, 0C7D7A8B4h
                dd    59B33D17h,  2EB40D81h, 0B7BD5C3Bh, 0C0BA6CADh

                dd   0EDB88320h,  9ABFB3B6h,  03B6E20Ch,  74B1D29Ah
                dd   0EAD54739h,  9DD277AFh,  04DB2615h,  73DC1683h
                dd   0E3630B12h,  94643B84h,  0D6D6A3Eh,  7A6A5AA8h
                dd   0E40ECF0Bh,  9309FF9Dh,  0A00AE27h,  7D079EB1h
                dd   0F00F9344h,  8708A3D2h,  1E01F268h,  6906C2FEh
                dd   0F762575Dh,  806567CBh,  196C3671h,  6E6B06E7h
                dd   0FED41B76h,  89D32BE0h,  10DA7A5Ah,  67DD4ACCh
                dd   0F9B9DF6Fh,  8EBEEFF9h,  17B7BE43h,  60B08ED5h
                dd   0D6D6A3E8h, 0A1D1937Eh,  38D8C2C4h,  4FDFF252h
                dd   0D1BB67F1h, 0A6BC5767h,  3FB506DDh,  48B2364Bh
                dd   0D80D2BDAh, 0AF0A1B4Ch,  36034AF6h,  41047A60h
                dd   0DF60EFC3h, 0A867DF55h,  316E8EEFh,  4669BE79h
                dd   0CB61B38Ch, 0BC66831Ah,  256FD2A0h,  5268E236h
                dd   0CC0C7795h, 0BB0B4703h,  220216B9h,  5505262Fh
                dd   0C5BA3BBEh, 0B2BD0B28h,  2BB45A92h,  5CB36A04h
                dd   0C2D7FFA7h, 0B5D0CF31h,  2CD99E8Bh,  5BDEAE1Dh

                dd    9B64C2B0h, 0EC63F226h,  756AA39Ch,  026D930Ah
                dd    9C0906A9h, 0EB0E363Fh,  72076785h,  05005713h
                dd    95BF4A82h, 0E2B87A14h,  7BB12BAEh,  0CB61B38h
                dd    92D28E9Bh, 0E5D5BE0Dh,  7CDCEFB7h,  0BDBDF21h
                dd    86D3D2D4h, 0F1D4E242h,  68DDB3F8h,  1FDA836Eh
                dd    81BE16CDh, 0F6B9265Bh,  6FB077E1h,  18B74777h
                dd    88085AE6h, 0FF0F6A70h,  66063BCAh,  11010B5Ch
                dd    8F659EFFh, 0F862AE69h,  616BFFD3h,  166CCF45h
                dd   0A00AE278h, 0D70DD2EEh,  4E048354h,  3903B3C2h
                dd   0A7672661h, 0D06016F7h,  4969474Dh,  3E6E77DBh
                dd   0AED16A4Ah, 0D9D65ADCh,  40DF0B66h,  37D83BF0h
                dd   0A9BCAE53h, 0DEBB9EC5h,  47B2CF7Fh,  30B5FFE9h
                dd   0BDBDF21Ch, 0CABAC28Ah,  53B39330h,  24B4A3A6h
                dd   0BAD03605h, 0CDD70693h,  54DE5729h,  23D967BFh
                dd   0B3667A2Eh, 0C4614AB8h,  5D681B02h,  2A6F2B94h
                dd   0B40BBE37h, 0C30C8EA1h,  5A05DF1Bh,  2D02EF8Dh

CRC             dd   1325596Eh              ;CRC pre ukazkovy program

.code
Start:
;------------------------------------------------------------------------------------------------;Sets SEH in case of an error
;------------------------------------------------------------------------------------------------
        mov  [delayESP],esp
        push offset error
        call SetUnhandledExceptionFilter
        mov  [previous], eax
;------------------------------------------------------------------------------------------------
        lea  esi, CRCTable              ;address of the CRC table needed for the calculation
        lea  edi, Start                 ;beginning of data for which the CRC will be calculated (beginning of your program)
        mov  ecx, End-Start+1           ;length of the data in bytes (length of your program)
        call CRC32                      ;jump to calculate the CRC

        push eax        ;saves the calculated CRC
;------------------------------------------------------------------------------------------------
;Sets the previous SEH service
;------------------------------------------------------------------------------------------------
        push dword ptr [previous]
        call SetUnhandledExceptionFilter
;------------------------------------------------------------------------------------------------
        pop  eax                        ;restores the calculated CRC
        cmp  dword ptr CRC, eax        ;compares the calculated CRC with the saved one
        jnz jump                        ;if the CRCs do not match, the program was either changed or a breakpoint was set
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
;------------------------------------------------------------------------------------------------
;The routine for the CRC calculation
;------------------------------------------------------------------------------------------------
;input:
;
;ESI = CRCTable address
;EDI = beginning of the data address
;ECX = number of bytes for the calculation
;
;output
;
;EAX = 32 bit CRC result
;------------------------------------------------------------------------------------------------

CRC32:
        mov  eax, 0FFFFFFFFh            ;initializes the eax and the edx
        mov  edx,eax                    ;saves the ecx into 0FFFFFFFFh
CRC32_1:
        push ecx                        ;the number of bytes for the calculation
        xor  ebx,ebx
        mov  bl,byte ptr [edi]          ;reads 1 byte from the data
        inc  edi                        ;and moves to the next
        xor  bl,al
        shl bx,1
        shl bx,1
        add ebx,esi                     ;sets according to the read byte in the CRC table
        mov  cx,word ptr [ebx+2]        ;reads a 32-bit
        mov  bx,word ptr [ebx]          ;the value from the CRC table
        mov  al,ah                      ;moves DX:AX to the right by 8 bytes
        mov  ah,dl
        mov  dl,dh
        xor  dh,dh
        xor  ax,bx                      ;XOR  CX:BX into DX:AX
        xor  dx,cx
        pop  ecx                        ;restores the number of the non-calculated data in bytes
        loop CRC32_1                    ;jumps if not all has been calculated yet, and decreases the number of bytes in the ecx by 1
        not  ax                         ;the final calculation of the CRC
        not  dx
        push dx                         ;saves the 16-bit values
        push ax                         ;into the stack
        pop  eax                        ;and after it has been read back, the complete 32-bit CRC will be in the eax register
        ret     ;return
End:

ends
end Start
