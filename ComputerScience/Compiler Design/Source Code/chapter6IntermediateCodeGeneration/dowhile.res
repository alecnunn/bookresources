 THE TABLE OF QUADRUPLES ARE

LABEL	OPER	OP1	OP2	RES

	=	156000	-	loanAmount
	=	0	-	balance
	=	1000	-	installments
	=	0	-	noOfInstallments
L1:	-	loanAmount	installments	t1
	=	t1	-	balance
	+	noOfInstallments	1	t2
	=	t2	-	noOfInstallments
	<	balance	0	t3
	If=	t3	0	L2
	goto	-	-	L1
L2:	Nop	-	-	-
	-	balance	-	t6
	=	-	excess
..................................
Printing the quadruples Blockwise
..................................
Block No:0
	=	156000	-	loanAmount
	=	0	-	balance
	=	1000	-	installments
	=	0	-	noOfInstallments
Block No:1
L1:	-	loanAmount	installments	t1
	=	t1	-	balance
	+	noOfInstallments	1	t2
	=	t2	-	noOfInstallments
	<	balance	0	t3
	If=	t3	0	L2
Block No:2
	goto	-	-	L1
Block No:3
	-	balance	-	t6
	=	-	excess
Construct Flow Graph
Print Flow Graph
B1
B2
B4	B3
B2	

Printing Labels
L1	L2	

