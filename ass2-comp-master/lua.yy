%skeleton "lalr1.cc" 
%defines
%define api.value.type variant
%define api.token.constructor

%code requires {
    #include "node.hh"
}

%code provides {
    extern StmtList *root;
    extern FILE *yyin;
}

%code{
    #define YY_DECL yy::parser::symbol_type yylex()
    YY_DECL;
    StmtList *root;
}

%token <std::string>                NAME STRING NUM
                                    TRUE FALSE
                                    ASSIGN COMMA
                                    ADD SUB MUL DIV POW MOD EQ LEN
                                    LP RP LB RB LC RC LT GT
                                    FOR DO ED IF THEN ELSE REPEAT UNTIL RETURN FUNCTION
%type <Exp*>                        exp exp5 exp4 exp3 exp2 exp1 var table funcall
%type <ExpList*>                    varlist explist
%type <Stmt*>                       stmt laststmt
%type <StmtList*>                   seq seq2
%token END 0 "end of file"

%%
seq:                stmt {
                        $$ = new StmtList();
                        $$->children.push_back($1);
                        root = $$;
                    }
                    | seq stmt {
                        $$ = $1;
                        $$->children.push_back($2);
                        root = $$;
                    }
                    | seq2 {
                        $$ = $1;
                        root = $$;
                    }
                    ;
seq2:               laststmt {
                        $$ = new StmtList();
                        $$->children.push_back($1);
                    }
                    | seq laststmt {
                        $$ = $1;
                        $$->children.push_back($2);
                    }
                    ;

laststmt:           RETURN exp { $$ = new LastStmt($2); }
                    ;

stmt:               funcall { $$ = new FunCallStmt($1); }
                    | varlist ASSIGN explist { $$ = new Assignment($1, $3); }
                    | FOR NAME ASSIGN exp COMMA exp DO seq ED { $$ = new For($2, $4, $6, $8); }
                    | IF exp THEN seq ELSE seq ED { $$ = new IfThenElse($2, $4, $6); }
                    | IF exp THEN seq ED { $$ = new IfThen($2, $4); }
                    | REPEAT seq UNTIL exp { $$ = new Repeat($2, $4); }
                    | FUNCTION NAME LP NAME RP seq ED { $$ = new FuncDef($2, $4, $6); }
                    ;

exp:                exp GT exp5 { $$ = new Great($1, $3); }
                    | exp LT exp5 { $$ = new Less($1, $3); }
                    | exp EQ exp5 { $$ = new Equality($1, $3); }
                    | exp5 { $$ = $1; }
                    ;

exp5:                 exp5 ADD exp4 { $$ = new Add($1, $3); }
                    | exp5 SUB exp4 { $$ = new Sub($1, $3); }
                    | exp4 { $$ = $1; }
                    ;

exp4:                 exp4 MUL exp3 { $$ = new Mul($1, $3); }
                    | exp4 DIV exp3 { $$ = new Div($1, $3); }
                    | exp4 MOD exp3 { $$ = new Mod($1, $3); }
                    | exp3 { $$ = $1; }
                    ;

exp3:               LEN exp2 { $$ = new Len($2); }
                    | exp2 { $$ = $1; }
                    ;

exp2:                 exp2 POW exp1 { $$ = new Pow($1, $3); }
                    | exp1 { $$ = $1; }
                    ;

exp1:               FALSE { $$ = new Boolea($1); }
                    | TRUE { $$ = new Boolea($1); }
                    | NUM { $$ = new Num($1); }
                    | STRING { $$ = new String($1); }
                    | LP exp RP { $$ = $2; }
                    | funcall { $$ = $1; }
                    | var { $$ = $1; }
                    | table { $$ = $1; }
                    ;

table:              LC RC { $$ = new Table(); }
                    | LC explist RC { $$ = new Table($2); }
                    ;

funcall:            NAME STRING { $$ = new FunCall($1, new String($2)); }
                    | NAME LP explist RP { $$ = new FunCall($1, $3); }
                    ;

var:                  NAME { $$ = new Var($1); }
                    | NAME LB exp RB { $$ = new SuffixVar($1, $3); }
                    ;

varlist:            var {
                        $$ = new ExpList();
                        $$ -> children.push_back($1);
                    }
                    | varlist COMMA var {
                        $$ = $1;
                        $$ -> children.push_back($3);
                    }
                    ;
explist:            exp {
                        $$ = new ExpList();
                        $$ -> children.push_back($1);
                    }
                    | explist COMMA exp {
                        $$ = $1;
                        $$ -> children.push_back($3);
                    }
                    ;