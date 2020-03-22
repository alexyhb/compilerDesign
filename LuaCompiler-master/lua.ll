%top{
#include "lua.tab.hh"
#define YY_DECL yy::parser::symbol_type yylex()
}

%option noyywrap nounput batch noinput

%%
([1-9][0-9]*)|0|([0-9]+\.[0-9]+)    return yy::parser::make_NUMBER(yytext);
"and"                               return yy::parser::make_AND(yytext);
"or"                                return yy::parser::make_OR(yytext);
"not"                               return yy::parser::make_NOT(yytext);
"break"                             return yy::parser::make_BREAK(yytext);
"do"                                return yy::parser::make_DO(yytext);
"else"                              return yy::parser::make_ELSE(yytext);
"elseif"                            return yy::parser::make_ELSEIF(yytext);
"end"                               return yy::parser::make_ED(yytext);
"false"                             return yy::parser::make_FALSE(yytext);
"for"                               return yy::parser::make_FOR(yytext);
"function"                          return yy::parser::make_FUNCTION(yytext);
"if"                                return yy::parser::make_IF(yytext);
"in"                                return yy::parser::make_IN(yytext);
"nil"                               return yy::parser::make_NIL(yytext);
"repeat"                            return yy::parser::make_REPEAT(yytext);
"return"                            return yy::parser::make_RETURN(yytext);
"then"                              return yy::parser::make_THEN(yytext);
"true"                              return yy::parser::make_TRUE(yytext);
"until"                             return yy::parser::make_UNTIL(yytext);
"while"                             return yy::parser::make_WHILE(yytext);
[A-Za-z_][A-Za-z0-9_]*              return yy::parser::make_NAME(yytext);
"..."                               return yy::parser::make_THREEDOTS(yytext);
".."                                return yy::parser::make_CONCAT(yytext);
"."                                 return yy::parser::make_DOT(yytext);
"="                                 return yy::parser::make_ASSIGN(yytext);
"("                                 return yy::parser::make_LP(yytext);
")"                                 return yy::parser::make_RP(yytext);
"["                                 return yy::parser::make_LB(yytext);
"]"                                 return yy::parser::make_RB(yytext);
"{"                                 return yy::parser::make_LC(yytext);
"}"                                 return yy::parser::make_RC(yytext);
","                                 return yy::parser::make_COMMA(yytext);
";"                                 return yy::parser::make_SEMICOLON(yytext);
":"                                 return yy::parser::make_COLON(yytext);
"+"                                 return yy::parser::make_ADD(yytext);
"-"                                 return yy::parser::make_SUB(yytext);
"*"                                 return yy::parser::make_MUL(yytext);
"/"                                 return yy::parser::make_DIV(yytext);
"^"                                 return yy::parser::make_POW(yytext);
"%"                                 return yy::parser::make_MOD(yytext);
"<"                                 return yy::parser::make_LT(yytext);
"<="                                return yy::parser::make_LE(yytext);
">"                                 return yy::parser::make_GT(yytext);
">="                                return yy::parser::make_GE(yytext);
"=="                                return yy::parser::make_EQ(yytext);
"~="                                return yy::parser::make_NE(yytext);
"#"                                 return yy::parser::make_LEN(yytext);
\"[^"]*\"                            return yy::parser::make_STRING(yytext);
\n                                  /* munch */
" "                                 /* munch */
<<EOF>>                             return yy::parser::make_END();
%%