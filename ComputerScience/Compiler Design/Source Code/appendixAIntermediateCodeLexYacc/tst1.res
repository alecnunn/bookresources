 THE TABLE OF QUADRUPLES ARE

LABEL	OPER	OP1	OP2	RES

	=	0		idx
CL1	<	idx	nums	t1
	goto			CL3
CL2	+	idx	1	t2
	=	t2		idx
	goto			CL1
CL3	ifgo	t1		CL4
	+	sum	idx	t3
	+	t3	x	t4
	=	t4		sum
	goto			CL2
CL4	(null)			 
	/	sum	nums	t5
	*	t5	y	t6
	=	t6		average

Name	Type	subtype	Width
x	basic	int	4

y	basic	int	4

z	basic	int	4

cstackTop=-1 and bstackTop =-1
