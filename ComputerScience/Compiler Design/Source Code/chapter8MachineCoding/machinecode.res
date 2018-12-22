
MOV CX,AX Move AX register to CX register
	8b		41	

MOV DX, #4050 - move an immediate data to DX reg
	c7		43		40		50	

Mov SI, #4050 - Move a immediate number to SI index reg
	be		40		50	

Mov AX, 2025 - Move the contents of the memory 2025 into accumulator
	b1		25		20	

Move the content of acc to a mem addressed by 2025
	b3		25		20	

Mov SS, BX - Move the contents of BX register to DS
	8e		d3	

Mov BX,SS - Move the contents of SS to BX
	8c		d3	

Push SI - Push SI contents onto stack
	ff		f6	

Push DI - Push DI contents onto Stack
	47	

Push DS - Push Segment Register onto stack
	1e	

Pop SI- Pop the top of the stack into SI reg
	8f		c6	

Pop DX - Pop the top into DX reg
	5a	

Pop ES - pop the top contents into ES reg
	1f	

XCNG DI -Exchange Register DI with Accumulator
	97	

EXNG SI, DI -Exchange Register DI and SI
	87		f6	
