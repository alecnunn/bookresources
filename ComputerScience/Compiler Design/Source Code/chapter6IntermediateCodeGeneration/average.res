RASSIGN = 37
 THE TABLE OF QUADRUPLES ARE

LABEL	OPER	OP1	OP2	RES

	=	10	-	n
	=	0	-	i
L1:	<	i	n	t1
	goto	-	-	L3
L2:	+	i	1	t2
	=	t2	-	i
	goto	-	-	L1
L3:	If=	t1	0	L4
	*	i	4	t3
	=	8	-	t4
	=[]	t4	t3	t5
	+	sum	t5	t6
	=	t6	-	sum
	goto	-	-	L2
L4:	Nop	-	-	-
	/	sum	n	t7
	=	t7	-	sum
..................................
Printing the quadruples Blockwise
..................................
Block No:0
	=	10	-	n
	=	0	-	i
Block No:1
L1:	<	i	n	t1
	goto	-	-	L3
Block No:2
L2:	+	i	1	t2
	=	t2	-	i
	goto	-	-	L1
Block No:3
L3:	If=	t1	0	L4
Block No:4
	*	i	4	t3
	=	8	-	t4
	=[]	t4	t3	t5
	+	sum	t5	t6
	=	t6	-	sum
	goto	-	-	L2
Block No:5
	/	sum	n	t7
	=	t7	-	sum
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

