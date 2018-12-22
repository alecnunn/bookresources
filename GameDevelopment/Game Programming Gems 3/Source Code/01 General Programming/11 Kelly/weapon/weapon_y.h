#ifndef BISON_WEAPON_Y_H
# define BISON_WEAPON_Y_H

#ifndef YYSTYPE
typedef union
{
   int      integer;     // for INTEGER token
   char     string[80];  // for IDENTIFIER token
} yystype;
# define YYSTYPE yystype
#endif
# define	TKN_START_WEAPON	257
# define	TKN_START_AMMO	258
# define	TKN_END	259
# define	TKN_WEAPON	260
# define	TKN_AMMO	261
# define	TKN_AMMO_CLIP	262
# define	TKN_AMMO_TYPE	263
# define	TKN_FIRE_RATE	264
# define	TKN_DAMAGE	265
# define	TKN_INTEGER	266
# define	TKN_IDENTIFIER	267


extern YYSTYPE yylval;

#endif /* not BISON_WEAPON_Y_H */
