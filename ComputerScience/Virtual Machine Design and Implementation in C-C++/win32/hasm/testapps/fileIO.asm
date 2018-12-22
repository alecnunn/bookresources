#fileIO.asm---------------------------------------------
.PB main
.PV buffer -32
.PV fname  -64

#allocate local storage on stack
PUSHQ $FP
MOV $FP,$SP
LQI $R7,64
SUB $SP,$SP,$R7

#read and print floating-point value
LQI $R1,25
INT 0
LQI $R1,21
INT 0

LQI $R2,10
LQI $R1,16
INT 0
LQI $R2,13
LQI $R1,16
INT 0

#read and print an integer
LQI $R3,0
LQI $R1,24
INT 0
LQI $R3,0
LQI $R1,20
INT 0

LQI $R2,10
LQI $R1,16
INT 0
LQI $R2,13
LQI $R1,16
INT 0

#read and print a string
LAI $R2,$FP,buffer
MOV $R7,$R2
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

#set file name
LAI $R2,$FP,fname
MOV $R9,$R2
LQI $R1,22
INT 0

#open a file, write a few bytes, then close
LQI $R3,1
LQI $R1,0
INT 0
MOV $R8,$R4

MOV $R2,$R8
LQI $R3,8
MOV $R4,$R7
LQI $R1,12
INT 0

MOV $R2,$R8
LQI $R1,1
INT 0

#open file, move around, read a byte, then close
MOV $R2,$R9
LQI $R3,0
LQI $R1,0
INT 0
MOV $R8,$R4

MOV $R2,$R8
LQI $R3,3
LQI $R4,0
LQI $R1,8
INT 0

MOV $R2,$R8
LQI $R1,7
INT 0
MOV $R2,$R3
LQI $R1,20
INT 0

LQI $R2,10
LQI $R1,16
INT 0
LQI $R2,13
LQI $R1,16
INT 0

MOV $R2,$R8
LQI $R3,3
MOV $R4,$R9
LQI $R1,11
INT 0

MOV $R2,$R9
LQI $R1,18
INT 0

MOV $R2,$R8
LQI $R1,1
INT 0

#reclaim local storage
MOV $SP,$FP
POPQ $FP

HALT
.PE