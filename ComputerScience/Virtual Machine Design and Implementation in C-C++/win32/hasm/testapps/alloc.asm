.PB main

#init the heap
LQI $R1,3
INT 7

#allocate memory, place address in $R20
LQI $R2,20
LQI $R1,0
INT 7
MOV $R20,$R3

MOV $R2,$R20
LQI $R1,20
INT 0

LQI $R2,10
LQI $R1,16
INT 0
LQI $R2,13
LQI $R1,16
INT 0

#allocate memory, place address in $R21
LQI $R2,11
LQI $R1,0
INT 7
MOV $R21,$R3

MOV $R2,$R21
LQI $R1,20
INT 0

LQI $R2,10
LQI $R1,16
INT 0
LQI $R2,13
LQI $R1,16
INT 0

#allocate memory, place address in $R22
LQI $R2,1
LQI $R1,0
INT 7
MOV $R22,$R3

MOV $R2,$R22
LQI $R1,20
INT 0

LQI $R2,10
LQI $R1,16
INT 0
LQI $R2,13
LQI $R1,16
INT 0

#print out heap
LQI $R1,2
INT 7

#dealloc memory
MOV $R2,$R20
LQI $R1,1
INT 7

#print out heap
LQI $R1,2
INT 7

#dealloc memory
MOV $R2,$R21
LQI $R1,1
INT 7

#print out heap
LQI $R1,2
INT 7

#dealloc memory
MOV $R2,$R22
LQI $R1,1
INT 7

#print out heap
LQI $R1,2
INT 7

HALT
.PE