.PB main
.PV filename1  -16
.PV filename2  -32
.PV buffer     -512

#allocate local storage
PUSHQ $FP
MOV $FP,$SP
LQI $R7,512
SUB $SP,$SP,$R7

#populate filename1 = notepad
LAI $R10,$FP,filename1
MOV $R11,$R10
LBI $R2,1

LBI $R1,'n'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'o'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'t'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'e'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'p'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'a'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'d'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'.'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'e'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'x'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'e'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,0
SB  $R1,$R10

#populate local filename2 = mem /f
LAI $R10,$FP,filename2
MOV $R12,$R10
LBI $R2,1

LBI $R1,'m'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'e'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'m'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,' '
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'/'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'f'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,0
SB  $R1,$R10

#spawn notepad
LBI $R1,0
LBI $R3,0
MOV $R2,$R11
INT 2

#wait for notepad to die, then print return code
MOV $R2,$R4
LBI $R1,1
INT 2

MOV $R2,$R3
LBI $R1,20
INT 0

LQI $R2,10
LQI $R1,16
INT 0
LQI $R2,13
LQI $R1,16
INT 0

#spawn notepad again using different function
LBI $R1,2
MOV $R2,$R11
INT 2

MOV $R2,$R3
LBI $R1,20
INT 0

LQI $R2,10
LQI $R1,16
INT 0
LQI $R2,13
LQI $R1,16
INT 0

#call mem /f and print output
LBI $R1,3
MOV $R2,$R12
LAI $R3,$FP,buffer
LQI $R4,512
INT 2

LAI $R3,$FP,buffer
MOV $R2,$R3
LBI $R1,18
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
