Reading from file tst1.c . . .
1.......
Name	Type	Sub Type	Width

z	21		1		4
y	21		1		4
x	21		1		4
Name	Type	Sub Type	Width

Name	Type	Sub Type	Width

b	21		1		4
c	21		1		4
a	21		1		4
Program parsed successfully.
\nElements in vector:
 	=	20	 	b


 	=	30	 	x


 	=	25	 	y


L1	<	a	b	t1


 	IFGO	t1	0	L2


 	++	a	postfix	t2


 	=	t2	 	c


 	>	x	y	t3


 	IFGO	t3	0	L3


 	=	30	 	x


 	GOTO	 	 	L4


L3	=	y	 	x


L4	GOTO	 	 	L1


L2	+	x	y	t4


 	=	t4	 	z

