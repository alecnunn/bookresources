.PB main
.PV buffer  -256

#allocate local storage
PUSHQ $FP
MOV $FP,$SP
LQI $R7,256
SUB $SP,$SP,$R7

#get the number of arguments
LQI $R1,0
INT 5
MOV $R20,$R2

LQI $R1,20
INT 0

LQI $R2,10
LQI $R1,16
INT 0
LQI $R2,13
LQI $R1,16
INT 0

#get the first two arguments
LAI $R3,$FP,buffer
LQI $R2,0
LQI $R1,1
INT 5

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

LAI $R3,$FP,buffer
LQI $R2,1
LQI $R1,1
INT 5

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