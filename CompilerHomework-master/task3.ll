%top{
#include "task3.tab.hh"
#define YY_DECL yy::parser::symbol_type yylex()
}
%option noyywrap nounput batch noinput
%x QUOTE DBQUOTE
%%
([^<();|$ \\\n\t'"=]|\\.)+  return yy::parser::make_TEXT(yytext);   
\<\(                    return yy::parser::make_SHELLSTART(yytext);
\)                      return yy::parser::make_SHELLEND(yytext);
;                       return yy::parser::make_SEMI(yytext);
=                       return yy::parser::make_EQUALS(yytext);
\|                      return yy::parser::make_PIPE(yytext);
\n                      return yy::parser::make_NEWL(yytext);
[ \t]+                  return yy::parser::make_BLANK(yytext);
'[^']*'                  return yy::parser::make_QUOTED(yytext);
\"                                        BEGIN(DBQUOTE);
<DBQUOTE>[^$"]+                           return yy::parser::make_DBQUOTED(yytext);
<DBQUOTE>\"                               BEGIN(INITIAL);
<DBQUOTE,INITIAL>$[a-zA-Z0-9][a-zA-Z0-9_]*      return yy::parser::make_VAREXP(yytext);
<<EOF>>                 return yy::parser::make_END();
%%
