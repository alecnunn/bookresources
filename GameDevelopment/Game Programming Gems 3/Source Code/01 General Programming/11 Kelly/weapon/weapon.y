/*
 *  FILE       : weapon.y
 *
 *  PROGRAMMER : Paul Kelly 
 *               paul_kelly2000@yahoo.com
 *
 *  DESCRIPTION: The yacc specification for parsing the input data file.
 *               
 *
 *  NOTES      : bison was used to test this example.  It can
 *               be found at www.gnu.org in the gnu tools.
 *
 *               Use the following to generate the parser:
 *
 *               bison -d -o weapon_y.c weapon.y
 *
 *               -d generates a file with an enumeration of the tokens that
 *                  can be used by the lexer and it also defines the types
 *                  that can be returned by the lexer.
 *               -o filename that the parser generated source code
 *                  will be placed.
 *
 *               Also, make sure the necessary lib and .h files are included 
 *               so that compiling and linking work.
 *
 */

%{
/* C code section */

/*
 *
 *  INCLUDES
 *
 */
#include "weapon.h"
#include <string.h>
#include <stdio.h>

/*
 *
 *  MACROS
 *
 */

#ifdef _DEBUG  // do only in debug builds
#define YYDEBUG  1 // outputs debug info during parse
#endif

/*
 *
 *  GLOBAL VARIABLES
 *
 */

extern char *yytext;


%}

/*
 *
 *  TOKENS
 *
 */

// return types for tokens
%union
{
   int      integer;     // for INTEGER token
   char     string[80];  // for IDENTIFIER token
}

// used by the lexer as return values so that the parser knows
// which tokens have been found
%token TKN_START_WEAPON TKN_START_AMMO TKN_END TKN_WEAPON TKN_AMMO
%token TKN_AMMO_CLIP TKN_AMMO_TYPE TKN_FIRE_RATE 
%token TKN_DAMAGE

// tokens that can return a value from lex to yacc via yylval
%token <integer> TKN_INTEGER
%token <string> TKN_IDENTIFIER



%%
/*
 *
 *  YACC SPECIFICATION
 *
 */

// weapon_data is the start symbol
weapon_data:      /* lamda rule - empty rule */
                  | weapon_data weapon_section
                  | weapon_data ammo_section

weapon_section:	TKN_START_WEAPON weapon TKN_END
weapon:			   /* lamda rule - empty rule */   
                  |  weapon TKN_WEAPON TKN_IDENTIFIER
                        { strcpy (weapon_tbl[weapon_cnt], $3); }
                     weapon_attribute TKN_END
                        { weapon_cnt++; }
weapon_attribute:	/* lamda rule - empty rule */
                  | weapon_attribute ammo_clip
			         | weapon_attribute ammo_type
			         | weapon_attribute fire_rate

ammo_clip:		   TKN_AMMO_CLIP TKN_INTEGER 
                     {weapon [weapon_cnt].ammo_clip = $2;}
ammo_type:		   TKN_AMMO_TYPE TKN_IDENTIFIER
                     {strcpy (weapon [weapon_cnt].ammo_type, $2);}
fire_rate:		   TKN_FIRE_RATE TKN_INTEGER
                     {weapon [weapon_cnt].fire_rate = $2;}

ammo_section:     TKN_START_AMMO ammo TKN_END
ammo:			      /* lamda rule - empty rule */
                  |  ammo TKN_AMMO TKN_IDENTIFIER
                        { strcpy (ammo_tbl[ammo_cnt], $3);}
                     ammo_attribute TKN_END
                        { ammo_cnt++;}
ammo_attribute:   /* lamda rule - empty rule */
                  | ammo_attribute TKN_DAMAGE TKN_INTEGER
                     {ammo [ammo_cnt].damage = $3;}

%%
/* C code section for yacc specification */

/*
 *-----------------------------------------------------------------------------
 *
 * FUNCTION: yyerror
 *    Call if a parse error is encountered.
 *    
 *
 *
 * RETURN:
 *    0 to signal error and stop parse.
 *
 * OUTPUTS:
 *    none
 *
 * INPUTS:
 *    none
 *
 *-----------------------------------------------------------------------------
 */
int yyerror (const char *msg)
{
   printf ("%s at '%s'\n", msg, yytext);

   return 0;
}