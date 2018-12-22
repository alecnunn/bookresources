.386
.MODEL FLAT,STDCALL
locals
jumps
UNICODE=0
include w32.inc


Extrn SetUnhandledExceptionFilter : PROC
Extrn GetProcAddress              : PROC
Extrn LoadLibraryA                : PROC

.data

message1       db "Detection using nmtrans.dll",0
message3       db "SoftICE found",0
message2       db "SoftICE not found",0
delayESP       dd 0                             ;the ESP register saves here
previous       dd 0                             ;ESP register saves the address of the previous SEH service here
SOFT95sym      db "C:\program\sice\SoftICE\nmtrans.dll",0     ;adjust this path according to SoftICE installation
SOFTNTsym      db "C:\program\NTICE\nmtrans.dll",0            ;this may be performed by reading from registers
SOFTsym        db "NmSymIsSoftICELoaded",0      ;the name of the function in nmtrans.dll



.code

Start:

;----------------------------------------------------------------------------
;Sets SEH in case of an error
;----------------------------------------------------------------------------
                mov  [delayESP],esp
                push offset error
                call SetUnhandledExceptionFilter
                mov  [previous], eax
;----------------------------------------------------------------------------

                call LoadLibraryA, offset SOFT95sym            ;loads DLL (nmtrans.dll) for SoftICE Windows 9x

                test eax, eax
                jz   no95                                      ;jump on error

                call GetProcAddress, eax, offset SOFTsym       ;finds address of the NmSymIsSoftICELoaded function

                test eax, eax
                jz   no95                                      ;jump on error


                call eax                                       ;calls the function NmSymIsSoftICELoaded whose
                                                               ;address is in EAX

                test eax,eax                                   ;jump if eax is not 0,
                jnz  exit                                      ;because SoftICE is active in memory



no95:

                call LoadLibraryA, offset SOFTNTsym           ;loads DLL (nmtrans.dll) for SoftICE Windows NT and
                                                              ;2000

                test eax,eax
                jz   exit                                     ;jump on error


                call GetProcAddress, eax, offset SOFTsym      ;detects address of the NmSymIsSoftICELoaded function

                test eax,eax
                jz   exit                                     ;jump on error

                call eax                                      ;calls NmSymIsSoftICELoaded function whose address is
                                                              ;in EAX

exit:


                push eax                                      ;saves the return value

;----------------------------------------------------------------------------
;Sets previous SEH service
;----------------------------------------------------------------------------
                push dword ptr [previous]
                call SetUnhandledExceptionFilter
;----------------------------------------------------------------------------

                pop  eax                                      ;restores the return value

                test eax,eax                                  ;if EAX isn't 0, SoftICE is active in memory
                jnz  jump                                     ;and the program ends



continue:
                call MessageBoxA,0, offset message2, offset message1,0
                call ExitProcess, -1

jump:

                call MessageBoxA,0, offset message3, offset message1,0
                call ExitProcess, -1




error:                                          ;starts a new SEH service in case of an error

                mov  esp, [delayESP]
                push offset continue
                ret



ends
end Start
