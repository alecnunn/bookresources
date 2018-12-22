 THE TABLE OF QUADRUPLES ARE

LABEL	OPER	OP1	OP2	RES

	=	0	-	idx
L1:	<	idx	nums	t1
	goto	-	-	L3
L2:	+	idx	1	t2
	=	t2	-	idx
	goto	-	-	L1
L3:	If=	t1	0	L4
	+	sum	idx	t3
	=	t3	-	sum
	goto	-	-	L2
L4:	Nop	-	-	-
	/	sum	nums	t4
	=	t4	-	average
..................................
Printing the quadruples Blockwise
..................................
Block No:0
	=	0	-	idx
Block No:1
L1:	<	idx	nums	t1
	goto	-	-	L3
Block No:2
L2:	+	idx	1	t2
	=	t2	-	idx
	goto	-	-	L1
Block No:3
L3:	If=	t1	0	L4
Block No:4
	+	sum	idx	t3
	=	t3	-	sum
	goto	-	-	L2
Block No:5
	/	sum	nums	t4
	=	t4	-	average
Construct Flow Graph
Print Flow Graph
B1
B2
B4	
B2	
B6	B5
B3	

Printing Labels
L1	L2	L3	L4	

