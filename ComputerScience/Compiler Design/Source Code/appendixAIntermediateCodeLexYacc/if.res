 THE TABLE OF QUADRUPLES ARE

LABEL	OPER	OP1	OP2	RES

	=	10.0		average
	<	average	10.0	t1
	ifgo	t1		CL1
	=	10		a
	goto	 	 	CL2
CL1	=	20	 	b
CL2	=	30	 	c
L1	(null)	 	 	 
	=	40	 	c
	goto	 	 	L1

Name	Type	subtype	Width
average	basic	double	8

t1	basic	double	8

cstackTop=-1 and bstackTop =-1
