#ipc4.asm---------------------------------------------
.PB main
.PV buffer    -32
.PV ipaddr    -64

#allocate local storage on stack
PUSHQ $FP
MOV $FP,$SP
LQI $R7,64
SUB $SP,$SP,$R7

#set the ipaddress 
LAI $R10,$FP,ipaddr
MOV $R14,$R10
LBI $R2,1

LBI $R1,'1'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'2'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'7'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'.'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'0'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'.'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'0'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'.'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'1'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,0
SB  $R1,$R10

#open a server socket
MOV $R2,$R14
LQI $R3,30000
LQI $R1,9
INT 10
MOV $R15,$R4

#accept a connection
MOV $R2,$R15
LAI $R3,$FP,buffer
LQI $R1,10
INT 10
MOV $R16,$R4

#print client address
LAI $R3,$FP,buffer
MOV $R2,$R3
LQI $R1,18
INT 0

LQI $R2,10
LQI $R1,16
INT 0
LQI $R2,13
LQI $R1,16
INT 0

#receive data from client ($R1=5)
MOV $R2,$R16
LAI $R3,$FP,buffer
LQI $R4,7
LQI $R1,5
INT 10

#print data
LAI $R3,$FP,buffer
MOV $R2,$R3
LQI $R1,18
INT 0

LQI $R2,10
LQI $R1,16
INT 0
LQI $R2,13
LQI $R1,16
INT 0

#close connection socket
MOV $R2,$R16
LQI $R1,11
INT 10

#close server socket
MOV $R2,$R15
LQI $R1,12
INT 10

#reclaim local storage
MOV $SP,$FP
POPQ $FP

HALT
.PE