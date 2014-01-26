%{
/* This %{ ... %} section is code that is glued into the generated scanner. */
#include "parser.h"
#define YYDEBUG 1
extern int outputStage; // This variable is located in vslc.c

    #define RETURN(t) do {                                             \
        if ( outputStage == 1 )                                        \
            fprintf ( stderr, "TOKEN ( %d,\t'%s' )\n", t, yytext );    \
        return t;                                                      \
    } while ( 0 )
    
    
%}

%option array
%option noyywrap
%option yylineno

digit	[0-9]
letter	[A-Za-z]
ws		[ \t\n]
string	[A-Za-z]?\"(\\.|[^\\"])*\"
id		({letter}|_)({letter}|{digit}|_)*
float	{digit}+\.{digit}+
integer {digit}+
comment	"//"(.)*\n

%%
FUNC		{ RETURN( FUNC		); }
PRINT		{ RETURN( PRINT		); }
RETURN		{ RETURN( RETURN	); }
IF			{ RETURN( IF		); }
THEN		{ RETURN( THEN		); }
ELSE		{ RETURN( ELSE		); }
END			{ RETURN( END		); }
WHILE		{ RETURN( WHILE		); }
DO			{ RETURN( DO		); }
INT			{ RETURN( INT	 	); }
BOOL		{ RETURN( BOOL		); }
FLOAT		{ RETURN( FLOAT		); }
CLASS		{ RETURN( _CLASS_	); }
HAS			{ RETURN( HAS		); }
WITH		{ RETURN( WITH		); }
START		{ RETURN( START		); }
VOID		{ RETURN( VOID		); }
THIS		{ RETURN( THIS		); }
NEW			{ RETURN( NEW		); }

":="		{ RETURN( ASSIGN		); }  
"+"			{ RETURN( '+'			); }	
"-"			{ RETURN( '-'			); }
"*"			{ RETURN( '*'			); }
"/"			{ RETURN( '/'			); }
">"			{ RETURN( '>'			); }
"<"			{ RETURN( '<'			); }
"<="		{ RETURN( LEQUAL		); }
">="		{ RETURN( GEQUAL		); }
"=="		{ RETURN( EQUAL			); }
"!="		{ RETURN( NEQUAL		); }
"||"		{ RETURN( OR			); }
"&&"		{ RETURN( AND			); }

{integer}	{ RETURN( INT_CONST 	); 	}
{float}		{ RETURN( FLOAT_CONST	);	}
TRUE		{ RETURN( TRUE_CONST	);	}
FALSE		{ RETURN( FALSE_CONST	);	}
{string}	{ RETURN( STRING_CONST 	);	}

{id}		{ RETURN( IDENTIFIER	); 	}
{ws}		{ /*ignore*/				}
{comment}	{ /*ignore*/				}
.           { RETURN( yytext[0] 	);	}

%%
