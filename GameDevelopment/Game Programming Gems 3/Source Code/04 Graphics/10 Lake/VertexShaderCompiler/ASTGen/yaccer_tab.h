typedef union {
	float fval;
	int ival;
	int token;
	char *sval;
	//can't make it an CastNode because its a C++ class
	void *node;
	} YYSTYPE;
#define	T_STRING	258
#define	T_FLOAT	259
#define	T_PLUS	260
#define	T_MINUS	261
#define	T_MULTIPLY	262
#define	T_DIVIDE	263
#define	T_DOT4	264
#define	T_NORMALIZE	265
#define	T_SQRT	266
#define	T_NEGATE	267
#define	T_POWER	268
#define	T_MAXWITH0	269
#define	T_CLAMPTO1	270
#define	T_SIN	271
#define	T_COS	272
#define	T_FLOOR	273
#define	T_CEILING	274
#define	T_EQUALS	275
#define	T_LBRACE	276
#define	T_RBRACE	277
#define	T_LPAREN	278
#define	T_RPAREN	279
#define	T_SEMICOLON	280
#define	T_COMMA	281
#define	T_TYPE_FLOAT	282
#define	T_TYPE_CLAMP	283
#define	T_DOT3	284


extern YYSTYPE yylval;
