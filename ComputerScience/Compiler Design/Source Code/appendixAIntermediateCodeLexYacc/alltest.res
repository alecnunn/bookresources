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
L1	(null)	 	 	 
	/	sum	nums	t4
	=	t4	 	average
	<	average	10.0	t5
	ifgo	t5	 	CL5
	=	10	 	a
	goto	 	 	CL6
CL5	=	20	 	b
CL6	=	30	 	c
	=	0	 	sum
	=	100000	 	requiredIncome
	=	2000	 	monthlyIncome
	=	0	 	noOfMonths
CL7	<=	sum	requiredIncome	t6
	ifgo	t6	 	CL8
	+	sum	monthlyIncome	t8
	=	t8	 	sum
	+	noOfMonths	1	t9
	=	t9	 	noOfMonths
	goto	 	 	CL7
CL8	(null)	 	 	 
	==	requiredIncome	sum	t10
	=	t10	 	check
	=	156000	 	loanAmount
	=	0	 	balance
	=	1000	 	installments
	=	0	 	noOfInstallments
CL9	-	loanAmount	installments	t11
	=	t11	 	balance
	+	noOfInstallments	1	t12
	=	t12	 	noOfInstallments
	<	balance	0	t13
	ifgo	t13	 	CL10
	goto	 	 	CL9
CL10	(null)	 	 	 
	-	balance		t16
	=	t16	 	excess
	goto	 	 	L1
	=	1	 	choice
	=	10	 	bonus
CL12	(null)	 	 	 
	+	marks	bonus	t17
	=	t17	 	marks
CL13	(null)	 	 	 
	-	marks	bonus	t18
	=	t18	 	marks
CL14	(null)	 	 	 
	+	marks	1	t19
	=	t19	 	marks
	goto	 	 	CL11
checkForCase	(null)	 	 	 
	ifgo	choice	1	CL12
	ifgo	choice	2	CL13
	goto	 	 	CL14
CL11	(null)	 	 	 
	>	marks	100	t20
	ifgo	t20	 	CL15
	=	100	 	marks

Name	Type	subtype	Width
t13	basic	int	4

t16	basic	int	4

t8	basic	int	4

t9	basic	int	4

cstackTop=-1 and bstackTop =-1
