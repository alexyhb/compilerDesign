%skeleton "lalr1.cc" 
%defines
%define api.value.type variant
%define api.token.constructor

%code requires {
#include "node.hh"
}

%code provides {
extern Node root;
extern FILE *yyin;
}

%code{
    unsigned int total = 0;
    #include <string>
    #define YY_DECL yy::parser::symbol_type yylex()
    YY_DECL;
    Node root;
}
%token <std::string>    ADD SUB MUL DIV POW MOD CONCAT LT LE LB RB GT GE
                        EQ NE LEN DOT NAME SEMICOLON ASSIGN AND OR NOT LP RP
                        BREAK DO ELSE ELSEIF ED FALSE FOR FUNCTION IF NUMBER
                        IN NIL REPEAT RETURN THEN TRUE UNTIL WHILE STRING
                        COMMA THREEDOTS COLON LC RC
%type <std::string>     fieldsep
%type <Node>            explist chunk chunk2 stat block var laststat
                        exp exp_unop exp3 prefixexp varlist
                        binop unop namelist parlist function funcbody
                        functioncall args tableconstructor
                        fieldlist field fieldlist2 funcname exp_a exp_b exp_x
%token END 0 "end of file"
%%
chunk:          stat {
                    $$ = Node("chunk", "");
                    $$.children.push_back($1);
                    root = $$;
                }
                | chunk stat {
                    $$ = $1;
                    $$.children.push_back($2);
                    root = $$;
                }
                | chunk2 {
                    $$ = $1;
                    root = $$;
                }
                ;

chunk2:         laststat {
                    $$ = Node("chunk", "");
                    $$.children.push_back($1);
                }
                | chunk laststat {
                    $$ = $1;
                    $$.children.push_back($2);
                }
                ;

block:          chunk {
                    $$ = Node("block", "");
                    $$.children.push_back($1);
                }
                ;

stat:           varlist ASSIGN explist {
                    $$ = Node("assign_stat", "");
                    $$.children.push_back($1);
                    $$.children.push_back($3);
                }
                | var args {
                    $$ = Node("fun_call_stat", "");
                    $$.children.push_back($1);
                    $$.children.push_back($2);
                }
                | REPEAT block UNTIL exp {
                    $$ = Node("repeat_stat", "");
                    $$.children.push_back($2);
                    $$.children.push_back($4);
                }
                | IF exp THEN block ED {
                    $$ = Node("if_stat", "");
                    $$.children.push_back($2);
                    $$.children.push_back($4);
                }
                | IF exp THEN block ELSE block ED {
                    $$ = Node("if_stat", "");
                    $$.children.push_back($2);
                    $$.children.push_back($4);
                    $$.children.push_back($6);
                }
                | FOR NAME ASSIGN exp COMMA exp DO block ED {
                    $$ = Node("for_stat", "");
                    $$.children.push_back(Node("var", $2));
                    $$.children.push_back($4);
                    $$.children.push_back($6);
                    $$.children.push_back($8);
                }
                | FUNCTION funcname funcbody {
                    $$ = Node("func_stat", "");
                    $$.children.push_back($2);
                    $$.children.push_back($3);
                }
                ;

laststat:       RETURN explist {
                    $$ = Node("laststat", "");
                    $$.children.push_back($2);
                }
                ;

funcname:       NAME {
                    $$ = Node("funcname", "");
                    $$.children.push_back(Node("NAME", $1));
                }
                | funcname DOT NAME {
                    $$ = $1;
                    $$.children.push_back(Node("DOT", $2));
                    $$.children.push_back(Node("NAME", $3));
                }
                ;

varlist:        var {
                    $$ = Node("varlist", "");
                    $$.children.push_back($1);
                }
                | varlist COMMA var {
                    $$ = $1;
                    $$.children.push_back($3);
                }
                ;

var:            NAME {
                    $$ = Node("var", $1);
                }
                | var DOT NAME {
                    $$ = $1;
                    $$.value += "." + $3;
                }
                | var LB exp RB {
                    $$ = Node("barket_var", "");
                    $$.children.push_back($1);
                    $$.children.push_back($3);
                }
                ;

namelist:       NAME {
                    $$ = Node("namelist", "");
                    $$.children.push_back(Node("NAME", $1));
                }
                | namelist COMMA NAME {
                    $$ = $1;
                    $$.children.push_back(Node("NAME", $3));
                }
                ;

explist:        exp {
                    $$ = Node("explist", "");
                    $$.children.push_back($1);
                }
                | explist COMMA exp {
                    $$ = $1;
                    $$.children.push_back($3);
                }
                ;

exp:            exp_x { $$ = $1; }
                | exp EQ exp_x {
                    $$ = Node("exp", "eq");
                    $$.children.push_back($1);
                    $$.children.push_back($3);
                }
                | exp GT exp_x {
                    $$ = Node("exp", "gt");
                    $$.children.push_back($1);
                    $$.children.push_back($3);
                }
                | exp LT exp_x {
                    $$ = Node("exp", "lt");
                    $$.children.push_back($1);
                    $$.children.push_back($3);
                }
                ;

exp_x:          exp_b {
                    $$ = $1;
                }
                | exp_x binop exp_b {
                    $$ = Node("exp", "");
                    $$.children.push_back($1);
                    $$.children.push_back($2);
                    $$.children.push_back($3);
                }
                | exp_x ADD exp_b {
                    $$ = Node("exp", "add");
                    $$.children.push_back($1);
                    $$.children.push_back($3);
                }
                | exp_x SUB exp_b {
                    $$ = Node("exp", "sub");
                    $$.children.push_back($1);
                    $$.children.push_back($3);
                }
                | exp_x MOD exp_b {
                    $$ = Node("exp", "mod");
                    $$.children.push_back($1);
                    $$.children.push_back($3);
                }
                ;

exp_b:          exp_unop {
                    $$ = $1;
                }
                | exp_b MUL exp_unop {
                    $$ = Node("exp", "mul");
                    $$.children.push_back($1);
                    $$.children.push_back($3);
                }
                | exp_b DIV exp_unop {
                    $$ = Node("exp", "div");
                    $$.children.push_back($1);
                    $$.children.push_back($3);
                }
                ;

exp_unop:       exp_a { $$ = $1; }
                | unop exp_a {
                    $$ = Node("exp", "unop");
                    $$.children.push_back($1);
                    $$.children.push_back($2);
                }
                ;

exp_a:          exp3 { $$ = $1; }
                | exp_a POW exp3 {
                    $$ = Node("exp", "pow");
                    $$.children.push_back($1);
                    $$.children.push_back($3);
                }
                ;

                ;
exp3:           FALSE {
                    $$ = Node("exp", $1);
                }
                | TRUE {
                    $$ = Node("exp", $1);
                }
                | NUMBER {
                    $$ = Node("num_exp", $1);
                }
                | STRING {
                    $$ = Node("STRING", $1.substr(1, $1.length() - 2));
                }
                | THREEDOTS {
                    $$ = Node("exp", "");
                    $$.children.push_back(Node("THREEDOTS", $1));
                }
                | function {
                    $$ = Node("exp", "");
                    $$.children.push_back($1);
                }
                | prefixexp {
                    $$ = $1;
                }
                | tableconstructor {
                    $$ = Node("exp", "table");
                    $$.children.push_back($1);
                }
                ;

prefixexp:      var { $$ = $1; }
                | functioncall { $$ = $1; }
                | LP exp RP { $$ = $2; }
                ;

functioncall:   var args {
                    $$ = Node("functioncall", "");
                    $$.children.push_back($1);
                    $$.children.push_back($2);
                }
                ;

args:           LP explist RP { $$ = $2; }
                | STRING { $$ = Node("STRING", $1.substr(1, $1.length() - 2)); }
                ;

function:       FUNCTION funcbody {
                    $$ = Node("function", "");
                    $$.children.push_back(Node("FUNCTION", $1));
                    $$.children.push_back($2);
                }
                ;

funcbody:       LP RP block ED {
                    $$ = Node("funcbody", "");
                    $$.children.push_back($3);
                }
                | LP parlist RP block ED {
                    $$ = Node("funcbody", "");
                    $$.children.push_back($2);
                    $$.children.push_back($4);
                }
                ;

parlist:        namelist {
                    $$ = Node("parlist", "");
                    $$.children.push_back($1);
                }
                | THREEDOTS {
                    $$ = Node("parlist", "");
                    $$.children.push_back(Node("THREEDOTS", $1));
                }
                | namelist COMMA THREEDOTS {
                    $$ = Node("parlist", "");
                    $$.children.push_back($1);
                    $$.children.push_back(Node("THREEDOTS", $3));
                }
                ;

tableconstructor:LC RC {
                    $$ = Node("fieldlist", "");
                }
                | LC fieldlist RC {
                    $$ = $2;
                }
                ;

fieldlist:      fieldlist2 { $$ = $1; }
                | fieldlist2 fieldsep { $$ = $1; }
                ;
fieldlist2:     field {
                    $$ = Node("fieldlist", "");
                    $$.children.push_back($1);
                }
                | fieldlist2 fieldsep field {
                    $$ = $1;
                    $$.children.push_back($3);
                }
                ;
field:          exp {
                    $$ = $1;
                }
                ;
fieldsep:       COMMA | SEMICOLON
                ;

binop:          CONCAT { $$ = Node("binop", $1); }
                | LE { $$ = Node("binop", $1); }
                | GE { $$ = Node("binop", $1); }
                | NE { $$ = Node("binop", $1); }
                | AND { $$ = Node("binop", $1); }
                | OR { $$ = Node("binop", $1); }
                ;

unop:           SUB { $$ = Node("unop", $1); }
                | NOT { $$ = Node("unop", $1); }
                | LEN { $$ = Node("unop", $1); }
                ;
