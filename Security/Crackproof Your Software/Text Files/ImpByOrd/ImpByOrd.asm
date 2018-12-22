; I used MASM 6.14

.586P

.MODEL       FLAT, STDCALL

 OPTION      CASEMAP: NONE

 UNICODE     = 0
 INCLUDE     WINDOWS.inc
 INCLUDE     APIMACRO.mac

 INCLUDELIB  iKERNEL32sub.lib
 INCLUDELIB  iUSER32sub.lib

.DATA
   TEXT      zTitle,   <VxDCall Test/0>
   TEXT      zPress,   <VWIN32 version is: 0x/#0.4X./0>
   TEXT      KERNEL32, <KERNEL32/0>

.DATA?
   Place     SIGN 64 DUP (?)

.CODE
 PrimaryThread:

   ;9x symptom is that GetProcAddress for Ord fails for K32
   iWin32i   GetModuleHandle, sKERNEL32
   iWin32    GetProcAddress, EAX, 1
   TEST      EAX, EAX
   MOV       EAX, -1
   JNE       Exit   ;not 9x  


   ; as always there are 2 possibilities (EliASM specific):
   ; the 1st is to use instructions-macros: iWin32, iMOV, iCMP, ...

   ;and the 2nd is to use iEXTERN and then normal instructions
   iEXTERN   KERNEL32_ORD_0001
   CMP       KERNEL32_ORD_0001, 80000000H 
   JB        Exit     
   PUSH      002A0000H
   CALL      KERNEL32_ORD_0001

   ;  i recommend to use the 1st method, because it doesn't "consume name":
   ; name KERNEL32_ORD_0001 can be used for other purposes - you can use it for
   ; example as string name: TEXT KERNEL32_ORD_0001, <Ord1/0>
   ;  while the 2nd method declares KERNEL32_ORD_0001 for the next use, so the
   ; name can't be used for other declarations/definitions

   MOV       EBX, OFFSET Place
   icWin32i  wsprintf, EBX, szPress, EAX
   iEXTERNi  MessageBox
   sWin32    MessageBox, NULL, EBX, szTitle, MB_OK
  Exit:
   iWin32    ExitProcess, EAX


END PrimaryThread