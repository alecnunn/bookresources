.GB gvar1  
.GW gvar2
.GD gvar3  10
.GQ gvar4  15   
#------------------------------------------------------------
.PB myproc1
	.PR  retval 6
	.PA  arg1  4	
	.PA arg2  3
	.PA arg3  1
	.PV var1  -6
	.PV var2 -11
	.PV var3 -18
LBI $R1,-23
LWI $R2,280
LDI $R3,-70000
LQI $R4,1234567890123
LF1I $F1, -2.3e-4
LF2I $D1,100e2
LAD $R5,myproc2
LAI $R6,$TOP,gvar4
LWI $R7,600
SB $R1,$R7
LB $R8,$R7
SW $R2,$R7
LW $R9,$R7
SD $R3,$R7
LD $R10,$R7
SQ $R4,$R7
LQ $R11,$R7
SF1 $F1,$R7
LF1 $F2,$R7
SF2 $D1,$R7
LF2 $D2,$R7
PUSHB $R4
PUSHW $R4
PUSHD $R4
PUSHQ $R4
PUSHF1 $F1
PUSHF2 $D1
.PL proc1label1
POPB $R4
POPW $R4
POPD $R4
POPQ $R4
POPF1 $F3
POPF2 $D3
.PE

#-------------------------------------------------------------
.PB myproc2
	.PR  p2ret 10 
	.PA  p2arg1  5	
	.PA  p2arg2  2
	.PA  p2arg3  1
	.PV p2var1  -5
	.PV  p2var2 -12
	.PV  p2var3 -19
MOV $R4,$R12
MOVF $F3,$F4
MOVD $D3,$D4
LAD $R13,proc2label1
JMP $R13
NOP
NOP
NOP
.PL proc2label1
LAD $R13,proc2label2
JE $R4,$R12,$R13
NOP
.PL proc2label2
JNE $R4,$R12,$R13
SLT $R1,$R4,$R13
INT 12
DI 
EI
LBI $R2,10
LBI $R3,112
AND $R1,$R2,$R3
OR  $R1,$R2,$R3
XOR $R1,$R2,$R3
NOT $R1,$R2
LBI $R2,4
BT  $R1,$R3,$R2
BS  $R1,$R2
SRA $R1,$R3,$R2
SRL $R1,$R3,$R2
SL  $R1,$R3,$R2
LBI $R2,10
LBI $R3,56
ADD $R1,$R2,$R3
SUB $R1,$R2,$R3
MULT $R1,$R2,$R3
DIV $R1,$R4,$R2,$R3
CAST_IF $R1,$F3
CAST_ID $R1,$D3
CAST_FI $F1,$R3
CAST_FD $F1,$D3
CAST_DI $D3,$R1
CAST_DF $D3,$F3
LF1I $F2,340.5e-12
LF1I $F3,12.456
FADD $F1,$F2,$F3
FSUB $F1,$F2,$F3
FMULT $F1,$F2,$F3
FDIV $F1,$F2,$F3
FSLT $F1,$F2,$F3
LF2I $D2,567.89
LF2I $D3,75.23e5
DADD $D1,$D2,$D3
DSUB $D1,$D2,$D3
DMULT $D1,$D2,$D3
DDIV $D1,$D2,$D3
DSLT $D1,$D2,$D3
HALT 
.PE