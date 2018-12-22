.PB main
.PV filename  -16
.PV input	  -128
.PV output    -512

#allocate local storage
PUSHQ $FP
MOV $FP,$SP
LQI $R7,512
SUB $SP,$SP,$R7

#populate name of shared lib = sharedLib.dll
LAI $R10,$FP,filename
MOV $R14,$R10
LBI $R2,1

LBI $R1,'s'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'h'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'a'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'r'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'e'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'d'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'L'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'i'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'b'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'.'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'d'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'l'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'l'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,0
SB  $R1,$R10

#populate XML input <IN>proc1</IN>
LAI $R10,$FP,input
MOV $R11,$R10
LBI $R2,1

LBI $R1,'<'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'I'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'N'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'>'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'p'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'r'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'o'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'c'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'1'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'<'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'/'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'I'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'N'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'>'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,0
SB  $R1,$R10

#load the library (put handle in $R20)
LQI $R1,0
MOV $R2,$R14
INT 9
MOV $R20,$R3

MOV $R2,$R4
LQI $R1,20
INT 0

LQI $R2,10
LQI $R1,16
INT 0
LQI $R2,13
LQI $R1,16
INT 0

#execute native call
LAI $R4,$FP,output
MOV $R3,$R11
MOV $R2,$R20
LQI $R1,1
INT 9

LAI $R2,$FP,output
LQI $R1,18
INT 0

LQI $R2,10
LQI $R1,16
INT 0
LQI $R2,13
LQI $R1,16
INT 0

#free library
MOV $R2,$R20
LQI $R1,2
INT 9

MOV $R2,$R3
LQI $R1,20
INT 0

LQI $R2,10
LQI $R1,16
INT 0
LQI $R2,13
LQI $R1,16
INT 0

#reclaim local storage
MOV $SP,$FP
POPQ $FP

HALT
.PE