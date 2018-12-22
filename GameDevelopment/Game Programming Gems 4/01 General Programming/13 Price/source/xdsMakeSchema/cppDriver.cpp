// ****************************************************************************
// *** 
// *** CPP parser integration
// *** 
// ****************************************************************************

// ----------------------------------------------------------------------------
// 
// This file is part of the XDS Toolkit, Copyright (C) 2003 Mark T. Price
// For conditions of distribution and use, see XdsLicense.h
//
// ----------------------------------------------------------------------------

#include "xdsMakeSchema.h"

extern "C" {
#include <tcl.h>

#include "crossrefP.h"

int f_ReadFile(FILE *fp);
void f_read_end( void );

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Global variables
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *filename_g;
int niveau;
Comp_t CompAct;
int niveauComp;
Symtab_t SymtabVariable;
Symtab_t SymtabClass;
int template_arg;
FILE *hig_fp = NULL;
FILE *test_fp;
FILE *pf = NULL;

int start_lineno_g;
char *scope_g;
char *sym_name_g;
char *arg_types_g;
FILE *yyfp = NULL;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Helper functions
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static int strnlen(const char *sz, int len)
{
	int iLen = 0;
	if(!sz)
		return 0;

	for(; *sz && len>0; ++sz, --len)
		++iLen;

	return iLen;
}
char *SN_StrnDup(const char *sz, int len)
{
	char *cp;

	cp = (char *)malloc(len+1);
	memcpy(cp, sz, len);
	cp[len] = '\0';
	return cp;
}


char *SN_StrDup (char *sz) { return strdup(sz); }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Stub functions
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

extern void Paf_panic( int i )
{
	exit( i );
}


extern int Put_symbol (int type, char *scope, char *sym_name, char *file, int start_lineno, int start_colpos, int end_lineno, int end_colpos, unsigned long attr, char *ret, char *arg_types, char *args, char *reserved, int start_lineno_highlight, int start_colpos_highlight, int end_lineno_highlight, int end_colpos_highlight )
{
   return 0;
}
extern void save_comment(int lineno, int charno, char *text, int length)
{
}
extern int get_symbol (char *class_name,char *scope_local,char *name,char *arg_types, char *scope,char *ret_type, char *ret_define, int exact)
{
	scope   [0] = 0;
	ret_type[0] = 0;
	ret_define[0] = 0;
	return 0;
}

extern int get_class_or_typedef( char *name, char *type )
{
	return 0;
}

extern int put_cross_ref ( int type,int scope_type,int scope_lev,
						  char *fnc_cls,char *fnc,char *fnc_arg_types,char *scope,char *what,
						  char *arg_types,char *file,int lineno,int acc)
{
	return 0;
}


}

// routines defined in defTypes.cpp
extern void decodeDeclaration(Declaration_t decl);


// ****************************************************************************
// *** 
// *** xdsExtract main
// *** 
// ****************************************************************************

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  support functions
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int log_symbol_filename(char *fname)
{
	if(yyfp != NULL && yyfp != stdin)
		fclose(yyfp);

	if(fname)
	{
		yyfp = fopen(fname, OPEN_MODE);
		if(yyfp == NULL)
		{
			printf("%s: unable to open file '%s' - %s\n", szProgName, fname, strerror(errno));
			fflush(stdout);
			return 1;
		}
	}
	else
		yyfp = stdin;


	return 0;
}

/*
void decodeDecl(Declaration_t decl)
{
	switch(decl->iType)
	{
	case DECLARATION_OBJECT:	// variable
	case DECLARATION_MEMBER: // class/struct/union member
	{
		switch(decl->storage_class)
		{
		case SN_TYPEDEF:  printf("typedef ");  break;
		case SN_FRIEND:   printf("friend ");  break;
		case SN_AUTO:     printf("auto ");  break;
		case SN_REGISTER: printf("register ");  break;
		case SN_STATIC:   printf("static ");  break;
		case SN_EXTERN:   printf("extern ");  break;
		default:
			break;
		}

		if(decl->Class)
		{
			Class_t cl = decl->Class;
			switch(cl->iType) {
			case SN_STRUCT: printf("struct "); break;
			case SN_CLASS:  printf("class ");  break;
			case SN_UNION:  printf("union ");  break;
			default:
				printf("<unknown> ");
			}
			if(cl->Name)
				printf("%s ", cl->Name->pcName);
			// else anonymous structure/union
			if(cl->ListMember)
			{
				printf("\n");
				Member_t m = (Member_t)cl->ListMember->ElemFirst;
				while(m)
				{
					if(m->Declaration)
					{
						printf("    ");
						decodeDecl(m->Declaration);
					}
					m = m->MemberNext;
				}
			}
		}

		if(decl->Enum)
		{
			Enum_t et = decl->Enum;
			printf("enum ");
			if(et->Name)
				printf("%s ", et->Name->pcName);
			// else anonymous enum
			if(et->ListEnumerator)
			{
				printf("\n");
				Enumerator_t en = (Enumerator_t)et->ListEnumerator->ElemFirst;
				while(en)
				{
					if(en->Expr && en->Expr->Type->s_int)
						printf("\t%s = %d\n", en->Name->pcName, en->Expr->intValue);
					else
						printf("\t%s\n", en->Name->pcName);
					en = en->EnumeratorNext;
				}
			}
		}

		if(decl->s_const)
			printf("const ");

		if(decl->s_signed)
			printf("signed ");
		if(decl->s_unsigned)
			printf("unsigned ");
		if(decl->s_volatile)
			printf("volatile ");
		if(decl->s_void)
			printf("void ");

		if(decl->s_bool)
			printf("bool ");
		if(decl->s_char)
			printf("char ");
		if(decl->s_double)
			printf("double ");
		if(decl->s_float)
			printf("float ");
		if(decl->s_int)
			printf("int ");
		if(decl->s_long)
			printf("long ");
		if(decl->s_short)
			printf("short ");

		if(decl->ListDeclarator)
		{
			Declarator_t d = (Declarator_t)decl->ListDeclarator->ElemFirst;	// each declarator is a separate data item
			while(d)
			{
				if(d->Expr && d->Expr->Type && d->Expr->Type->s_int)
				{
					printf(": %d ", d->Expr->intValue);
				}
				if(d->ListOper)
				{
					Oper_t o = (Oper_t)d->ListOper->ElemFirst;
					while(o)
					{
						if(o->Name)
							printf("%s ", o->Name->pcName);
						switch(o->type)
						{
						case FUNCTION: printf("() "); break;
						case ARRAY:
							if(o->Expr)
							{
								if(o->Expr->Type && o->Expr->Type->s_int)
									printf("[%d] ", o->Expr->intValue);
								else
									printf("[%s] ", o->Expr->Name ? o->Expr->Name->pcName : "");
							}
							else
								printf("[] ");
							break;
						case POINTER_STAR: printf("* "); break;
						case POINTER_AMPERSAND: printf("& "); break;
						}
						o = o->OperNext;
					}
				}
				if(d->Name)
					printf("%s ", d->Name->pcName);
				printf("\n");
				d = d->DeclaratorNext;
			}
		}
	}	
	case DECLARATION_UNKNOWN:
	case DECLARATION_ASM:
	case DECLARATION_TEMPLATE:
	case DECLARATION_LINKAGE_SPECIFICATION:
	case DECLARATION_NAMESPACE_DEFINITION:
	case DECLARATION_NAMESPACE_ALIAS_DEFINITION:
	case DECLARATION_USING_DIRECTIVE:
	case DECLARATION_USING:
	case DECLARATION_ARGUMENT: // function call argument
	default:
		break;
	}
}
*/

void start_parser( char *filename_a, int is_cpp)
{
	if(yyfp == NULL)
	{
		return;
	}

	filename_g = filename_a;
	keyw_cpp = is_cpp;

	ivt = 0;
	iva = 0;
	sIfstack[0].status = True;
	niveau = 0;

	if(f_ReadFile(yyfp))
	{
		return;
	}

	Declaration_t decl;
	while(token( 0 ) != 0)
	{
		if(decl = f_Declaration( 0 ))
		{
//            decodeDecl(decl);
			decodeDeclaration(decl);
		}
		else
		{
//			printf("Null declaration!\n");
			break;
		}
		if( niveau != 0 )
		{
#ifdef TEST
				printf( "bad niveau: %d (line:%d)\n", niveau, f_lineno( 0 ));
#endif
		}
	}

	f_read_end();
}
