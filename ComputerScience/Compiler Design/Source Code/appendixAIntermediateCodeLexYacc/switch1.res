 THE TABLE OF QUADRUPLES ARE

LABEL	OPER	OP1	OP2	RES

	=	10		marks
	=	1		choice
	=	10		bonus
CL2	(null)	 	 	 
	+	marks	bonus	t1
	=	t1	 	marks
CL3	(null)	 	 	 
	-	marks	bonus	t2
	=	t2	 	marks
CL4	(null)	 	 	 
	+	marks	1	t3
	=	t3	 	marks
	goto	 	 	CL1
checkForCase	(null)	 	 	 
	ifgo	choice	1	CL2
	ifgo	choice	2	CL3
	ifgo	choice	3	CL4
CL1	(null)	 	 	 

Name	Type	subtype	Width
cstackTop=-1 and bstackTop =-1
