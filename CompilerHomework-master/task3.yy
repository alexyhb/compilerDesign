%skeleton "lalr1.cc"
%defines
%define api.value.type variant
%define api.token.constructor
%code requires {
  #include "node.h"
}
%code provides {
  extern Node root;
}
%code{
  #include <string>
  #include <iostream>
  #define YY_DECL yy::parser::symbol_type yylex()
  YY_DECL;
  Node root;
}
%token <std::string> TEXT
%token <std::string> BLANK
%token <std::string> SEMI
%token <std::string> PIPE
%token <std::string> NEWL
%token <std::string> QUOTED
%token <std::string> DBQUOTED
%token <std::string> VAREXP
%token <std::string> EQUALS
%token <std::string> SHELLSTART
%token <std::string> SHELLEND
%type <Node> line
%type <Node> blanks
%type <Node> anything
%type <Node> command
%type <Node> subshell
%type <Node> normal_command
%type <Node> equals_command
%type <Node> equals
%type <Node> stream
%type <Node> optline
%type <Node> concatenate
%type <Node> concatenate2
%token END 0 "end of file"
%%
stream:     optline {
                $$ = Node("stream", "");
                $$.children.push_back($1);
            }
            | stream NEWL optline {
                $$ = $1;
                $$.children.push_back($3);
                root = $$;
            }
            ;

optline:    /* empty */ {
                $$ = Node("optline", "empty");
            }
            | line {
                $$ = Node("optline", "has line");
                $$.children.push_back($1);
            }
            ;

command:    normal_command { $$ = $1; } | equals_command { $$ = $1; }
            ;

line:       command { $$ = $1; }
            | equals_command BLANK normal_command {
                $$ = $1;
                $$.children.push_back($3);
            }
			| line PIPE command {
				$$ = Node("pipeline", "");
				$$.children.push_back($1);
				$$.children.push_back($3);
			}
			| line SEMI command {
				$$ = Node("line", "");
				$$.children.push_back($1);
				$$.children.push_back($3);
			}
            ;

equals:     anything EQUALS concatenate {
                $$ = Node("equals", "");
                $$.children.push_back($1);
                $$.children.push_back($3);
            }
			| blanks anything EQUALS concatenate {
				$$ = Node("equals", "");
                $$.children.push_back($2);
                $$.children.push_back($4);
			}
            ;

subshell:	SHELLSTART stream SHELLEND {
				$$ = Node("SUBSHELL", "");
				$$.children.push_back($2);
			}
			;

equals_command:	equals { $$ = $1; }
                | equals_command BLANK equals {
                    $$ = $1;
                    $$.children.push_back($3);
                }
                ;

normal_command:	anything {
                    $$ = Node("command", "");
                    $$.children.push_back($1);
                }
				| blanks anything {
					$$ = Node("command", "");
                    $$.children.push_back($2);
				}
                | normal_command blanks concatenate {
                    $$ = $1;
                    $$.children.push_back($3);
                }
				| normal_command blanks subshell {
					$$ = $1;
					$$.children.push_back($3);
				}
                ;

concatenate:    anything {
                    $$ = $1;
                }
                | anything anything concatenate2 {
                    $$ = $3;
                    $$.children.push_front($2);
                    $$.children.push_front($1);
                }
                ;

concatenate2: /* empty */ {
                $$ = Node("concatenate", "");
            }
            | concatenate2 anything {
                $$ = $1;
                $$.children.push_back($2);
            }
            ;

anything:   TEXT { $$ = Node("WORD", $1); }
            | QUOTED { $$ = Node("QUOTED", $1.substr(1, $1.length() - 2)); }
            | DBQUOTED { $$ = Node("DBQUOTED", $1); }
            | EQUALS { $$ = Node("EQUALS", $1); }
            | VAREXP { $$ = Node("VAREXP", $1.substr(1, $1.length() - 1)); }
            ;

blanks:		BLANK { /* ignore one or more blank */ }
			| blanks BLANK
			;