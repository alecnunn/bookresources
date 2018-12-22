In while->L1: ....
In while->L2: ....
 THE TABLE OF QUADRUPLES ARE

LABEL	OPER	OP1	OP2	RES

	=	0	-	sum
	=	100000	-	requiredIncome
	=	2000	-	monthlyIncome
	=	0	-	noOfMonths
L1:	<=	sum	requiredIncome	t1
	If=	t1	0	L2
	+	sum	monthlyIncome	t3
	=	t3	-	sum
	+	noOfMonths	1	t4
	=	t4	-	noOfMonths
	goto	-	-	L1
L2:	Nop	-	-	-
	==	requiredIncome	sum	t5
	=	t5	-	check
..................................
Printing the quadruples Blockwise
..................................
Block No:0
	=	0	-	sum
	=	100000	-	requiredIncome
	=	2000	-	monthlyIncome
	=	0	-	noOfMonths
Block No:1
L1:	<=	sum	requiredIncome	t1
	If=	t1	0	L2
Block No:2
	+	sum	monthlyIncome	t3
	=	t3	-	sum
	+	noOfMonths	1	t4
	=	t4	-	noOfMonths
	goto	-	-	L1
Block No:3
	==	requiredIncome	sum	t5
	=	t5	-	check
Construct Flow Graph
Print Flow Graph
B1
B2
B4	B3
B2	

Printing Labels
L1	L2	

