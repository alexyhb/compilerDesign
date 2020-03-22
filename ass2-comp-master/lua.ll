%top{
#include "lua.tab.hh"
#define YY_DECL yy::parser::symbol_type yylex()
}

%option noyywrap nounput batch noinput

%%
([1-9][0-9]*)|0|([0-9]+\.[0-9]+)        return yy::parser::make_NUM(yytext);

"true"                                  return yy::parser::make_TRUE(yytext);
"false"                                 return yy::parser::make_FALSE(yytext);

"for"                                   return yy::parser::make_FOR(yytext);
"do"                                    return yy::parser::make_DO(yytext);
"end"                                   return yy::parser::make_ED(yytext);
"if"                                    return yy::parser::make_IF(yytext);
"then"                                  return yy::parser::make_THEN(yytext);
"else"                                  return yy::parser::make_ELSE(yytext);
"repeat"                                return yy::parser::make_REPEAT(yytext);
"until"                                 return yy::parser::make_UNTIL(yytext);
"return"                                return yy::parser::make_RETURN(yytext);
"function"                              return yy::parser::make_FUNCTION(yytext);

"="                                     return yy::parser::make_ASSIGN(yytext);
"=="                                    return yy::parser::make_EQ(yytext);
","                                     return yy::parser::make_COMMA(yytext);
"+"                                     return yy::parser::make_ADD(yytext);
"-"                                     return yy::parser::make_SUB(yytext);
"*"                                     return yy::parser::make_MUL(yytext);
"/"                                     return yy::parser::make_DIV(yytext);
"^"                                     return yy::parser::make_POW(yytext);
"%"                                     return yy::parser::make_MOD(yytext);
"#"                                     return yy::parser::make_LEN(yytext);

"<"                                     return yy::parser::make_LT(yytext);
">"                                     return yy::parser::make_GT(yytext);

"["                                     return yy::parser::make_LB(yytext);
"]"                                     return yy::parser::make_RB(yytext);

"("                                     return yy::parser::make_LP(yytext);
")"                                     return yy::parser::make_RP(yytext);

"{"                                     return yy::parser::make_LC(yytext);
"}"                                     return yy::parser::make_RC(yytext);

[A-Za-z_][A-Za-z0-9_.]*                 return yy::parser::make_NAME(yytext);
\"[^"]*\"                               return yy::parser::make_STRING(yytext);

\n                                      /* munch */
" "                                     /* munch */

<<EOF>>                                 return yy::parser::make_END();
%%