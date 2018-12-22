 THE TABLE OF QUADRUPLES ARE

LABEL	OPER	OP1	OP2	RES

	=	0		sum
	=	0		idx
CL1	<	idx	nums	t1
	goto			CL3
CL2	+	idx	1	t2
	=	t2		idx
	goto			CL1
CL3	ifgo	t1		CL4
	+	sum	qty	t3
	=	t3		sum
	goto			CL2
CL4	(null)			 
	/	sum	nums	t4
	=	t4		average
	<	average	10.0	t5
	ifgo	t5		CL5
	=	10		a
	goto	 	 	CL6
CL5	=	20	 	b
CL6	=	30	 	c

Name	Type	subtype	Width
t3	basic	int	4

cstackTop=-1 and bstackTop =-1
