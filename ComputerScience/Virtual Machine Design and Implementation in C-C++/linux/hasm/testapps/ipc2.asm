#ipc2.asm---------------------------------------------
.PB main
.PV hostname  -32
.PV ipaddr    -64

#allocate local storage on stack
PUSHQ $FP
MOV $FP,$SP
LQI $R7,64
SUB $SP,$SP,$R7

#get the local host name and print it 
LAI $R2,$FP,hostname
LQI $R3,32
LBI $R1,6
INT 10

LQI $R1,18
INT 0

LQI $R2,10
LQI $R1,16
INT 0
LQI $R2,13
LQI $R1,16
INT 0

#get the ip address of local host
LAI $R2,$FP,hostname
LAI $R3,$FP,ipaddr
LQI $R1,7
INT 10

MOV $R2,$R3
LQI $R1,18
INT 0

LQI $R2,10
LQI $R1,16
INT 0
LQI $R2,13
LQI $R1,16
INT 0

#get the host name associated with this ip
LAI $R3,$FP,hostname
LAI $R2,$FP,ipaddr
LQI $R1,8
INT 10

MOV $R2,$R3
LQI $R1,18
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