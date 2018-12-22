#ipc1.asm---------------------------------------------
.PB main
.PV buffer -40
.PV mutex  -64

#allocate local storage on stack
PUSHQ $FP
MOV $FP,$SP
LQI $R7,64
SUB $SP,$SP,$R7

#set name of mutex and store address in $R14
LAI $R10,$FP,mutex
MOV $R14,$R10
LBI $R2,1

LBI $R1,'m'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'i'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'n'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'e'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,0
SB  $R1,$R10

#obtain a mutex
MOV $R2,$R14
LQI $R1,0
INT 10
MOV $R15,$R3

#print string to signal that mutex obtained
MOV $R2,$R14
LQI $R1,18
INT 0

LQI $R2,10
LQI $R1,16
INT 0
LQI $R2,13
LQI $R1,16
INT 0

#read and print string
LAI $R2,$FP,buffer
LQI $R1,22
INT 0

LQI $R1,18
INT 0

LQI $R2,10
LQI $R1,16
INT 0
LQI $R2,13
LQI $R1,16
INT 0

#release mutex
MOV $R2,$R15
LQI $R1,1
INT 10


#reclaim local storage
MOV $SP,$FP
POPQ $FP

HALT
.PE