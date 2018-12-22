cexe: y.tab.o lex.yy.o st.o cicprocess.o
	cc cicprocess.o st.o y.tab.o lex.yy.o -o cexe
lex.yy.o: lex.yy.c y.tab.h
	cc -c lex.yy.c
y.tab.h: cic.y
	yacc -d cic.y
y.tab.o: y.tab.c cic.h
	cc -c y.tab.c
y.tab.c: cic.y cic.h def.h op.h
	yacc -d  cic.y
lex.yy.c: cic.l cic.h def.h op.h
	flex cic.l
cicprocess.o: cicprocess.c cic.h def.h op.h
	cc -c cicprocess.c
st.o:   st.c cic.h def.h op.h
	cc -c st.c
