#ipc3.asm---------------------------------------------
.PB main
.PV message    -32
.PV ipaddr     -64

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

#set the message contents ( all 7 bytes of it, woo hoo! ) 
LAI $R10,$FP,message
MOV $R15,$R10
LBI $R2,1

LBI $R1,'h'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'e'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'l'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'l'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'o'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,'!'
SB  $R1,$R10
ADD $R10,$R10,$R2

LBI $R1,0
SB  $R1,$R10

#create the client socket
MOV $R2,$R14
LQI $R3,30000
LQI $R1,2
INT 10
MOV $R16,$R4

#send some data
MOV $R2,$R16
MOV $R3,$R15
LQI $R4,7
LQI $R1,4
INT 10

#shut the client socket down
MOV $R2,$R16
LQI $R1,3
INT 10

#reclaim local storage
MOV $SP,$FP
POPQ $FP

HALT
.PE