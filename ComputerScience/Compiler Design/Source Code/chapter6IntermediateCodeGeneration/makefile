icexe: ic.tab.o lex.yy.o hset.o icmain.o
	cc icmain.o hset.c ic.tab.o lex.yy.o -o icexe
ic.tab.o: ic.tab.c ic.h	
	cc -c ic.tab.c
ic.tab.c: ic.y ic.h define.h op.h
	bison -d  ic.y
lex.yy.o: lex.yy.c 
	cc -c lex.yy.c
lex.yy.c: ic.l ic.h define.h op.h
	lex ic.l
icmainnew.o: icmain.c ic.h define.h op.h
	cc -c icmain.c
hset.o:	hset.c ic.h define.h op.h
	cc -c hset.c

